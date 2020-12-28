/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│vi: set net ft=c++ ts=2 sts=2 sw=2 fenc=utf-8                              :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/safemacros.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

class Log {
 public:
  Log();
  ~Log();
  int *x();

 private:
  int *x_;
};

Log::Log() {
  x_ = new int[64];
}
Log::~Log() {
  delete x_;
}
int *Log::x() {
  return x_;
}

class Log g_log;

int main(int argc, char *argv[]) {
  int *x = new int[64];
  memset(x, 0, 64 * sizeof(int));
  for (int i = 0; i < min(64, argc); ++i) g_log.x()[i] += argc;
  printf("%p %d %d %d\n", (void *)(intptr_t)g_log.x(), g_log.x()[0],
         g_log.x()[0], g_log.x()[0]);
  delete x;
  return 0;
}
