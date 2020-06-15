#ifndef COSMOPOLITAN_LIBC_STDIO_FPUTC_H_
#define COSMOPOLITAN_LIBC_STDIO_FPUTC_H_
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Writes byte to stream.
 *
 * @return c (as unsigned char) if written or -1 w/ errno
 */
forceinline int __fputc(int c, FILE *f) {
  /* assert((f->iomode & O_ACCMODE) != O_RDONLY); */
  if (c != -1) {
    unsigned char ch = (unsigned char)c;
    f->buf[f->end] = ch;
    f->end = (f->end + 1) & (f->size - 1);
    if (f->beg == f->end || f->bufmode == _IONBF ||
        (f->bufmode == _IOLBF && ch == '\n')) {
      if (f->writer) {
        return f->writer(f);
      } else if (f->beg == f->end) {
        return fseteof(f);
      }
    }
    return ch;
  } else {
    return fseteof(f);
  }
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_FPUTC_H_ */
