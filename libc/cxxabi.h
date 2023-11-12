#ifndef _CXXABI_H
#define _CXXABI_H
COSMOPOLITAN_C_START_

union CxaGuardValue;

char *__cxa_demangle(const char *, char *, size_t *, int *);
int __cxa_atexit(void (*)(void *), void *, void *) paramsnonnull((1)) dontthrow;
int __cxa_guard_acquire(union CxaGuardValue *);
int __cxa_thread_atexit(void *, void *, void *) dontthrow;
void __cxa_finalize(void *);
void __cxa_guard_abort(union CxaGuardValue *) dontthrow;
void __cxa_guard_release(union CxaGuardValue *) dontthrow;
void __cxa_pure_virtual(void) wontreturn;

COSMOPOLITAN_C_END_
#endif /* _CXXABI_H */
