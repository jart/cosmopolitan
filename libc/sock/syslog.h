#ifndef COSMOPOLITAN_LIBC_SOCK_SYSLOG_H_
#define COSMOPOLITAN_LIBC_SOCK_SYSLOG_H_
COSMOPOLITAN_C_START_

int setlogmask(int) libcesque;
void openlog(const char *, int, int) libcesque;
void syslog(int, const char *, ...) libcesque;
void closelog(void) libcesque;
void vsyslog(int, const char *, va_list) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_SYSLOG_H_ */
