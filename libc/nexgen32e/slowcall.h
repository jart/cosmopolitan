#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_SLOWCALL_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_SLOWCALL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define slowcall(fn, arg1, arg2, arg3, arg4, arg5, arg6)                \
  ({                                                                    \
    void *ax;                                                           \
    asm volatile("push\t%7\n\t"                                         \
                 "push\t%6\n\t"                                         \
                 "push\t%5\n\t"                                         \
                 "push\t%4\n\t"                                         \
                 "push\t%3\n\t"                                         \
                 "push\t%2\n\t"                                         \
                 "push\t%1\n\t"                                         \
                 "call\tslowcall"                                       \
                 : "=a"(ax)                                             \
                 : "g"(fn), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), \
                   "g"(arg5), "g"(arg6)                                 \
                 : "memory");                                           \
    ax;                                                                 \
  })

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_SLOWCALL_H_ */
