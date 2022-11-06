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
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/log/log.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Opens file.
 *
 * Here's an example of how a file can be created:
 *
 *     int fd = openat(AT_FDCWD, "hi.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
 *     write(fd, "hello\n", 6);
 *     close(fd);
 *
 * Here's an example of how that file could read back into memory:
 *
 *     char data[513] = {0};
 *     int fd = openat(AT_FDCWD, "hi.txt", O_RDONLY);
 *     read(fd, data, 512);
 *     close(fd);
 *     assert(!strcmp(data, "hello\n"));
 *
 * If your main() source file has this statement:
 *
 *     STATIC_YOINK("zip_uri_support");
 *
 * Then you can read zip assets by adding a `"/zip/..."` prefix to `file`, e.g.
 *
 *     // run `zip program.com hi.txt` beforehand
 *     openat(AT_FDCWD, "/zip/hi.txt", O_RDONLY);
 *
 * @param dirfd is normally `AT_FDCWD` but if it's an open directory and
 *     `file` names a relative path then it's opened relative to `dirfd`
 * @param file is a UTF-8 string naming filesystem entity, e.g. `foo/bar.txt`,
 *     which on Windows is bludgeoned into a WIN32 path automatically, e.g.
 *     - `foo/bar.txt` becomes `foo\bar.txt`
 *     - `/tmp/...` becomes whatever GetTempPath() is
 *     - `\\...` or `//...` is passed through to WIN32 unchanged
 *     - `/c/foo` or `\c\foo` becomes `\\?\c:\foo`
 *     - `c:/foo` or `c:\foo` becomes `\\?\c:\foo`
 *     - `/D` becomes `\\?\D:\`
 * @param flags must have one of the following under the `O_ACCMODE` bits:
 *     - `O_RDONLY`     to open `file` for reading only
 *     - `O_WRONLY`     to open `file` for writing
 *     - `O_RDWR`       to open `file` for reading and writing
 *     The following may optionally be bitwise or'd into `flags`:
 *     - `O_CREAT`      create file if it doesn't exist
 *     - `O_TRUNC`      automatic `ftruncate(fd,0)` if exists
 *     - `O_CLOEXEC`    automatic close() upon execve()
 *     - `O_EXCL`       exclusive access (see below)
 *     - `O_APPEND`     open file for appending only
 *     - `O_EXEC`       open file for execution only; see fexecve()
 *     - `O_NOCTTY`     prevents `file` possibly becoming controlling terminal
 *     - `O_NONBLOCK`   asks read/write to fail with `EAGAIN` rather than block
 *     - `O_DIRECT`     it's complicated (not supported on Apple and OpenBSD)
 *     - `O_DIRECTORY`  useful for stat'ing (hint on UNIX but required on NT)
 *     - `O_NOFOLLOW`   fail if it's a symlink (zero on Windows)
 *     - `O_DSYNC`      it's complicated (zero on non-Linux/Apple)
 *     - `O_RSYNC`      it's complicated (zero on non-Linux/Apple)
 *     - `O_VERIFY`     it's complicated (zero on non-FreeBSD)
 *     - `O_SHLOCK`     it's complicated (zero on non-BSD)
 *     - `O_EXLOCK`     it's complicated (zero on non-BSD)
 *     - `O_PATH`       open only for metadata (Linux 2.6.39+ otherwise zero)
 *     - `O_NOATIME`    don't record access time (zero on non-Linux)
 *     - `O_RANDOM`     hint random access intent (zero on non-Windows)
 *     - `O_SEQUENTIAL` hint sequential access intent (zero on non-Windows)
 *     - `O_COMPRESSED` ask fs to abstract compression (zero on non-Windows)
 *     - `O_INDEXED`    turns on that slow performance (zero on non-Windows)
 *     - `O_TMPFILE`    should not be used; use tmpfd() or tmpfile() instead
 *     There are three regular combinations for the above flags:
 *     - `O_RDONLY`: Opens existing file for reading. If it doesn't
 *       exist then nil is returned and errno will be `ENOENT` (or in
 *       some other cases `ENOTDIR`).
 *     - `O_WRONLY|O_CREAT|O_TRUNC`: Creates file. If it already
 *       exists, then the existing copy is destroyed and the opened
 *       file will start off with a length of zero. This is the
 *       behavior of the traditional creat() system call.
 *     - `O_WRONLY|O_CREAT|O_EXCL`: Create file only if doesn't exist
 *       already. If it does exist then `nil` is returned along with
 *       `errno` set to `EEXIST`.
 * @param mode is an octal user/group/other permission signifier that's
 *     ignored if `O_CREAT` isn't passed in `flags`; when creating files
 *     you'll usually want `mode` to be `0644` which enables global read
 *     and only permits the owner to write; or when creating executables
 *     you'll usually want `mode` to be `0755` which is the same, except
 *     the executable bit is set thrice too
 * @return file descriptor (which needs to be close()'d), or -1 w/ errno
 * @raise EPERM if pledge() is in play w/o appropriate rpath/wpath/cpath
 * @raise EACCES if unveil() is in play and didn't unveil your `file` path
 * @raise EACCES if we don't have permission to search a component of `file`
 * @raise EACCES if file exists but requested `flags & O_ACCMODE` was denied
 * @raise EACCES if file doesn't exist and parent dir lacks write permissions
 * @raise EACCES if `O_TRUNC` was specified in `flags` but writing was denied
 * @raise ENOTSUP if `file` is on zip file system and `dirfd` isn't `AT_FDCWD`
 * @raise ENOTDIR if a directory component in `file` exists as non-directory
 * @raise ENOTDIR if `file` is relative and `dirfd` isn't an open directory
 * @raise EROFS when writing is requested w/ `file` on read-only filesystem
 * @raise ENAMETOOLONG if symlink-resolved `file` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `file` exists longer than `NAME_MAX`
 * @raise ENOTSUP if `file` is on zip file system and process is vfork()'d
 * @raise ENOSPC if file system is full when `file` would be `O_CREAT`ed
 * @raise EINTR if we needed to block and a signal was delivered instead
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise ENOENT if `file` doesn't exist when `O_CREAT` isn't in `flags`
 * @raise ENOENT if `file` points to a string that's empty
 * @raise ENOMEM if insufficient memory was available
 * @raise EMFILE if process `RLIMIT_NOFILE` has been reached
 * @raise ENFILE if system-wide file limit has been reached
 * @raise EOPNOTSUPP if `file` names a named socket
 * @raise EFAULT if `file` points to invalid memory
 * @raise ETXTBSY if writing is requested on `file` that's being executed
 * @raise ELOOP if `flags` had `O_NOFOLLOW` and `file` is a symbolic link
 * @raise ELOOP if a loop was detected resolving components of `file`
 * @raise EISDIR if writing is requested and `file` names a directory
 * @cancellationpoint
 * @asyncsignalsafe
 * @restartable
 * @threadsafe
 * @vforksafe
 */
int openat(int dirfd, const char *file, int flags, ...) {
  int rc;
  va_list va;
  unsigned mode;
  struct ZiposUri zipname;
  va_start(va, flags);
  mode = va_arg(va, unsigned);
  va_end(va);
  BEGIN_CANCELLATION_POINT;

  if (file && (!IsAsan() || __asan_is_valid_str(file))) {
    if (!__isfdkind(dirfd, kFdZip)) {
      if (_weaken(__zipos_open) &&
          _weaken(__zipos_parseuri)(file, &zipname) != -1) {
        if (!__vforked && dirfd == AT_FDCWD) {
          rc = _weaken(__zipos_open)(&zipname, flags, mode);
        } else {
          rc = enotsup(); /* TODO */
        }
      } else if (!IsWindows() && !IsMetal()) {
        rc = sys_openat(dirfd, file, flags, mode);
      } else if (IsMetal()) {
        rc = sys_openat_metal(dirfd, file, flags, mode);
      } else {
        rc = sys_open_nt(dirfd, file, flags, mode);
      }
    } else {
      rc = enotsup(); /* TODO */
    }
  } else {
    rc = efault();
  }

  END_CANCELLATION_POINT;
  STRACE("openat(%s, %#s, %s, %#o) → %d% m", DescribeDirfd(dirfd), file,
         DescribeOpenFlags(flags), (flags & (O_CREAT | O_TMPFILE)) ? mode : 0,
         rc);
  return rc;
}
