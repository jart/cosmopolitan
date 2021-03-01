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
#include "libc/bits/bits.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/dirent.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/win32finddata.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * @fileoverview Directory Streams for Linux+Mac+Windows+FreeBSD+OpenBSD.
 *
 * System interfaces for listing the contents of file system directories
 * are famously incompatible across platforms. Most native projects that
 * have been around a long time implement wrappers for this. Normally it
 * will only be for DOS or Windows support. So this is the first time it
 * has been done for five platforms, having a remarkably tiny footprint.
 */

/**
 * Directory stream object.
 */
struct dirstream {
  int64_t fd;
  int64_t tell;
  struct dirent ent;
  union {
    struct {
      unsigned buf_pos;
      unsigned buf_end;
      char buf[BUFSIZ];
    };
    struct {
      bool isdone;
      struct NtWin32FindData windata;
    };
  };
};

/**
 * FreeBSD getdents() and XNU getdirentries() ABI.
 */
struct dirent_bsd {
  uint32_t d_fileno;
  uint16_t d_reclen;
  uint8_t d_type;
  uint8_t d_namlen;
  char d_name[256];
};

/**
 * OpenBSD getdents() ABI.
 */
struct dirent_openbsd {
  uint64_t d_fileno;
  int64_t d_off;
  uint16_t d_reclen;
  uint8_t d_type;
  uint8_t d_namlen;
  uint8_t __zomg[4];
  char d_name[256];
};

/**
 * NetBSD getdents().
 */
struct dirent_netbsd {
  uint64_t d_fileno;
  uint16_t d_reclen;
  uint16_t d_namlen;
  uint8_t d_type;
  char d_name[512];
};

static textwindows DIR *opendir_nt_impl(char16_t name[PATH_MAX], size_t len) {
  DIR *res;
  if (len + 2 + 1 <= PATH_MAX) {
    if (name[len - 1] != u'\\') name[len++] = u'\\';
    name[len++] = u'*';
    name[len] = u'\0';
    if ((res = calloc(1, sizeof(DIR)))) {
      if ((res->fd = FindFirstFile(name, &res->windata)) != -1) {
        return res;
      } else {
        __winerr();
      }
      free(res);
    }
  } else {
    enametoolong();
  }
  return NULL;
}

static textwindows noinline DIR *opendir_nt(const char *path) {
  int len;
  char16_t name[PATH_MAX];
  if ((len = __mkntpath(path, name)) == -1) return NULL;
  return opendir_nt_impl(name, len);
}

static textwindows noinline DIR *fdopendir_nt(int fd) {
  DIR *res;
  char16_t name[PATH_MAX];
  if (__isfdkind(fd, kFdFile)) {
    if ((res = opendir_nt_impl(
             name, GetFinalPathNameByHandle(
                       g_fds.p[fd].handle, name, ARRAYLEN(name),
                       kNtFileNameNormalized | kNtVolumeNameDos)))) {
      close(fd);
      return res;
    }
  } else {
    ebadf();
  }
  return NULL;
}

static textwindows noinline struct dirent *readdir_nt(DIR *dir) {
  if (!dir->isdone) {
    memset(&dir->ent, 0, sizeof(dir->ent));
    dir->ent.d_ino = 0;
    dir->ent.d_off = dir->tell++;
    dir->ent.d_reclen = sizeof(dir->ent) +
                        tprecode16to8(dir->ent.d_name, sizeof(dir->ent.d_name),
                                      dir->windata.cFileName)
                            .ax +
                        1;
    switch (dir->windata.dwFileType) {
      case kNtFileTypeDisk:
        dir->ent.d_type = DT_BLK;
        break;
      case kNtFileTypeChar:
        dir->ent.d_type = DT_CHR;
        break;
      case kNtFileTypePipe:
        dir->ent.d_type = DT_FIFO;
        break;
      default:
        if (dir->windata.dwFileAttributes & kNtFileAttributeDirectory) {
          dir->ent.d_type = DT_DIR;
        } else {
          dir->ent.d_type = DT_REG;
        }
        break;
    }
    dir->isdone = !FindNextFile(dir->fd, &dir->windata);
    return &dir->ent;
  } else {
    return NULL;
  }
}

/**
 * Opens directory, e.g.
 *
 *     DIR *d;
 *     struct dirent *e;
 *     CHECK((d = opendir(path)));
 *     while ((e = readdir(d))) {
 *       printf("%s/%s\n", path, e->d_name);
 *     }
 *     LOGIFNEG1(closedir(d));
 *
 * @returns newly allocated DIR object, or NULL w/ errno
 * @errors ENOENT, ENOTDIR, EACCES, EMFILE, ENFILE, ENOMEM
 * @see glob()
 */
