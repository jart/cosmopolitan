#ifndef COSMOPOLITAN_LIBC_LOG_LIBFATAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_LIBFATAL_INTERNAL_H_
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/sysv/consts/nr.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern char __fatalbuf[];

forceinline int __getpid(void) {
  int rc;
  if (!IsWindows()) {
    asm volatile("call\t__syscall__"
                 : "=a"(rc)
                 : "0"(__NR_getpid)
                 : "rcx", "r11", "memory");
    return rc;
  } else {
    return GetCurrentProcessId();
  }
}

forceinline ssize_t __write(const void *data, size_t size) {
  char cf;
  ssize_t rc;
  uint32_t wrote;
  if (!IsWindows()) {
    asm volatile("call\t__syscall__"
                 : "=@ccc"(cf), "=a"(rc)
                 : "1"(__NR_write), "D"(2), "S"(data), "d"(size)
                 : "rcx", "r11", "memory");
    if (cf && IsBsd()) rc = -rc;
    return rc;
  } else {
    if (WriteFile(GetStdHandle(kNtStdErrorHandle), data, size, &wrote, 0)) {
      return wrote;
    } else {
      return -1;
    }
  }
}

forceinline size_t __strlen(const char *s) {
  size_t i = 0;
  while (s[i]) ++i;
  return i;
}

forceinline ssize_t __write_str(const char *s) {
  return __write(s, __strlen(s));
}

forceinline char *__stpcpy(char *d, const char *s) {
  size_t i;
  for (i = 0;; ++i) {
    if (!(d[i] = s[i])) {
      return d + i;
    }
  }
}

forceinline void *__repmovsb(void *di, void *si, size_t cx) {
  asm("rep movsb"
      : "=D"(di), "=S"(si), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(si), "2"(cx), "m"(*(char(*)[cx])si));
  return di;
}

forceinline void *__mempcpy(void *d, const void *s, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    ((char *)d)[i] = ((const char *)s)[i];
  }
  return (char *)d + n;
}

forceinline char *__uintcpy(char p[hasatleast 21], uint64_t x) {
  char t;
  size_t i, a, b;
  i = 0;
  do {
    p[i++] = x % 10 + '0';
    x = x / 10;
  } while (x > 0);
  p[i] = '\0';
  if (i) {
    for (a = 0, b = i - 1; a < b; ++a, --b) {
      t = p[a];
      p[a] = p[b];
      p[b] = t;
    }
  }
  return p + i;
}

forceinline char *__intcpy(char p[hasatleast 21], int64_t x) {
  if (x < 0) *p++ = '-', x = -(uint64_t)x;
  return __uintcpy(p, x);
}

forceinline char *__fixcpy(char p[hasatleast 17], uint64_t x, uint8_t k) {
  while (k > 0) *p++ = "0123456789abcdef"[(x >> (k -= 4)) & 15];
  *p = '\0';
  return p;
}

forceinline char *__hexcpy(char p[hasatleast 17], uint64_t x) {
  return __fixcpy(p, x, ROUNDUP(x ? bsrl(x) + 1 : 1, 4));
}

void __printf(const char *, ...);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_LIBFATAL_INTERNAL_H_ */
