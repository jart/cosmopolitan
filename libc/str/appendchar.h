#ifndef COSMOPOLITAN_LIBC_RUNTIME_APPENDCHAR_H_
#define COSMOPOLITAN_LIBC_RUNTIME_APPENDCHAR_H_
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline void AppendChar(char **p, char *pe, wint_t c) {
  if (*p < pe) *p += tpencode(*p, pe - *p, c, false);
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_APPENDCHAR_H_ */
