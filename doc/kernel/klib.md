# **Basic built-in libc functionality**

## ***kernel/klib.h***

### _<kernel/klib/stdio.h>_
```c
* void kprintf(const char *fmt, ...); // %d, %x, %s
* void panic(const char *msg);
* void outb(uint16_t port, uint8_t data);
* uint8_t inb(uint16_t port);
```

### _<kernel/klib/stdlib.h>_
```c
* void *kmalloc(size_t size);
* void kfree(void *ptr);
* void *vmalloc(size_t size, vmalloc_req_t phy_type);
* void vfree(void *ptr);
```

### _<kernel/klib/string.h>_
```c
* size_t strlen(const char *str);
* char *strncpy(char *dst, const char *src, size_t n);
/* Warning: strcmp behaviour differs from GLIBC version! It returns only 0 or -1 */
* int strcmp(const char *s1, const char *s2);
* void *memset(void *s, int c, size_t n);
```

### _<kernel/klib/stdarg.h>_
```c
* va_start(v, l)
* va_end(v)
* va_arg(v, T)
* va_copy(d, s)
```

### _<kernel/klib/list.h>_
```c
#define init_list(head)
#define list_empty(head)
#define append_to_list(head, node)
#define remove_from_list(node)
#define for_each_in_list(head, i)
```