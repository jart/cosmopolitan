/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/thompike.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  if (!SupportsXnu() || !SupportsWindows())
    exit(0);  // TODO: figure out which tests to comment out
}

TEST(__is_evil_path, empty) {
  EXPECT_FALSE(__is_evil_path(""));
}

TEST(__is_evil_path, newline) {
  EXPECT_TRUE(__is_evil_path("x\n"));
  EXPECT_TRUE(__is_evil_path("x\n/"));
  EXPECT_TRUE(__is_evil_path("x\n//"));
  EXPECT_TRUE(__is_evil_path("\nx//"));
  EXPECT_TRUE(__is_evil_path("\nx"));
  EXPECT_TRUE(__is_evil_path("\nx/"));
  EXPECT_FALSE(__is_evil_path("\n/x/"));
  EXPECT_FALSE(__is_evil_path("/\n/x/"));
  EXPECT_FALSE(__is_evil_path("/\n/x"));
}

TEST(__is_evil_path, overlong_newline) {
  EXPECT_TRUE(__is_evil_path("x\300\212"));
  EXPECT_TRUE(__is_evil_path("x\300\212/"));
  EXPECT_TRUE(__is_evil_path("x\300\212//"));
  EXPECT_TRUE(__is_evil_path("\300\212x//"));
  EXPECT_TRUE(__is_evil_path("\300\212x"));
  EXPECT_TRUE(__is_evil_path("\300\212x/"));
  EXPECT_FALSE(__is_evil_path("\300\212/x/"));
  EXPECT_FALSE(__is_evil_path("/\300\212/x/"));
  EXPECT_FALSE(__is_evil_path("/\300\212/x"));
}

TEST(__is_evil_path, overlong_nul) {
  EXPECT_TRUE(__is_evil_path("x\300\200"));
  EXPECT_TRUE(__is_evil_path("x\300\200/"));
  EXPECT_TRUE(__is_evil_path("x\300\200//"));
  EXPECT_TRUE(__is_evil_path("\300\200x//"));
  EXPECT_TRUE(__is_evil_path("\300\200x"));
  EXPECT_TRUE(__is_evil_path("\300\200x/"));
  EXPECT_FALSE(__is_evil_path("\300\200/x/"));
  EXPECT_FALSE(__is_evil_path("/\300\200/x/"));
  EXPECT_FALSE(__is_evil_path("/\300\200/x"));
}

TEST(__is_evil_path, overlonger_nuls) {
  EXPECT_TRUE(__is_evil_path("\340\200\200"));
  EXPECT_TRUE(__is_evil_path("\360\200\200\200"));
}

TEST(__is_evil_path, emdash) {
  EXPECT_FALSE(__is_evil_path("\342\200\224"));
  EXPECT_TRUE(__is_evil_path("\342\200"));
}

TEST(__is_evil_path, cont) {
  EXPECT_TRUE(__is_evil_path("\200"));
  EXPECT_FALSE(__is_evil_path("\200/hi"));
}
