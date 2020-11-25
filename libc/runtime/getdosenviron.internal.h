#ifndef COSMOPOLITAN_LIBC_DOSENVIRON_H_
#define COSMOPOLITAN_LIBC_DOSENVIRON_H_
#ifndef __STRICT_ANSI__
#include "libc/bits/safemacros.internal.h"
#include "libc/str/appendchar.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Transcodes NT environment variable block from UTF-16 to UTF-8.
 *
 * @param env is a double NUL-terminated block of key=values
 * @param buf is the new environment
 * @param size is the byte capacity of buf
 * @param envp stores NULL-terminated string pointer list
 * @param max is the pointer count capacity of envp
 * @return number of variables decoded, excluding NULL-terminator
 */
static inline int GetDosEnviron(const char16_t *env, char *buf, size_t size,
                                char **envp, size_t max) {
  wint_t wc;
  size_t envc;
  char *p, *pe;
  bool endstring;
  const char16_t *s;
  s = env;
  envc = 0;
  if (size) {
    p = buf;
    pe = buf + size - 1;
    if (p < pe) {
      wc = DecodeNtsUtf16(&s);
      while (wc) {
        if (++envc < max) {
          envp[envc - 1] = p < pe ? p : NULL;
        }
        do {
          AppendChar(&p, pe, wc);
          endstring = !wc;
          wc = DecodeNtsUtf16(&s);
        } while (!endstring);
        buf[min(p - buf, size - 2)] = u'\0';
      }
    }
    AppendChar(&p, pe, '\0');
    buf[min(p - buf, size - 1)] = u'\0';
  }
  if (max) envp[min(envc, max - 1)] = NULL;
  return envc;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_DOSENVIRON_H_ */
