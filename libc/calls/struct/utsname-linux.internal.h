#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_LINUX_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_LINUX_INTERNAL_H_
COSMOPOLITAN_C_START_

struct utsname_linux {
  char sysname[65];
  char nodename[65];
  char release[65];
  char version[65];
  char machine[65];
  char domainname[65];
};

int sys_uname_linux(struct utsname_linux *) asm("sys_uname");

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_LINUX_INTERNAL_H_ */
