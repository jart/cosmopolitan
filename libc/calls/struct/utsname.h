#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_H_

#define SYS_NMLN 150

COSMOPOLITAN_C_START_

struct utsname {             /* cosmo abi */
  char sysname[SYS_NMLN];    /* name of os */
  char nodename[SYS_NMLN];   /* name of network node */
  char release[SYS_NMLN];    /* release level */
  char version[SYS_NMLN];    /* version level */
  char machine[SYS_NMLN];    /* hardware type */
  char domainname[SYS_NMLN]; /* domain name */
};

int uname(struct utsname *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_UTSNAME_H_ */
