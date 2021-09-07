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
#include "libc/unicode/langinfo.h"

static const char c_time[] = "\
Sun\0\n\
Mon\0\n\
Tue\0\n\
Wed\0\n\
Thu\0\n\
Fri\0\n\
Sat\0\n\
Sunday\0\n\
Monday\0\n\
Tuesday\0\n\
Wednesday\0\n\
Thursday\0\n\
Friday\0\n\
Saturday\0\n\
Jan\0\n\
Feb\0\n\
Mar\0\n\
Apr\0\n\
May\0\n\
Jun\0\n\
Jul\0\n\
Aug\0\n\
Sep\0\n\
Oct\0\n\
Nov\0\n\
Dec\0\n\
January\0\n\
February\0\n\
March\0\n\
April\0\n\
May\0\n\
June\0\n\
July\0\n\
August\0\n\
September\0\n\
October\0\n\
November\0\n\
December\0\n\
AM\0\n\
PM\0\n\
a %b %e %T %Y\0\n\
m/%d/%y\0\n\
H:%M:%S\0\n\
I:%M:%S %p\0\n\
0\n\
0\n\
m/%d/%y\0\n\
0123456789\0\n\
a %b %e %T %Y\0\n\
H:%M:%S";

static const char c_messages[] = "\
^[yY]\0\
^[nN]\0\
yes\0\
no";

static const char c_numeric[] = "\
.\0\
";

char *nl_langinfo(int item) {
  if (item == CODESET) {
    return "UTF-8";
  } else {
    return 0;
  }
}
