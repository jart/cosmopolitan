/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/fmt/fmt.h"
#include "libc/macros.internal.h"
#include "libc/stdio/stdio.h"

struct Append {
  size_t i, n;
  char *p;
};

int AppendFmt(struct Append *b, const char *fmt, ...) {
  int n;
  char *p;
  va_list va, vb;
  va_start(va, fmt);
  va_copy(vb, va);
  n = vsnprintf(b->p + b->i, b->n - b->i, fmt, va);
  if (n >= b->n - b->i) {
    do {
      if (b->n) {
        b->n += b->n >> 1; /* this is the important line */
      } else {
        b->n = 16;
      }
    } while (b->i + n + 1 > b->n);
    b->p = realloc(b->p, b->n);
    vsnprintf(b->p + b->i, b->n - b->i, fmt, vb);
  }
  va_end(vb);
  va_end(va);
  b->i += n;
  return n;
}

char *DumpHexc(const char *data, size_t size, size_t *z) {
  long o;
  int b, i, n;
  char A[128], *p;
  struct Append buf;
  if (size == -1) size = data ? strlen(data) : 0;
  buf.i = 0;
  buf.n = 256;
  buf.p = calloc(1, 256);
  AppendFmt(&buf, "\"\\\n");
  for (b = o = 0; (n = MIN(16, size)); data += n, size -= n) {
    p = A;
    for (i = 0; i < n; ++i) {
      *p++ = '\\';
      *p++ = 'x';
      *p++ = "0123456789abcdef"[(data[i] & 0xF0) >> 4];
      *p++ = "0123456789abcdef"[(data[i] & 0x0F) >> 0];
    }
    if (o) AppendFmt(&buf, "\\\n");
    AppendFmt(&buf, "%.*s", p - A, A);
    o += n;
  }
  AppendFmt(&buf, "\"");
  if (z) *z = buf.i;
  return buf.p;
}

int main(int argc, char *argv[]) {
  char *p;
  size_t n, g;
  char buf[512];
  struct Append b = {0};
  while ((g = fread(buf, 1, sizeof(buf), stdin))) {
    AppendFmt(&b, "%.*s", g, buf);
  }
  if (!ferror(stdin)) {
    p = DumpHexc(b.p, b.i, &n);
    fwrite(p, 1, n, stdout);
  }
  printf("\n");
  return ferror(stdin) || ferror(stdout) ? 1 : 0;
}
