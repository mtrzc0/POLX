# **TrytonOS i386**
The goal of this project is to create a simple one core concurrent operating system that allows you to understand how it works. It will be a ready-made model of the system with basic components installed. The included documentation should provide easy understanding of how the basic components of operating systems work together. This activity is intended to broaden practical knowledge of operating system design.

![](./doc/drawings/trytonos_screenshot.png)

## Roadmap
- [x] **Kernel**
    - [x] Main function
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
        - [x] Virtual memory manager
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
        - [x] Signals (inside kernel only)
        - [ ] Shared memory
    - [x] Device drivers
        - [x] Terminal
        - [x] Timer
        - [x] PIC
        - [x] Keyboard
- [x] **Basic standard C library**
    - [x] Programs loader
    - [x] Syscalls wrappers
    - [x] Basic functions
        - [x] malloc
        - [x] free
        - [x] printf
        - [x] perror
        - [x] readline
        - [x] strlen
- [x] **Operating system**
    - [x] Init
    - [x] User shell
    - [x] Shell misc programs
