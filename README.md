# **TrytonOS i386**
The goal of this project is to create a simple one core concurrent operating system that allows you to understand how it works. It will be a ready-made model of the system with basic components installed. The included documentation should provide easy understanding of how the basic components of operating systems work together. This activity is intended to broaden practical knowledge of operating system design.

## Roadmap
- [ ] Kernel
    - [ ] Main function
    - [x] Architecture depend
        - [x] Bootstraping
        - [x] GTD
        - [x] IDT
            - [x] Exceptions
            - [x] IRQ
            - [x] Syscalls
        - [x] Paging
            - [x] On demand paging (without second storage support)
        - [x] Ring 3
    - [x] Memory management
        - [x] Physical memory manager
        - [ ] Virtual memory manager
    - [x] CPU management
        - [x] Core abstraction
        - [x] Scheduler
        - [x] ELF loader
    - [x] Virtual file system
        - [x] FS abstraction
        - [x] File descriptors
        - [x] Namei
        - [x] Initrd
        - [x] Ramfs
    - [ ] Inter-process communication
        - [x] Signals
        - [ ] Shared memory
    - [ ] Device drivers
        - [x] VGA
        - [x] Timer
        - [x] PIC
        - [ ] Keyboard
- [ ] Basic standard C library
- [ ] User shell