DIR *opendir(const char *name) {
  int fd;
  DIR *res;
  if (!IsWindows()) {
    res = NULL;
    if ((fd = open(name, O_RDONLY | O_DIRECTORY | O_CLOEXEC)) != -1) {
      if (!(res = fdopendir(fd))) close(fd);
    }
    return res;
  } else {
    return opendir_nt(name);
  }
}

/**
 * Creates directory object for file descriptor.
 *
 * @param fd gets owned by this function, if it succeeds
 * @return new directory object, which must be freed by closedir(),
 *     or NULL w/ errno
 * @errors ENOMEM and fd is closed
 */
DIR *fdopendir(int fd) {
  DIR *dir;
  if (!IsWindows()) {
    if (!(dir = calloc(1, sizeof(*dir)))) return NULL;
    dir->fd = fd;
    return dir;
  } else {
    return fdopendir_nt(fd);
  }
}

/**
 * Reads next entry from directory stream.
 *
 * This API doesn't define any particular ordering.
 *
 * @param dir is the object opendir() or fdopendir() returned
 * @return next entry or NULL on end or error, which can be
 *     differentiated by setting errno to 0 beforehand
 */
struct dirent *readdir(DIR *dir) {
  int rc;
  long basep;
  struct dirent *ent;
  struct dirent_bsd *bsd;
  struct dirent_netbsd *nbsd;
  struct dirent_openbsd *obsd;
  if (!IsWindows()) {
    if (dir->buf_pos >= dir->buf_end) {
      basep = dir->tell; /* TODO(jart): what does xnu do */
      rc = getdents(dir->fd, dir->buf, sizeof(dir->buf) - 256, &basep);
      if (!rc || rc == -1) return NULL;
      dir->buf_pos = 0;
      dir->buf_end = rc;
    }
    if (IsLinux()) {
      ent = (struct dirent *)(dir->buf + dir->buf_pos);
      dir->buf_pos += ent->d_reclen;
      dir->tell = ent->d_off;
    } else if (IsOpenbsd()) {
      obsd = (struct dirent_openbsd *)(dir->buf + dir->buf_pos);
      dir->buf_pos += obsd->d_reclen;
      ent = &dir->ent;
      ent->d_ino = obsd->d_fileno;
      ent->d_off = obsd->d_off;
      ent->d_reclen = obsd->d_reclen;
      ent->d_type = obsd->d_type;
      memcpy(ent->d_name, obsd->d_name, obsd->d_namlen + 1);
    } else if (IsNetbsd()) {
      nbsd = (struct dirent_netbsd *)(dir->buf + dir->buf_pos);
      dir->buf_pos += nbsd->d_reclen;
      ent = &dir->ent;
      ent->d_ino = nbsd->d_fileno;
      ent->d_off = dir->tell++;
      ent->d_reclen = nbsd->d_reclen;
      ent->d_type = nbsd->d_type;
      memcpy(ent->d_name, nbsd->d_name, MAX(256, nbsd->d_namlen + 1));
    } else {
      bsd = (struct dirent_bsd *)(dir->buf + dir->buf_pos);
      dir->buf_pos += bsd->d_reclen;
      ent = &dir->ent;
      ent->d_ino = bsd->d_fileno;
      ent->d_off = IsXnu() ? (dir->tell = basep) : dir->tell++;
      ent->d_reclen = bsd->d_reclen;
      ent->d_type = bsd->d_type;
      memcpy(ent->d_name, bsd->d_name, bsd->d_namlen + 1);
    }
    return ent;
  } else {
    return readdir_nt(dir);
  }
}

/**
 * Closes directory object returned by opendir().
 * @return 0 on success or -1 w/ errno
 */
int closedir(DIR *dir) {
  int rc;
  if (dir) {
    if (!IsWindows()) {
      rc = close(dir->fd);
    } else {
      rc = FindClose(dir->fd) ? 0 : __winerr();
    }
    free(dir);
  } else {
    rc = 0;
  }
  return rc;
}

/**
 * Returns offset into directory data.
 */
long telldir(DIR *dir) {
  return dir->tell;
}

/**
 * Returns file descriptor associated with DIR object.
 */
int dirfd(DIR *dir) {
  if (IsWindows()) return eopnotsupp();
  return dir->fd;
}
