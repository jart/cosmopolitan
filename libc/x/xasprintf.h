#ifndef COSMOPOLITAN_LIBC_X_XASPRINTF_H_
#define COSMOPOLITAN_LIBC_X_XASPRINTF_H_
COSMOPOLITAN_C_START_

char *xasprintf(const char *, ...) paramsnonnull((1))
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
char *xvasprintf(const char *, va_list) paramsnonnull()
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_X_XASPRINTF_H_ */
