#ifndef COSMOPOLITAN_LIBC_LOG_BSD_H_
#define COSMOPOLITAN_LIBC_LOG_BSD_H_
COSMOPOLITAN_C_START_

void err(int, const char *, ...) wontreturn;
void verr(int, const char *, va_list) wontreturn;
void errc(int, int, const char *, ...) wontreturn;
void verrc(int, int, const char *, va_list) wontreturn;
void errx(int, const char *, ...) wontreturn;
void verrx(int, const char *, va_list) wontreturn;
void warn(const char *, ...);
void vwarn(const char *, va_list);
void warnc(int, const char *, ...);
void vwarnc(int, const char *, va_list);
void warnx(const char *, ...);
void vwarnx(const char *, va_list);
void err_set_exit(void (*)(int));

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_LOG_BSD_H_ */
