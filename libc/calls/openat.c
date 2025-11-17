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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/log/log.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

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
 *     __static_yoink("zipos");
 *
 * Then you can read zip assets by adding a `"/zip/..."` prefix to `path`, e.g.
 *
 *     // run `zip program.com hi.txt` beforehand
 *     openat(AT_FDCWD, "/zip/hi.txt", O_RDONLY);
 *
 * Cosmopolitan uses tricks like `/C/Windows` to make UNIX-like
 * filesystem paths possible on Windows. To understand better how this
 * works, read the documentation for __mkntpath() and __getcosmosdrive().
 *
 * On Windows, Cosmopolitan emulates the following device files, which
 * must be specified as an absolute case-sensitive UNIX style path:
 *
 * - `/dev/null` using `CreateFile(u"NUL")`
 * - `/dev/random` using ProcessPrng()
 * - `/dev/urandom` using ProcessPrng()
 * - `/dev/stdin` using `dup(0)`
 * - `/dev/stdout` using `dup(1)`
 * - `/dev/stderr` using `dup(2)`
 * - `/dev/fd/x` using `dup(x)`
 *
 * To prevent the most outrageously evil kinds of files from being
 * created, this wrapper may raise `EILSEQ` in some cases when When
 * `O_CREAT` is used, and the file doesn't currently exist. You're not
 * allowed to create a file with a newline character. You're also not
 * allowed to create a filename that has malformed or overlong utf-8
 * sequences. However you can still operate on ones that do exist.
 *
 * @param dirfd is normally `AT_FDCWD` but if it's an open directory and
 *     `path` names a relative path then it's opened relative to `dirfd`
 * @param path is a UTF-8 string naming a filesystem entity
 * @param flags must have one of the following under the `O_ACCMODE` bits:
 *     - `O_RDONLY`     to open `path` for reading only
 *     - `O_WRONLY`     to open `path` for writing
 *     - `O_RDWR`       to open `path` for reading and writing
 *     The following may optionally be bitwise or'd into `flags`:
 *     - `O_CREAT`      create file if it doesn't exist
 *     - `O_TRUNC`      automatic `ftruncate(fd,0)` if exists (atomic on unix)
 *     - `O_CLOEXEC`    automatic close() upon execve()
 *     - `O_EXCL`       exclusive access (see below)
 *     - `O_APPEND`     open file for appending only
 *     - `O_NOFOLLOW`   fail with ELOOP if it's a symlink
 *     - `O_NONBLOCK`   asks read/write to fail with `EAGAIN` rather than block
 *     - `O_UNLINK`     delete file automatically on close
 *     - `O_NOCTTY`     prevents `path` from becoming the controlling terminal
 *     - `O_DIRECTORY`  advisory feature for avoiding accidentally opening files
 *     - `O_SYNC`       makes file operations synchronize appropriately
 *     - `O_DSYNC`      makes write() flush data asap (10x-1000x slower!)
 *     - `O_RSYNC`      makes read() flush data asap (10x-1000x slower!)
 *     - `O_DIRECT`     bypass kernel cache when doing read/write (10x slower!)
 *     - `O_NOATIME`    don't record access time (zero on non-Linux)
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
 * @raise EPERM if `mode & 07000` is not authorized or nonzero on Windows
 * @raise EACCES if unveil() is in play and didn't unveil your `path` path
 * @raise EACCES if we don't have permission to search a component of `path`
 * @raise EACCES if file exists but requested `flags & O_ACCMODE` was denied
 * @raise EACCES if file doesn't exist and parent dir lacks write permissions
 * @raise EACCES if `O_TRUNC` was specified in `flags` but writing was denied
 * @raise ENOTSUP if `path` is on zip file system and `dirfd` isn't `AT_FDCWD`
 * @raise ENOEXEC if `path` is a zip path and this executable isn't a zip file
 * @raise ENOTDIR if a directory component in `path` exists as non-directory
 * @raise ENOTDIR if `path` ends with a trailing slash and refers to a file
 * @raise ENOTDIR if `path` is relative and `dirfd` isn't an open directory
 * @raise ENOTDIR if `path` isn't a directory and `O_DIRECTORY` was passed
 * @raise EILSEQ if creating file and last component of `path` had newline
 * @raise EILSEQ if creating and last component had trailing dots or spaces
 * @raise EILSEQ on Windows if `path` has bad utf-8 of control characters
 * @raise EROFS when writing is requested w/ `path` on read-only filesystem
 * @raise ENAMETOOLONG if symlink-resolved `path` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `path` exists longer than `NAME_MAX`
 * @raise ENOTSUP if `path` is on zip file system and process is vfork()'d
 * @raise ENOSPC if file system is full when `path` would be `O_CREAT`ed
 * @raise EINTR if we needed to block and a signal was delivered instead
 * @raise EEXIST if `O_CREAT|O_EXCL` are used and `path` already existed
 * @raise EINVAL if ASCII control codes are used in `path` on Windows
 * @raise EINVAL if `O_UNLINK` is used without `O_CREAT|O_EXCL`
 * @raise EINVAL if `O_TRUNC` is specified in `O_RDONLY` mode
 * @raise EINVAL if `flags` contains unsupported bits
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise ENOENT if `path` doesn't exist when `O_CREAT` isn't in `flags`
 * @raise ENOENT if `path` points to a string that's empty
 * @raise ENOMEM if insufficient memory was available to implementation
 * @raise EMFILE if process `RLIMIT_NOFILE` has been reached
 * @raise ENFILE if system-wide file limit has been reached
 * @raise EOPNOTSUPP if `path` names a named socket
 * @raise EFAULT if `path` points to invalid memory
 * @raise ETXTBSY if writing is requested on `path` that's being executed
 * @raise ELOOP if `flags` had `O_NOFOLLOW` and `path` is a symbolic link
 * @raise ELOOP if a loop was detected resolving components of `path`
 * @raise EISDIR if writing is requested and `path` names a directory
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable
 * @vforksafe
 */
