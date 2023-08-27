#include <kernel/elf_loader.h>
#include <kernel/syscalls.h>
#include <kernel/signal.h>
#include <kernel/namei.h>
#include <kernel/klib.h>
#include <kernel/task.h>
#include <kernel/arch.h>
#include <kernel/vmm.h>
#include <kernel/vfs.h>
#include <kernel/fd.h>

#include <errno.h>

extern vfs_node_ptr_t dev_zero;
extern task_t *kernel_task;
extern task_t *actual_task;
extern int errno;

int do_execve(task_t *t, char *path, char *const argv[])
{
	vmm_aspace_t *as;
	vfs_node_ptr_t exe_file, parent;
	uintptr_t stack_start, stack_end, stack_ptr_reg;
	uintptr_t argv_user_stack[256];
	size_t stack_size, argv_ctr;
	char *string;
	

	exe_file = namei_path_to_vfs_node(NULL, path, &parent);
	/* File does not exist */
	if (exe_file == NULL) {
		errno = ENOENT;
		return -1;
	}

	/* 
		Create new AS with loaded executable 
	*/
	as = (vmm_aspace_t *)vmm_aspace_create(NULL);
	/* Error when loading ELF file */
	if (elf_load(as, exe_file) < 0) {
		/* errno already set by elf_load() */
		return -1;
	}

	/* 
		Create process stack 
		
		******************** stack_end   0xbfffd000
		********************
		********************
		******************** stack_start 0xbffff000
	*/
	stack_start = VM_USER_STACK;
	stack_size = PAGE_SIZE * 2;
	stack_end = stack_start - stack_size;
	/* Fill stack with zeroes */
	vmm_mmap_at(as, stack_end, dev_zero, 0, stack_size, 0, VMM_RW | VMM_USER);
	vmm_set_stack(as, stack_end, stack_size);

	/*
		Copy argv into process stack

		Becareful you are working on stack memory here,
		so keep in mind that addresses are going down
	*/
	argv_ctr = 0;
	stack_ptr_reg = stack_start;

	while (argv[argv_ctr] != NULL && argv_ctr < 255) {
		string = vmm_copy_string_from(t->aspace, (uintptr_t)argv[argv_ctr], 1024);
		if (string == NULL) {
			errno = EFAULT;
			return -1;
		}
		stack_ptr_reg -= strlen(string)+1;

		if (vmm_copy_data_to(as, stack_ptr_reg, string, strlen(string)+1) < 0) {
			errno = EFAULT;
			return -1;
		}
		argv_user_stack[argv_ctr] = stack_ptr_reg;

		argv_ctr++;
		kfree(string);
	}
	argv_user_stack[argv_ctr] = (uintptr_t)NULL;
	argv_ctr += 1;

	/* Align down stack_ptr_reg before copying argv array */
	if (stack_ptr_reg % 4 != 0) {
		stack_ptr_reg = (stack_ptr_reg + stack_ptr_reg % 4) - 4;
	}
	stack_ptr_reg -= sizeof(uintptr_t)*argv_ctr;

	if (vmm_copy_data_to(as, stack_ptr_reg, argv_user_stack, sizeof(uintptr_t)*argv_ctr) < 0) {
		errno = EFAULT;
		return -1;
	}
	
	/*
		Replace task address space and destroy old one
	*/
	vmm_aspace_destroy(t->aspace);
	t->aspace = as;
	regs_set_pd(t->regs, as->pd);

	/*
		Close non default file descriptors
	*/
	t->used_fd[0] = kernel_task->used_fd[0];	
	t->used_fd[1] = kernel_task->used_fd[1];	
	
	for (size_t i=2; i < MAX_FDS && t->fd_ctr > 2; i++) {
		if (t->used_fd[i] != NULL) {
			do_close(t, i);
		}
	}

	/*
		Restore default signal mask
	*/
	t->mask = SIG_DEFAULT_MASK;

	/*
		Update task registers
	*/
	regs_set_execve_args(t->regs, argv_ctr, stack_ptr_reg);
	regs_set_instruction_pointer(t->regs, t->aspace->code_entry);
	
	stack_ptr_reg -= sizeof(uintptr_t) * argv_ctr;
	regs_set_stack_pointer(t->regs, stack_ptr_reg);

	return 0;
}

int execve(const char *path, char *const argv[])
{
	int ret;
	uintptr_t paddr, old_tmp;
	char *path_in_kernel_as;
	char **argv_in_kernel_as, **argv_in_user_as;
	size_t i, i_after_next_page;

	path_in_kernel_as = vmm_copy_string_from(actual_task->aspace, (uintptr_t)path, 1024);
	if (path_in_kernel_as == NULL) {
		errno = E2BIG;
		return -1;
	}

	/* Copy whole argv content into kernel AS */
	paddr = vmm_get_frame_from(actual_task->aspace->pd, argv);
	if (!paddr) {
		errno = EFAULT;
		return -1;
	}
	
	old_tmp = vmm_unmap(VM_TMP_MAP);
	vmm_map(paddr, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
	argv_in_user_as = (char **)(VM_TMP_MAP + ((uintptr_t)argv % PAGE_SIZE));
	argv_in_kernel_as = (char **)kmalloc(sizeof(char *)*256);
	
	i = 0;
	i_after_next_page = 0;
	while (i < 256) {
		/* Map next page when crossing page boundary */
		if ((uintptr_t)&argv_in_user_as[i-i_after_next_page] % PAGE_SIZE == 0) {
			argv += i;
			vmm_unmap(VM_TMP_MAP);
			paddr = vmm_get_frame_from(actual_task->aspace->pd, argv);
			if (!paddr) {
				kfree(path_in_kernel_as);
				kfree(argv_in_kernel_as);
				vmm_map(old_tmp, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
				errno = EFAULT;
				return -1;
			}
			vmm_map(paddr, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
			argv_in_user_as = (char **)VM_TMP_MAP;
			
			i_after_next_page = i;
		}

		argv_in_kernel_as[i] = argv_in_user_as[i - i_after_next_page];
		if (argv_in_user_as[i - i_after_next_page] == NULL) {
			break;
		}
		i++;
	}
	vmm_unmap(VM_TMP_MAP);
	vmm_map(old_tmp, VM_TMP_MAP, PG_KERN_PAGE_FLAG);

	ret = do_execve(actual_task, path_in_kernel_as, argv_in_kernel_as);
	kfree(path_in_kernel_as);
	kfree(argv_in_kernel_as);

	return ret;
}
