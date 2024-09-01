#ifndef COSMOPOLITAN_LIBC_STR_KMP_H_
#define COSMOPOLITAN_LIBC_STR_KMP_H_
COSMOPOLITAN_C_START_

char *__memmem_kmp(const char *, size_t, const char *, size_t);
char16_t *__memmem_kmp16(const char16_t *, size_t, const char16_t *, size_t);
wchar_t *__memmem_kmp32(const wchar_t *, size_t, const wchar_t *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STR_KMP_H_ */
