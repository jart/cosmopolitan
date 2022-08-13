#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_H_

#define SYS_NMLN 321

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct utsname {
  char sysname[SYS_NMLN];
  char nodename[SYS_NMLN];
  char release[SYS_NMLN];
  char version[SYS_NMLN];
  char machine[SYS_NMLN];
  char domainname[SYS_NMLN];
};

int uname(struct utsname *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_H_ */
