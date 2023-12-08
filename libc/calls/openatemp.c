/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/serialize.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Opens unique temporary file with maximum generality.
 *
 * This function is similar to mkstemp() in that it does two things:
 *
 * 1. Generate a unique filename by mutating `template`
 * 2. Return a newly opened file descriptor to the name
 *
 * Exclusive secure access is assured even if `/tmp` is being used on a
 * UNIX system like Super Dimensional Fortress or CPanel where multiple
 * hostile adverserial users may exist on a single multi-tenant system.
 *
 * The substring XXXXXX is replaced with 30 bits of base32 entropy and a
 * hundred retries are attempted in the event of collisions. The XXXXXXX
 * pattern must be present at the end of the supplied template string.
 *
 * If the generated filename needs to have a file extension (rather than
 * ending with random junk) then this API has the helpful `suffixlen` to
 * specify exactly how long that suffix in the template actually is. For
 * example if the template is `"/tmp/notes.XXXXXX.txt"` then `suffixlen`
 * should be `4`.
 *
 * The flags `O_RDWR | O_CREAT | O_EXCL` are always set and don't need
 * to be specified by the caller. It's a good idea to pass `O_CLOEXEC`
 * and some applications may want `O_APPEND`. Cosmopolitan also offers
 * `O_UNLINK` which will ensure the created file will delete itself on
 * close similar to calling unlink() after this function on `template`
 * which is mutated on success, except `O_UNLINK` will work right when
 * running on Windows and it's polyfilled automatically on UNIX.
 *
 * The `mode` parameter should usually be `0600` to ensure owner-only
 * read/write access. However it may be useful to set this to `0700`
 * when creating executable files. Please note that sometimes `/tmp` is
 * mounted by system administrators as `noexec`. It's also permissible
 * to pass `0` here, since the `0600` bits are always set implicitly.
 *
 * ### Examples
 *
 * Here's an example of how to replicate the functionality of tmpfile()
 * which creates an unnamed temporary file as an stdio handle, which is
 * guaranteed to either not have a name (unlinked on UNIX), or shall be
 * deleted once closed (will perform kNtFileFlagDeleteOnClose on WIN32)
 *
 *     char path[] = "/tmp/XXXXXX";
 *     int fd = openatemp(AT_FDCWD, path, 0, O_UNLINK, 0);
 *     FILE *tmp = fdopen(fd, "w+");
 *
 * Here's an example of how to do mktemp() does, where a temporary file
 * name is generated with pretty good POSIX and security best practices
 *
 *     char path[PATH_MAX+1];
 *     const char *tmpdir = getenv("TMPDIR");
 *     strlcpy(path, tmpdir ? tmpdir : "/tmp", sizeof(path));
 *     strlcat(path, "/notes.XXXXXX.txt", sizeof(path));
 *     close(openatemp(AT_FDCWD, path, 4, O_UNLINK, 0));
 *     printf("you can use %s to store your notes\n", path);
 *
 * @param dirfd is open directory file descriptor, which is ignored if
 *     `template` is an absolute path; or `AT_FDCWD` to specify getcwd
 * @param template is a pathname relative to current directory by default,
 *     that needs to have "XXXXXX" at the end of the string; this memory
 *     must be mutable and should be owned by the calling thread; it will
 *     be modified (only on success) to return the generated filename
 * @param suffixlen may be nonzero to permit characters after the "XXXXXX"
 * @param mode is conventionally 0600, for owner-only non-exec access
 * @param flags could have O_APPEND, O_CLOEXEC, O_UNLINK, O_SYNC, etc.
 * @return exclusive open file descriptor for file at the generated path
 *     stored to `template`, or -1 w/ errno
 * @raise EINVAL if `template` (less the `suffixlen` region) didn't
 *     end with the string "XXXXXXX"
 * @raise EINVAL if `suffixlen` was negative or too large
 * @cancelationpoint
 */
int openatemp(int dirfd, char *template, int suffixlen, int flags, int mode) {
  flags &= ~O_ACCMODE;
  flags |= O_RDWR | O_CREAT | O_EXCL;
  int len = strlen(template);
  if (6 + suffixlen < 6 || 6 + suffixlen > len ||
      READ32LE(template + len - suffixlen - 6) != READ32LE("XXXX") ||
      READ16LE(template + len - suffixlen - 6 + 4) != READ16LE("XX")) {
    return einval();
  }
  for (;;) {
    int w = _rand64();
    for (int i = 0; i < 6; ++i) {
      template[len - suffixlen - 6 + i] =
          "0123456789abcdefghikmnpqrstvwxyz"[w & 31];
      w >>= 5;
    }
    int fd, e = errno;
    if ((fd = openat(dirfd, template, flags, mode | 0600)) != -1) {
      return fd;
    } else if (errno == EEXIST) {
      errno = e;
    } else {
      memcpy(template + len - suffixlen - 6, "XXXXXX", 6);
      return -1;
    }
  }
}
