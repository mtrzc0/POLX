#include <elf32.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

#include <kernel/vmm.h>
#include <kernel/vfs.h>
#include <kernel/klib.h>

extern int errno;
extern vfs_node_ptr_t dev_zero;

static Elf32_Ehdr *_load_Ehdr(vfs_node_ptr_t file)
{
	Elf32_Ehdr *hdr;

	hdr = (Elf32_Ehdr *)kmalloc(sizeof(Elf32_Ehdr));
	/* Error when reading file */
	if (vfs_read(file, 0, sizeof(Elf32_Ehdr), (char*)hdr) < 0) {
		kfree(hdr);
		return NULL;
	}

	return hdr;
}

static bool _is_elf_file(Elf32_Ehdr *hdr)
{
	if (hdr == NULL) return false;
	if (hdr->e_ident[EI_MAG0] != ELFMAG0) return false;
	if (hdr->e_ident[EI_MAG1] != ELFMAG1) return false;
	if (hdr->e_ident[EI_MAG2] != ELFMAG2) return false;
	if (hdr->e_ident[EI_MAG3] != ELFMAG3) return false;

	return true;
}

static int _load_exe_format(vmm_aspace_t *as, vfs_node_ptr_t file, Elf32_Ehdr *hdr)
{
	Elf32_Word flags;
	Elf32_Phdr *phdr;
	size_t tmp_sz;
	uintptr_t tmp_vaddr;
	int ret;

	/* Load program headers table into memory */
	phdr = (Elf32_Phdr *)kmalloc(sizeof(Elf32_Phdr) * hdr->e_phnum);
	if (vfs_read(file, hdr->e_phoff, sizeof(Elf32_Phdr) * hdr->e_phnum, (char*)phdr) < 0)
		return -1;

	/* Move each PT_LOAD type segment to target address space */
	for (Elf32_Half i=0; i < hdr->e_phnum; i++) {
		flags = 0;
		if (phdr[i].p_type != PT_LOAD)
			continue;
		
		/* Build flags for VMM */
		flags |= VMM_USER;
		if (phdr[i].p_flags & PF_W)
			flags |= VMM_RW;
		
		/* Copy file to memory and fill gap with 0's */
		if (phdr[i].p_memsz > phdr[i].p_filesz) {
			ret = vmm_mmap_at(as, phdr[i].p_vaddr, file, phdr[i].p_offset,
						       phdr[i].p_filesz, 0, flags);

			if (ret < 0)
				return ret;

			tmp_sz = phdr[i].p_memsz - phdr[i].p_filesz;
			tmp_vaddr = phdr[i].p_vaddr + phdr[i].p_filesz;
			ret = vmm_mmap_at(as, tmp_vaddr, dev_zero, 0,
						tmp_sz, phdr[i].p_align, flags);

			if (ret < 0)
				return ret;
		} else {
			/* Copy file to memory */
			ret = vmm_mmap_at(as, phdr[i].p_vaddr, file, phdr[i].p_offset,
					      phdr[i].p_filesz, phdr[i].p_align , flags);

			if (ret < 0)
				return ret;
			
			/* End of last loaded segment */
			tmp_vaddr = phdr[i].p_vaddr + phdr[i].p_memsz;
		}
	}

	as->code_entry = hdr->e_entry;
	as->data_end = tmp_vaddr;

	kfree(phdr);
	return 0;
}

int elf_load(vmm_aspace_t *as, vfs_node_ptr_t file)
{
	Elf32_Ehdr *hdr;
	int sub_ret;

	/* Error when loading header */
	hdr = _load_Ehdr(file);
	if (hdr == NULL)
		return -1;

	/* Not an ELF file */
	if (! _is_elf_file(hdr)) {
		errno = EEXIST;
		return -1;
	}

	switch(hdr->e_type) {
	case ET_EXEC:
		sub_ret = _load_exe_format(as, file, hdr);
		break;
	
	case ET_REL:
		/* Not implemented */
	case ET_NONE:
	default:
		errno = ENOEXEC;
		return -1;
	}

	kfree(hdr);
	return sub_ret;
}
