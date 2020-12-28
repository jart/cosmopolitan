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
#include "libc/bits/progn.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/dirent.h"
#include "libc/dce.h"
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

struct dirent$freebsd {
  uint32_t d_fileno;
  uint16_t d_reclen;
  uint8_t d_type;
  uint8_t d_namlen;
  char d_name[256];
};

struct dirstream {
  int64_t tell;
  int64_t fd;
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

static textwindows noinline DIR *opendir$nt(const char *name) {
  int len;
  DIR *res;
  char16_t name16[PATH_MAX];
  if ((len = __mkntpath(name, name16)) == -1) return NULL;
  if (len + 2 + 1 > PATH_MAX) return PROGN(enametoolong(), NULL);
  if (name16[len - 1] == u'/' || name16[len - 1] == u'\\') {
    name16[--len] = u'\0';
  }
  name16[len++] = u'/';
  name16[len++] = u'*';
  name16[len] = u'\0';
  if (!(res = calloc(1, sizeof(DIR)))) return NULL;
  if ((res->fd = FindFirstFile(name16, &res->windata)) != -1) {
    return res;
  } else {
    __winerr();
    free(res);
    return NULL;
  }
}

static textwindows noinline struct dirent *readdir$nt(DIR *dir) {
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
  if (!IsWindows() && !IsXnu()) {
    res = NULL;
    if ((fd = open(name, O_RDONLY | O_DIRECTORY | O_CLOEXEC)) != -1) {
      if (!(res = fdopendir(fd))) close(fd);
    }
    return res;
  } else if (IsWindows()) {
    return opendir$nt(name);
  } else {
    enosys(); /* TODO(jart): Implement me! */
    return NULL;
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
  if (!IsWindows() && !IsXnu()) {
    if ((dir = calloc(1, sizeof(*dir)))) {
      dir->fd = fd;
      return dir;
    }
  } else {
    enosys(); /* TODO(jart): Implement me! */
  }
  return NULL;
}

/**
 * Reads next entry from DIR.
 *
 * This API doesn't define any particular ordering.
 *
 * @param dir is the object opendir() or fdopendir() returned
 * @return next entry or NULL on end or error, which can be
 *     differentiated by setting errno to 0 beforehand
 */
struct dirent *readdir(DIR *dir) {
  int rc;
  struct dirent *ent;
  struct dirent$freebsd *freebsd;
  if (!IsWindows()) {
    if (dir->buf_pos >= dir->buf_end) {
      if (!(rc = getdents(dir->fd, dir->buf,
                          sizeof(dir->buf) - sizeof(dir->ent.d_name))) ||
          rc == -1) {
        return NULL;
      }
      dir->buf_pos = 0;
      dir->buf_end = rc;
    }
    if (IsLinux()) {
      ent = (struct dirent *)(dir->buf + dir->buf_pos);
      dir->buf_pos += ent->d_reclen;
      dir->tell = ent->d_off;
    } else {
      freebsd = (struct dirent$freebsd *)(dir->buf + dir->buf_pos);
      dir->buf_pos += freebsd->d_reclen;
      ent = &dir->ent;
      ent->d_ino = freebsd->d_fileno;
      ent->d_off = dir->tell++;
      ent->d_reclen = freebsd->d_reclen;
      ent->d_type = freebsd->d_type;
      memcpy(ent->d_name, freebsd->d_name, freebsd->d_namlen + 1);
    }
    return ent;
  } else {
    return readdir$nt(dir);
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
  return dir->fd;
}
