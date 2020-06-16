#ifndef COSMOPOLITAN_LIBC_RUNTIME_APPENDCHAR_H_
#define COSMOPOLITAN_LIBC_RUNTIME_APPENDCHAR_H_
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/str/tpencode.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

static inline void AppendChar(char **p, char *pe, wint_t wc) {
  uint64_t w;
  w = tpenc(wc);
  do {
    if (*p >= pe) break;
    *(*p)++ = w & 0xff;
  } while (w >>= 8);
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_APPENDCHAR_H_ */
