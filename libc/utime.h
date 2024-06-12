#ifndef COSMOPOLITAN_LIBC_UTIME_H_
#define COSMOPOLITAN_LIBC_UTIME_H_
COSMOPOLITAN_C_START_

struct utimbuf {
  int64_t actime;  /* access time */
  int64_t modtime; /* modified time */
};

int utime(const char *, const struct utimbuf *) libcesque;

#ifdef _COSMO_SOURCE
int sys_utime(const char *, const struct utimbuf *) libcesque;
#endif /* _COSMO_SOURCE */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_UTIME_H_ */
