#ifndef COSMOPOLITAN_LIBC_SOCK_SYSLOG_H_
#define COSMOPOLITAN_LIBC_SOCK_SYSLOG_H_
COSMOPOLITAN_C_START_

int setlogmask(int);
void openlog(const char *, int, int);
void syslog(int, const char *, ...);
void closelog(void);
void vsyslog(int, const char *, va_list);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_SYSLOG_H_ */
