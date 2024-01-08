#ifndef COSMOPOLITAN_LIBC_LOG_LIBFATAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_LIBFATAL_INTERNAL_H_
#include "libc/macros.internal.h"
COSMOPOLITAN_C_START_

forceinline unsigned long __strlen(const char *s) {
  unsigned long n = 0;
  while (*s++) ++n;
  return n;
}

forceinline int __strcmp(const char *l, const char *r) {
  size_t i = 0;
  while (l[i] == r[i] && r[i]) ++i;
  return (l[i] & 255) - (r[i] & 255);
}

forceinline char *__stpcpy(char *d, const char *s) {
  size_t i;
  for (i = 0;; ++i) {
    if (!(d[i] = s[i])) {
      return d + i;
    }
  }
}

forceinline void *__repstosb(void *di, char al, size_t cx) {
#if defined(__x86__) && defined(__GNUC__) && !defined(__STRICT_ANSI__)
  asm("rep stosb"
      : "=D"(di), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(cx), "a"(al));
  return di;
#else
  char *d = di;
  while (cx--) {
    *d++ = al;
    asm volatile("" ::: "memory");
  }
  return (void *)d;
#endif
}

forceinline void *__repmovsb(void *di, const void *si, size_t cx) {
#if defined(__x86__) && defined(__GNUC__) && !defined(__STRICT_ANSI__)
  asm("rep movsb"
      : "=D"(di), "=S"(si), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(si), "2"(cx), "m"(*(char(*)[cx])si));
  return di;
#else
  char *d = di;
  const char *s = si;
  while (cx--) {
    *d++ = *s++;
    asm volatile("" ::: "memory");
  }
  return (void *)d;
#endif
}

forceinline void *__mempcpy(void *d, const void *s, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    ((char *)d)[i] = ((const char *)s)[i];
    asm volatile("" ::: "memory");
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
  return __fixcpy(p, x, ROUNDUP(x ? (__builtin_clzll(x) ^ 63) + 1 : 1, 4));
}

forceinline const void *__memchr(const void *s, unsigned char c, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if (((const unsigned char *)s)[i] == c) {
      return (const unsigned char *)s + i;
    }
  }
  return 0;
}

forceinline char *__strstr(const char *haystack, const char *needle) {
  size_t i;
  for (;;) {
    for (i = 0;; ++i) {
      if (!needle[i]) return (/*unconst*/ char *)haystack;
      if (!haystack[i]) break;
      if (needle[i] != haystack[i]) break;
    }
    if (!*haystack++) break;
  }
  return 0;
}

forceinline char16_t *__strstr16(const char16_t *haystack,
                                 const char16_t *needle) {
  size_t i;
  for (;;) {
    for (i = 0;; ++i) {
      if (!needle[i]) return (/*unconst*/ char16_t *)haystack;
      if (!haystack[i]) break;
      if (needle[i] != haystack[i]) break;
    }
    if (!*haystack++) break;
  }
  return 0;
}

forceinline const char *__strchr(const char *s, unsigned char c) {
  for (;; ++s) {
    if ((*s & 255) == c) return s;
    if (!*s) return 0;
  }
}

forceinline unsigned long __atoul(const char *p) {
  int c;
  unsigned long x = 0;
  while ('0' <= (c = *p++) && c <= '9') x *= 10, x += c - '0';
  return x;
}

forceinline long __atol(const char *p) {
  int s = *p;
  unsigned long x;
  if (s == '-' || s == '+') ++p;
  x = __atoul(p);
  if (s == '-') x = -x;
  return x;
}

forceinline void *__memset(void *a, int c, unsigned long n) {
  char *d = a;
  unsigned long i;
  for (i = 0; i < n; ++i) {
    __asm__ volatile("" ::: "memory");
    d[i] = c;
  }
  return d;
}

forceinline void *__memcpy(void *a, const void *b, unsigned long n) {
  char *d = a;
  unsigned long i;
  const char *s = b;
  for (i = 0; i < n; ++i) {
    __asm__ volatile("" ::: "memory");
    d[i] = s[i];
  }
  return d;
}

forceinline void *__memmove(void *a, const void *b, unsigned long n) {
  char *d = a;
  unsigned long i;
  const char *s = b;
  if (d > s) {
    for (i = n; i--;) {
      __asm__ volatile("" ::: "memory");
      d[i] = s[i];
    }
  } else {
    for (i = 0; i < n; ++i) {
      __asm__ volatile("" ::: "memory");
      d[i] = s[i];
    }
  }
  return d;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_LOG_LIBFATAL_INTERNAL_H_ */