int openat(int dirfd, const char *path, int flags, ...) {
  int rc;
  va_list va;
  unsigned mode;
  struct stat st;
  struct ZiposUri zipname;
  va_start(va, flags);
  mode = va_arg(va, unsigned);
  va_end(va);
  BEGIN_CANCELATION_POINT;

  if (kisdangerous(path)) {
    rc = efault();
  } else if ((flags & O_CREAT) && __is_evil_path(path) &&
             fstatat(dirfd, path, &st, AT_SYMLINK_NOFOLLOW)) {
    rc = eilseq();
  } else if ((flags & O_UNLINK) &&
             (flags & (O_CREAT | O_EXCL)) != (O_CREAT | O_EXCL)) {
    // O_UNLINK is a non-standard cosmo extension; we've chosen bits for
    // this magic number which we believe are unlikely to interfere with
    // the bits chosen by operating systems both today and in the future
    // however, due to the risks here and the irregularity of using this
    // feature for anything but temporary files, we are going to prevent
    // the clever use cases for now; please file an issue if you want it
    rc = einval();
  } else if (__isfdkind(dirfd, kFdZip)) {
    rc = enotsup();  // TODO
  } else if (_weaken(__zipos_open) &&
             _weaken(__zipos_parseuri)(path, &zipname) != -1) {
    if (!__vforked && dirfd == AT_FDCWD) {
      rc = _weaken(__zipos_open)(&zipname, flags);
    } else {
      rc = enotsup();  // TODO
    }
  } else if ((flags & O_ACCMODE) == O_RDONLY && (flags & O_TRUNC)) {
    // Every operating system we've tested (with the notable exception
    // of OpenBSD) will gladly truncate files opened in read-only mode
    rc = einval();
  } else if (IsLinux() || IsXnu() || IsFreebsd() || IsOpenbsd() || IsNetbsd()) {
    // openat unix userspace
    rc = sys_openat(dirfd, path, flags & ~O_UNLINK, mode);
    if (rc != -1) {
      // openat succeeded
      if (flags & O_UNLINK) {
        // Implement Cosmopolitan O_UNLINK extension for UNIX
        // This cannot fail since we require O_CREAT / O_EXCL
        unassert(!sys_unlinkat(dirfd, path, 0));
      }
    } else {
      // openat failed
      if (IsFreebsd() && errno == EMLINK) {
        // Address FreeBSD divergence from IEEE Std 1003.1-2008 (POSIX.1)
        // in the case when O_NOFOLLOW is used, but fails due to symlink.
        errno = ELOOP;
      }
      if (IsNetbsd() && errno == EFTYPE) {
        // Address NetBSD divergence from IEEE Std 1003.1-2008 (POSIX.1)
        // in the case when O_NOFOLLOW is used but fails due to symlink.
        errno = ELOOP;
      }
    }
  } else if (IsMetal()) {
    rc = sys_openat_metal(dirfd, path, flags, mode);
  } else if (IsWindows()) {
    rc = sys_open_nt(dirfd, path, flags, mode);
  } else {
    rc = enosys();
  }

  END_CANCELATION_POINT;
  STRACE("openat(%s, %#s, %s%s) → %d% m", DescribeDirfd(dirfd), path,
         DescribeOpenFlags(flags), DescribeOpenMode(flags, mode), rc);
  return rc;
}

__weak_reference(openat, openat64);
