# ***Simple IPC mechanism***
Table of contents:   
0. [What is signal](#what-is-signal)   

1. [API](#api)   
- [Send signal](#send-signal)   
- [Signal handler](#signal-handler)   

# What is signal
Signal it's just command sent to process by kernel itself or other process. Each TCB has fields responsible for storing informations how process will handle certain signals.   

    signal mask - bitmap of signals that process want to hanlde
    signal current - bitmap of currently handled signal
    
There are some mandatory signals that must be handled instantly i.e SIGKILL, SIGSEGV. That signals are handled by kernel which kill receiver process and return exit code to process parent. Some not mandatory to handle signals i.e SIGCHLD will be handled only if receiver signal mask permit it (process is waiting for child termination). Signals are handled when scheduler chose receiver process to run.   

List of signals:   
- SIGKILL - Terminate process instantly   
- SIGSEGV - Terminate process instantly   
- SIGCHLD - Notify parent about terminated child   

# <kernel/signal.h>
## API
### Send signal
```c
/* Return 0 if signal was send or -1 and set errno when failed */
int sig_send(task_t *dst, sig_t signal);
```

### Signal handler
```c
void sig_handler(task_t *t);
```
