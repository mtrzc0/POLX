# ***Init Ram Filesystem***
Initrd is a readonly filesystem and it is based on tar archive. In fact that this OS does not have support for second storage, initrd is threated as hard disk containing all system and user files.

# _<kernel/initrd.h>_
## Initialization
```c
void initrd_load(uintptr_t start_of_initrd);
```
Initialize initrd filesystem and mount it at '/'
