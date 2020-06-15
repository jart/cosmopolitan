/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

/* TODO(jart): calling malloc_usable_size was a terrible idea */

TEST(todo_jart, broken_in_opt_native_mode) {
  (void)0;
  (void)0;
}

int64_t fd;

TEST(gc, usageExample_c11) {
  fd = open("/dev/null", O_WRONLY);
  defer(close_s, &fd);
  char *msg = gc(xasprintf("%d + %d = %d", 2, 2, 2 + 2));
  write(fd, msg, strlen(msg));
}

TEST(gc, checkMallocUsableSizeWorksTheWayWeHopeItDoes) {
  char *p = malloc(32);
  EXPECT_GE(malloc_usable_size(p), 32);
  free(p);
  EXPECT_GE(malloc_usable_size(p), 0);
}

noinline void function1of1(char *p) {
  EXPECT_GE(malloc_usable_size(gc(p)), 32);
}
TEST(gc, testOne) {
  char *p = malloc(32);
  function1of1(p);
  EXPECT_EQ(malloc_usable_size(p), 0);
}

noinline void function2of2(char *p1, char *p2) {
  EXPECT_GE(malloc_usable_size(p1), 32);
  EXPECT_GE(malloc_usable_size(p2), 64);
  gc(p2);
  EXPECT_GE(malloc_usable_size(p1), 32);
  EXPECT_GE(malloc_usable_size(p2), 64);
}
noinline void function1of2(char *p1, char *p2) {
  EXPECT_GE(malloc_usable_size(p1), 32);
  EXPECT_GE(malloc_usable_size(p2), 64);
  function2of2(gc(p1), p2);
  EXPECT_GE(malloc_usable_size(p1), 32);
  EXPECT_GE(malloc_usable_size(p2), 0);
}
TEST(gc, testTwo) {
  char *p1 = malloc(32);
  char *p2 = malloc(64);
  function1of2(p1, p2);
  EXPECT_GE(malloc_usable_size(p1), 0);
  EXPECT_GE(malloc_usable_size(p2), 0);
}
