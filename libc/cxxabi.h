#ifndef _CXXABI_H
#define _CXXABI_H
COSMOPOLITAN_C_START_

#ifdef __cplusplus
namespace __cxxabiv1 {
#endif /* __cplusplus */

char *__cxa_demangle(const char *, char *, size_t *, int *);
int __cxa_atexit(void (*)(void *), void *, void *) paramsnonnull((1)) dontthrow;
int __cxa_thread_atexit(void (*)(void *), void *, void *) dontthrow;
void __cxa_finalize(void *);

#ifdef __cplusplus
} /* namespace __cxxabiv1 */
#endif /* __cplusplus */

COSMOPOLITAN_C_END_
#endif /* _CXXABI_H */
