/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/kprintf.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"

#define N 128

#define append(...) o += ksnprintf(buf + o, N - o, __VA_ARGS__)

static const struct thatispacked {
  int flag;
  const char name[10];
} kOpenFlags[] = {
    {O_CREAT, "CREAT"},          //
    {O_EXCL, "EXCL"},            //
    {O_SYNC, "SYNC"},            //
    {O_NOCTTY, "NOCTTY"},        //
    {O_TRUNC, "TRUNC"},          //
    {O_APPEND, "APPEND"},        //
    {O_NONBLOCK, "NONBLOCK"},    //
    {O_DSYNC, "DSYNC"},          //
    {O_DIRECT, "DIRECT"},        //
    {O_LARGEFILE, "LARGEFILE"},  //
    {O_DIRECTORY, "DIRECTORY"},  //
    {O_NOFOLLOW, "NOFOLLOW"},    //
    {O_NOATIME, "NOATIME"},      //
    {O_CLOEXEC, "CLOEXEC"},      //
    {_O_PATH, "PATH"},           //
    {O_UNLINK, "UNLINK"},        //
};

/**
 * Describes openat() flags argument.
 */
const char *_DescribeOpenFlags(char buf[N], int x) {
  int o = 0;
  if (x == -1)
    return "-1";

  switch (x & O_ACCMODE) {
    case O_RDONLY:
      append("O_RDONLY");
      break;
    case O_WRONLY:
      append("O_WRONLY");
      break;
    case O_RDWR:
      append("O_RDWR");
      break;
    default:
      append("%d", x & O_ACCMODE);
      break;
  }
  x &= ~O_ACCMODE;

  for (int i = 0; i < ARRAYLEN(kOpenFlags); ++i) {
    if ((x & kOpenFlags[i].flag) == kOpenFlags[i].flag) {
      append("|O_%s", kOpenFlags[i].name);
      x &= ~kOpenFlags[i].flag;
    }
  }

  if (x)
    append("|%#x", x);

  return buf;
}
