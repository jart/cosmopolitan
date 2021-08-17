#ifndef COSMOPOLITAN_LIBC_RUNTIME_SYSCONF_H_
#define COSMOPOLITAN_LIBC_RUNTIME_SYSCONF_H_

#define _SC_ARG_MAX          0
#define _SC_CHILD_MAX        1
#define _SC_CLK_TCK          2
#define _SC_OPEN_MAX         4
#define _SC_PAGESIZE         30
#define _SC_PAGE_SIZE        30
#define _SC_NPROCESSORS_ONLN 1002

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

long sysconf(int);
unsigned GetCpuCount(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_SYSCONF_H_ */
