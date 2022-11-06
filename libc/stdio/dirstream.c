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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/nopl.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/nt/struct/win32finddata.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/zip.h"
#include "libc/zipos/zipos.internal.h"

/**
 * @fileoverview Directory Streams for Linux+Mac+Windows+FreeBSD+OpenBSD.
 *
 * System interfaces for listing the contents of file system directories
 * are famously incompatible across platforms. Most native projects that
 * have been around a long time implement wrappers for this. Normally it
 * will only be for DOS or Windows support. So this is the first time it
 * has been done for five platforms, having a remarkably tiny footprint.
 */

int sys_getdents(unsigned, void *, unsigned, long *);

/**
 * Directory stream object.
 */
struct dirstream {
  bool iszip;
  int64_t fd;
  int64_t tell;
  pthread_mutex_t lock;
  struct {
    uint64_t offset;
    uint64_t records;
    uint8_t *prefix;
    size_t prefixlen;
  } zip;
  struct dirent ent;
  union {
    struct {
      unsigned buf_pos;
      unsigned buf_end;
      uint64_t buf[(BUFSIZ + 256) / 8];
    };
    struct {
      bool isdone;
      char16_t *name;
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

// TODO(jart): wipe these locks when forking

void _lockdir(DIR *dir) {
  pthread_mutex_lock(&dir->lock);
}

void _unlockdir(DIR *dir) {
  pthread_mutex_unlock(&dir->lock);
}

#ifdef _NOPL1
#define _lockdir(d)   _NOPL1("__threadcalls", _lockdir, d)
#define _unlockdir(d) _NOPL1("__threadcalls", _unlockdir, d)
#else
#define _lockdir(d)   (__threaded ? _lockdir(d) : 0)
#define _unlockdir(d) (__threaded ? _unlockdir(d) : 0)
#endif

static textwindows DIR *opendir_nt_impl(char16_t *name, size_t len) {
  DIR *res;
  if (len + 2 + 1 <= PATH_MAX) {
    if (len == 1 && name[0] == '.') {
      name[0] = '*';
    } else {
      if (len > 1 && name[len - 1] != u'\\') {
        name[len++] = u'\\';
      }
      name[len++] = u'*';
    }
    name[len] = u'\0';
    if ((res = calloc(1, sizeof(DIR)))) {
      if ((res->fd = FindFirstFile(name, &res->windata)) != -1) {
        return res;
      }
      __fix_enotdir(-1, name);
      free(res);
    }
  } else {
    enametoolong();
  }
  return NULL;
}

static textwindows dontinline DIR *opendir_nt(const char *path) {
  int len;
  DIR *res;
  char16_t *name;
  if (*path) {
    if ((name = malloc(PATH_MAX * 2))) {
      if ((len = __mkntpath(path, name)) != -1 &&
          (res = opendir_nt_impl(name, len))) {
        res->name = name;
        return res;
      }
      free(name);
    }
  } else {
    enoent();
  }
  return NULL;
}

static textwindows dontinline DIR *fdopendir_nt(int fd) {
  DIR *res;
  char16_t *name;
  if (__isfdkind(fd, kFdFile)) {
    if ((name = malloc(PATH_MAX * 2))) {
      if ((res = opendir_nt_impl(
               name, GetFinalPathNameByHandle(
                         g_fds.p[fd].handle, name, PATH_MAX,
                         kNtFileNameNormalized | kNtVolumeNameDos)))) {
        res->name = name;
        close(fd);
        return res;
      }
      free(name);
    }
  } else {
    ebadf();
  }
  return NULL;
}

static textwindows uint8_t GetNtDirentType(struct NtWin32FindData *w) {
  switch (w->dwFileType) {
    case kNtFileTypeDisk:
      return DT_BLK;
    case kNtFileTypeChar:
      return DT_CHR;
    case kNtFileTypePipe:
      return DT_FIFO;
    default:
      if (w->dwFileAttributes & kNtFileAttributeDirectory) {
        return DT_DIR;
      } else if (w->dwFileAttributes & kNtFileAttributeReparsePoint) {
        return DT_LNK;
      } else {
        return DT_REG;
      }
  }
}

static textwindows dontinline struct dirent *readdir_nt(DIR *dir) {
  size_t i;
  if (!dir->isdone) {
    bzero(&dir->ent, sizeof(dir->ent));
    dir->ent.d_ino++;
    dir->ent.d_off = dir->tell++;
    dir->ent.d_reclen =
        tprecode16to8(dir->ent.d_name, sizeof(dir->ent.d_name) - 2,
                      dir->windata.cFileName)
            .ax;
    for (i = 0; i < dir->ent.d_reclen; ++i) {
      if (dir->ent.d_name[i] == '\\') {
        dir->ent.d_name[i] = '/';
      }
    }
    dir->ent.d_type = GetNtDirentType(&dir->windata);
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
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if we needed to block and a signal was delivered instead
 * @cancellationpoint
 * @see glob()
 */
DIR *opendir(const char *name) {
  DIR *res;
  int fd, rc;
  struct stat st;
  struct Zipos *zip;
  struct ZiposUri zipname;
  if (_weaken(pthread_testcancel_np) &&
      (rc = _weaken(pthread_testcancel_np)())) {
    errno = rc;
    return 0;
  }
  if (!name || (IsAsan() && !__asan_is_valid_str(name))) {
    efault();
    res = 0;
  } else if (_weaken(__zipos_get) &&
             _weaken(__zipos_parseuri)(name, &zipname) != -1) {
    if (_weaken(__zipos_stat)(&zipname, &st) != -1) {
      if (S_ISDIR(st.st_mode)) {
        zip = _weaken(__zipos_get)();
        if ((res = calloc(1, sizeof(DIR)))) {
          res->iszip = true;
          res->fd = -1;
          res->zip.offset = GetZipCdirOffset(zip->cdir);
          res->zip.records = GetZipCdirRecords(zip->cdir);
          res->zip.prefix = malloc(zipname.len + 2);
          memcpy(res->zip.prefix, zipname.path, zipname.len);
          if (zipname.len && res->zip.prefix[zipname.len - 1] != '/') {
            res->zip.prefix[zipname.len++] = '/';
          }
          res->zip.prefix[zipname.len] = '\0';
          res->zip.prefixlen = zipname.len;
        }
      } else {
        enotdir();
        res = 0;
      }
    } else {
      res = 0;
    }
  } else if (!IsWindows()) {
    res = 0;
    if ((fd = open(name, O_RDONLY | O_NOCTTY | O_DIRECTORY | O_CLOEXEC)) !=
        -1) {
      if (!(res = fdopendir(fd))) {
        close(fd);
      }
    }
  } else {
    res = opendir_nt(name);
  }
  return res;
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
  } else {
    dir = fdopendir_nt(fd);
  }
  return dir;
}

static struct dirent *readdir_impl(DIR *dir) {
  size_t n;
  long basep;
  int rc, mode;
  uint8_t *s, *p;
  struct Zipos *zip;
  struct dirent *ent;
  struct dirent *lastent;
  struct dirent_bsd *bsd;
  struct dirent_netbsd *nbsd;
  struct dirent_openbsd *obsd;
  if (dir->iszip) {
    ent = 0;
    zip = _weaken(__zipos_get)();
    while (!ent && dir->tell < dir->zip.records) {
      _npassert(ZIP_CFILE_MAGIC(zip->map + dir->zip.offset) ==
                kZipCfileHdrMagic);
      s = ZIP_CFILE_NAME(zip->map + dir->zip.offset);
      n = ZIP_CFILE_NAMESIZE(zip->map + dir->zip.offset);
      if (dir->zip.prefixlen < n &&
          !memcmp(dir->zip.prefix, s, dir->zip.prefixlen)) {
        s += dir->zip.prefixlen;
        n -= dir->zip.prefixlen;
        p = memchr(s, '/', n);
        if (!p || p + 1 - s == n) {
          if (p + 1 - s == n) --n;
          mode = GetZipCfileMode(zip->map + dir->zip.offset);
          ent = (struct dirent *)dir->buf;
          ent->d_ino++;
          ent->d_off = dir->zip.offset;
          ent->d_reclen = MIN(n, 255);
          ent->d_type = S_ISDIR(mode) ? DT_DIR : DT_REG;
          memcpy(ent->d_name, s, ent->d_reclen);
          ent->d_name[ent->d_reclen] = 0;
        } else {
          lastent = (struct dirent *)dir->buf;
          n = p - s;
          n = MIN(n, 255);
          if (!lastent->d_ino || (n != lastent->d_reclen) ||
              memcmp(lastent->d_name, s, n)) {
            ent = lastent;
            ent->d_ino++;
            ent->d_off = -1;
            ent->d_reclen = n;
            ent->d_type = DT_DIR;
            memcpy(ent->d_name, s, ent->d_reclen);
            ent->d_name[ent->d_reclen] = 0;
          }
        }
      }
      dir->zip.offset += ZIP_CFILE_HDRSIZE(zip->map + dir->zip.offset);
      dir->tell++;
    }
    return ent;
  } else if (!IsWindows()) {
    if (dir->buf_pos >= dir->buf_end) {
      basep = dir->tell; /* TODO(jart): what does xnu do */
      rc = sys_getdents(dir->fd, dir->buf, sizeof(dir->buf) - 256, &basep);
      STRACE("sys_getdents(%d) → %d% m", dir->fd, rc);
      if (!rc || rc == -1) return NULL;
      dir->buf_pos = 0;
      dir->buf_end = rc;
    }
    if (IsLinux()) {
      ent = (struct dirent *)((char *)dir->buf + dir->buf_pos);
      dir->buf_pos += ent->d_reclen;
      dir->tell = ent->d_off;
    } else if (IsOpenbsd()) {
      obsd = (struct dirent_openbsd *)((char *)dir->buf + dir->buf_pos);
      dir->buf_pos += obsd->d_reclen;
      ent = &dir->ent;
      ent->d_ino = obsd->d_fileno;
      ent->d_off = obsd->d_off;
      ent->d_reclen = obsd->d_reclen;
      ent->d_type = obsd->d_type;
      memcpy(ent->d_name, obsd->d_name, obsd->d_namlen + 1);
    } else if (IsNetbsd()) {
      nbsd = (struct dirent_netbsd *)((char *)dir->buf + dir->buf_pos);
      dir->buf_pos += nbsd->d_reclen;
      ent = &dir->ent;
      ent->d_ino = nbsd->d_fileno;
      ent->d_off = dir->tell++;
      ent->d_reclen = nbsd->d_reclen;
      ent->d_type = nbsd->d_type;
      memcpy(ent->d_name, nbsd->d_name, MAX(256, nbsd->d_namlen + 1));
    } else {
      bsd = (struct dirent_bsd *)((char *)dir->buf + dir->buf_pos);
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
 * Reads next entry from directory stream.
 *
 * This API doesn't define any particular ordering.
 *
 * @param dir is the object opendir() or fdopendir() returned
 * @return next entry or NULL on end or error, which can be
 *     differentiated by setting errno to 0 beforehand
 */
struct dirent *readdir(DIR *dir) {
  struct dirent *e;
  _lockdir(dir);
  e = readdir_impl(dir);
  _unlockdir(dir);
  return e;
}

/**
 * Reads directory entry reentrantly.
 *
 * @param dir is the object opendir() or fdopendir() returned
 * @param output is where directory entry is copied if not eof
 * @param result will receive `output` pointer, or null on eof
 * @return 0 on success, or errno on error
 * @returnserrno
 * @threadsafe
 */
errno_t readdir_r(DIR *dir, struct dirent *output, struct dirent **result) {
  int err, olderr;
  struct dirent *entry;
  _lockdir(dir);
  olderr = errno;
  errno = 0;
  entry = readdir_impl(dir);
  err = errno;
  errno = olderr;
  if (err) {
    _unlockdir(dir);
    return err;
  }
  if (entry) {
    memcpy(output, entry, entry->d_reclen);
  } else {
    output = 0;
  }
  _unlockdir(dir);
  *result = output;
  return 0;
}

/**
 * Closes directory object returned by opendir().
 * @return 0 on success or -1 w/ errno
 */
int closedir(DIR *dir) {
  int rc;
  if (dir) {
    if (dir->iszip) {
      free(dir->zip.prefix);
      rc = 0;
    } else if (!IsWindows()) {
      rc = close(dir->fd);
    } else {
      free(dir->name);
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
 * @threadsafe
 */
long telldir(DIR *dir) {
  long rc;
  _lockdir(dir);
  rc = dir->tell;
  _unlockdir(dir);
  return rc;
}

/**
 * Returns file descriptor associated with DIR object.
 * @threadsafe
 */
int dirfd(DIR *dir) {
  int rc;
  _lockdir(dir);
  if (dir->iszip) {
    rc = eopnotsupp();
  } else if (IsWindows()) {
    rc = eopnotsupp();
  } else {
    rc = dir->fd;
  }
  _unlockdir(dir);
  return rc;
}

/**
 * Seeks to beginning of directory stream.
 * @threadsafe
 */
void rewinddir(DIR *dir) {
  _lockdir(dir);
  if (dir->iszip) {
    dir->tell = 0;
    dir->zip.offset = GetZipCdirOffset(_weaken(__zipos_get)()->cdir);
  } else if (!IsWindows()) {
    if (!lseek(dir->fd, 0, SEEK_SET)) {
      dir->buf_pos = dir->buf_end = 0;
      dir->tell = 0;
    }
  } else {
    FindClose(dir->fd);
    if ((dir->fd = FindFirstFile(dir->name, &dir->windata)) != -1) {
      dir->isdone = false;
      dir->tell = 0;
    } else {
      dir->isdone = true;
    }
  }
  _unlockdir(dir);
}

/**
 * Seeks in directory stream.
 * @threadsafe
 */
void seekdir(DIR *dir, long off) {
  long i;
  struct Zipos *zip;
  _lockdir(dir);
  zip = _weaken(__zipos_get)();
  if (dir->iszip) {
    dir->zip.offset = GetZipCdirOffset(_weaken(__zipos_get)()->cdir);
    for (i = 0; i < off && i < dir->zip.records; ++i) {
      dir->zip.offset += ZIP_CFILE_HDRSIZE(zip->map + dir->zip.offset);
    }
  } else {
    i = lseek(dir->fd, off, SEEK_SET);
    dir->buf_pos = dir->buf_end = 0;
  }
  dir->tell = i;
  _unlockdir(dir);
}
