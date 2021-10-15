/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/log/color.internal.h"
#include "libc/log/internal.h"
#include "libc/log/libfatal.internal.h"
#include "libc/runtime/runtime.h"

/**
 * Prints initial part of fatal message.
 *
 * @note this is support code for __check_fail(), __assert_fail(), etc.
 */
relegated void __start_fatal(const char *file, int line) {
  bool colorful;
  char s[16 + 16 + 16 + 16 + PATH_MAX + 16 + NAME_MAX + 16], *p = s;
  __restore_tty(1);
  colorful = cancolor();
  *p++ = '\r';
  if (colorful) p = __stpcpy(p, "\e[J\e[30;101m");
  p = __stpcpy(p, "error");
  if (colorful) p = __stpcpy(p, "\e[94;49m"), *p++ = ':';
  p = __stpcpy(p, file), *p++ = ':';
  p = __intcpy(p, line), *p++ = ':';
  p = __stpcpy(p, program_invocation_short_name);
  if (colorful) p = __stpcpy(p, "\e[0m");
  *p++ = ':';
  *p++ = ' ';
  __write(s, p - s);
}
