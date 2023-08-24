#ifndef __elf_loader_dot_H
#define __elf_loader_dot_H

#include <kernel/vmm.h>
#include <kernel/vfs.h>

int elf_load(vmm_aspace_t *as, vfs_node_ptr_t file);

#endif
