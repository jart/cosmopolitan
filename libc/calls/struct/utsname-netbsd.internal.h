#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_NETBSD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_NETBSD_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct utsname_netbsd {
  char sysname[256];  /* name of os */
  char nodename[256]; /* name of network node */
  char release[256];  /* release level */
  char version[256];  /* version level */
  char machine[256];  /* hardware type */
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_NETBSD_INTERNAL_H_ */
