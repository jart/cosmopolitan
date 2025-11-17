// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/str/str.h"

// The purpose of this function is to prevent the most truly evil kinds
// of filesystem paths from being created. However it's important that
// we preserve the ability to work with existing ones.
//
//     "Implementations are encouraged to have mkdir() and mkdirat() report
//      an [EILSEQ] error if the last component of path contains "any bytes
//      that have the encoded value of a <newline> character."
//                                  ──Quoth POSIX.1-2024
//
// Functions that create files should consult this function first and
// raise an EILSEQ error if false is returned.
bool __is_evil_path(const char *p) {

  // get bounds of last component and check for newline
  const char *e = p + strlen(p);
  while (e > p && e[-1] == '/')
    --e;
  const char *c = e;
  while (c > p) {
    int b = *--c;
    if (b == '\n')
      return true;
    if (b == '/')
      break;
  }

  // sys_open() on xnu doesn't allow malformed utf-8 but we check that
  // we don't create such paths in userspace across all OSes too
  if (SupportsXnu())
    if (!isutf8(c, e - c))
      return true;

  // on Windows if you try to CreateFile("foo . . . ") then the way
  // normalization works is it'll succeed and a file named "foo" will
  // pop up on your computer. now obviously "." are ".." are fine as
  // last components, since everyone agrees on their normalization.
  // however we should probably steer people away from doing things that
  // won't work when they migrate their app to the windows platform.
  // it's also worth noting that windows hates control characters.
  // however they forgot to hate the DEL (0177) character somehow.
  // checking here also helps us raise EILSEQ instead of EINVAL.
  if (SupportsWindows()) {
    if (e - c == 1 && *c == '.')
      return false;
    if (e - c == 2 && c[0] == '.' && c[1] == '.')
      return false;
    if (e > c)
      if (e[-1] == '.' || e[-1] == ' ')
        return true;
    while (c < e)
      if ((*c++ & 255) < 32)
        return true;
  }

  return false;
}
