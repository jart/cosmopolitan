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

#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/nt/systeminfo.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

static void __get_tmpdir_impl(char dest[static PATH_MAX]) {
  char *s;
  int m, n = 0;
  union {
    char buf8[PATH_MAX];
    char16_t buf16[PATH_MAX];
  } U;

  // we prefer the POSIX-specified $TMPDIR variable
  if ((s = getenv("TMPDIR")))
    if ((n = strlcpy(dest, s, PATH_MAX)) >= PATH_MAX)
      n = 0;

  // on windows we'll also try %TMP% and %TEMP%
  if (n <= 0)
    if (IsWindows())
      if ((n = GetTempPath(PATH_MAX, U.buf16)))
        n = __mkunixpath(U.buf16, dest);

  // resolve relative tmp dir against current directory
  if (n > 0) {
    if (dest[0] != '/') {
      if ((m = __getcwd(U.buf8, PATH_MAX)) != -1) {
        --m;  // __getcwd() includes nul terminator in length
        while (m && U.buf8[m - 1] == '/')
          U.buf8[--m] = 0;
        if (m + 1 + n < PATH_MAX) {
          memmove(dest + m + 1, dest, n + 1);
          memcpy(dest, U.buf8, m);
          dest[m] = '/';
          n = m + 1 + n;
        }
      }
    }
  }

  // ensure there's room for user to append another path component
  if (n > 0 && n + NAME_MAX + 2 > PATH_MAX)
    n = 0;

  // recursively ensure tmp dir exists
  if (n > 0)
    if (makedirs(dest, 0755))
      n = 0;

  // ensure resulting path has trailing slash
  if (n > 0)
    if (dest[n - 1] != '/')
      if (strlcat(dest, "/", PATH_MAX) >= PATH_MAX)
        n = 0;

  // if all else fails, return /tmp/
  if (n <= 0)
    n = strlcpy(dest, "/tmp/", PATH_MAX);
}

static struct {
  atomic_uint once;
  char path[PATH_MAX];
} __tmpdir;

static void __tmpdir_init(void) {
  int e = errno;
  __get_tmpdir_impl(__tmpdir.path);
  errno = e;
}

/**
 * Returns pretty good temporary directory.
 *
 * The order of precedence is:
 *
 *   - $TMPDIR/ is always favored if defined
 *   - GetTempPath(), for the New Technology
 *   - /tmp/ to make security scene go crazy
 *
 * This always succeeds. This routine always guarantees a trailing slash
 * will exist at the end of the returned path. The returned path will
 * also have at least NAME_MAX bytes available before you'll encounter
 * the PATH_MAX limit.
 *
 * Since the paths we choose come from environment variables, they can
 * be relative, and if that happens, we resolve the path against the
 * current directory. This requires i/o and it only happens once, since
 * the result of this function is memoized. We further ensure the temp
 * directory that's been selected actually exists, using makedirs().
 */
char *__get_tmpdir(void) {
  cosmo_once(&__tmpdir.once, __tmpdir_init);
  return __tmpdir.path;
}
