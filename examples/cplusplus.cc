/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│vi: set net ft=c++ ts=2 sts=2 sw=2 fenc=utf-8                              :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
