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

#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/ctype.h"
#include "libc/limits.h"
#include "libc/str/str.h"
#include "libc/str/tab.h"
#include "libc/sysv/errfuns.h"

/**
 * Turns DOS path into UNIX style path.
 *
 * Overlapping operands will result in undefined behavior.
 *
 * @param path is absolute UNC or DOS style path in UTF-16
 * @param path8 is utf-8 output which must have `PATH_MAX` bytes
 * @return strlen of `path8` or -1 w/ errno
 * @raise ENAMETOOLONG if `path` decoded longer than `PATH_MAX`
 */
textwindows int __mkunixpath(const char16_t *path,
                             char path8[static PATH_MAX]) {

  // convert utf-16 to utf-8
  // we can't modify `buf` until we're certain of success
  char *p = path8;
  int n = tprecode16to8(p, PATH_MAX, path).ax;
  if (n >= PATH_MAX - 1)
    return enametoolong();  // utf-8 explosion

  // convert backslash to slash
  for (int i = 0; i < n; ++i)
    if (p[i] == '\\')
      p[i] = '/';

  // turn //?/c:/... and //./c:/... and /??/c:/... into c:/...
  // but don't convert stuff like //?/pipe/cosmo/...
  if (p[0] == '/' &&    //
      (p[1] == '/' ||   //
       p[1] == '?') &&  //
      (p[2] == '?' ||   //
       p[2] == '.') &&  //
      p[3] == '/' &&    //
      isalpha(p[4]) &&  //
      p[5] == ':' &&    //
      p[6] == '/') {
    memmove(p, p + 4, n - 4 + 1);
    n -= 4;
  }

  // turn c:/... into /c/...
  if (isalpha(p[0]) &&  //
      p[1] == ':' &&    //
      p[2] == '/') {
    p[1] = p[0];
    p[0] = '/';
  }

  // turn /c into / based on $SYSTEMDRIVE
  // turn /c/... into /... based on $SYSTEMDRIVE
  if (p[0] == '/' &&
      kToLower[p[1] & 255] == kToLower[__getcosmosdrive() & 255]) {
    if (p[2] == '/') {
      // don't do this magic if we're qualifying /C/x/...
      if (!(isalpha(p[3]) && (p[4] == '/' || !p[4]))) {
        for (size_t i = 0; i < n - 2 + 1; ++i)
          p[i] = p[2 + i];
        n -= 2;
      }
    } else if (!p[2]) {
      p[1] = 0;
      n = 1;
    }
  }

  return n;
}
