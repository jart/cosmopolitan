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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/mem/critbit0.h"
#include "libc/mem/mem.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/win32finddata.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/zip.internal.h"

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
  int fd;
  bool iszip;
  long tell;
  int64_t hand;
  pthread_mutex_t lock;
  struct dirent ent;
  union {
    struct {
      struct Zipos *zipos;
      uint64_t inode;
      uint64_t offset;
      uint64_t records;
      size_t prefixlen;
      uint8_t prefix[ZIPOS_PATH_MAX];
      struct critbit0 found;
    } zip;
    struct {
      unsigned buf_pos;
      unsigned buf_end;
      uint64_t buf[(BUFSIZ + 256) / 8];
    };
    struct {
      bool isdone;
      struct NtWin32FindData windata;
      char16_t name16[PATH_MAX];
      uint32_t name16len;
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

static void _lockdir(DIR *dir) {
  if (__threaded) {
    pthread_mutex_lock(&dir->lock);
  }
}

static void _unlockdir(DIR *dir) {
  if (__threaded) {
    pthread_mutex_unlock(&dir->lock);
  }
}

static textwindows dontinline int fdopendir_nt(DIR *res, int fd) {
  if (!__isfdkind(fd, kFdFile)) {
    return ebadf();
  }
  res->name16len = GetFinalPathNameByHandle(
      g_fds.p[fd].handle, res->name16, ARRAYLEN(res->name16),
      kNtFileNameNormalized | kNtVolumeNameDos);
  if (!res->name16len) {
    return __winerr();
  }
  if (res->name16len + 2 + 1 > ARRAYLEN(res->name16)) {
    return enametoolong();
  }
  if (res->name16len > 1 && res->name16[res->name16len - 1] != u'\\') {
    res->name16[res->name16len++] = u'\\';
  }
  res->name16[res->name16len++] = u'*';
  res->name16[res->name16len] = u'\0';
  if ((res->hand = FindFirstFile(res->name16, &res->windata)) == -1) {
    return __fix_enotdir(-1, res->name16);
  }
  return 0;
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
  if (dir->isdone) {
    return NULL;
  }

  // join absolute path
  uint64_t ino = 0;
  size_t i = dir->name16len - 1;
  char16_t *p = dir->windata.cFileName;
  while (*p) {
    if (i + 1 < ARRAYLEN(dir->name16)) {
      dir->name16[i++] = *p++;
    } else {
      // ignore errors and set inode to zero
      goto GiveUpOnGettingInode;
    }
  }
  dir->name16[i] = u'\0';

  // get inode that's consistent with stat()
  int e = errno;
  int64_t fh =
      CreateFile(dir->name16, kNtFileReadAttributes, 0, 0, kNtOpenExisting,
                 kNtFileAttributeNormal | kNtFileFlagBackupSemantics |
                     kNtFileFlagOpenReparsePoint,
                 0);
  if (fh != -1) {
    struct NtByHandleFileInformation wst;
    if (GetFileInformationByHandle(fh, &wst)) {
      ino = (uint64_t)wst.nFileIndexHigh << 32 | wst.nFileIndexLow;
    }
    CloseHandle(fh);
  } else {
    // ignore errors and set inode to zero
    // TODO(jart): How do we handle "." and ".."?
    errno = e;
  }

GiveUpOnGettingInode:
  // restore original directory search path
  dir->name16[dir->name16len - 1] = u'*';
  dir->name16[dir->name16len] = u'\0';

  // create result object
  bzero(&dir->ent, sizeof(dir->ent));
  dir->ent.d_ino = ino;
  dir->ent.d_off = dir->tell++;
  tprecode16to8(dir->ent.d_name, sizeof(dir->ent.d_name),
                dir->windata.cFileName);
  dir->ent.d_type = GetNtDirentType(&dir->windata);
  dir->isdone = !FindNextFile(dir->hand, &dir->windata);
  return &dir->ent;
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

  // allocate directory iterator object
  DIR *dir;
  if (!(dir = calloc(1, sizeof(*dir)))) {
    return NULL;
  }

  // on unix, file descriptor isn't required to be tracked
  dir->fd = fd;
  if (!__isfdkind(fd, kFdZip)) {
    if (IsWindows()) {
      if (!fdopendir_nt(dir, fd)) {
        return dir;
      } else {
        free(dir);
        return 0;
      }
    }
    return dir;
  }
  dir->iszip = true;

  // ensure open /zip/... file is a directory
  struct ZiposHandle *h = (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle;
  if (h->cfile != ZIPOS_SYNTHETIC_DIRECTORY &&
      !S_ISDIR(GetZipCfileMode(h->zipos->map + h->cfile))) {
    free(dir);
    enotdir();
    return 0;
  }

  // get path of this file descriptor and ensure trailing slash
  size_t len;
  char *name;
  if (h->cfile != ZIPOS_SYNTHETIC_DIRECTORY) {
    len = ZIP_CFILE_NAMESIZE(h->zipos->map + h->cfile);
    name = ZIP_CFILE_NAME(h->zipos->map + h->cfile);
  } else {
    len = h->size;
    name = (char *)h->data;
  }
  if (len + 2 > ZIPOS_PATH_MAX) {
    free(dir);
    enametoolong();
    return 0;
  }
  if (len) memcpy(dir->zip.prefix, name, len);
  if (len && dir->zip.prefix[len - 1] != '/') {
    dir->zip.prefix[len++] = '/';
  }
  dir->zip.prefix[len] = 0;
  dir->zip.prefixlen = len;

  // setup state values for directory iterator
  dir->zip.zipos = h->zipos;
  dir->zip.inode = h->cfile;
  dir->zip.offset = GetZipCdirOffset(h->zipos->cdir);
  dir->zip.records = GetZipCdirRecords(h->zipos->cdir);

  return dir;
}

/**
 * Opens directory, e.g.
 *
 *     DIR *d;
 *     struct dirent *e;
 *     d = opendir(path);
 *     while ((e = readdir(d))) {
 *       printf("%s/%s\n", path, e->d_name);
 *     }
 *     closedir(d);
 *
 * @returns newly allocated DIR object, or NULL w/ errno
 * @errors ENOENT, ENOTDIR, EACCES, EMFILE, ENFILE, ENOMEM
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINTR if we needed to block and a signal was delivered instead
 * @cancellationpoint
 * @see glob()
 */
DIR *opendir(const char *name) {
  int rc;
  if (_weaken(pthread_testcancel_np) &&
      (rc = _weaken(pthread_testcancel_np)())) {
    errno = rc;
    return 0;
  }
  int fd;
  if ((fd = open(name, O_RDONLY | O_DIRECTORY | O_NOCTTY | O_CLOEXEC)) == -1) {
    return 0;
  }
  DIR *res = fdopendir(fd);
  if (!res) close(fd);
  return res;
}

static struct dirent *readdir_zipos(DIR *dir) {
  struct dirent *ent = 0;
  while (!ent && dir->tell < dir->zip.records + 2) {
    size_t n;
    if (!dir->tell) {
      ent = &dir->ent;
      ent->d_off = dir->tell;
      ent->d_ino = dir->zip.inode;
      ent->d_type = DT_DIR;
      ent->d_name[0] = '.';
      ent->d_name[1] = 0;
      n = 1;
    } else if (dir->tell == 1) {
      ent = &dir->ent;
      ent->d_off = dir->tell;
      ent->d_ino = 0;  // TODO
      ent->d_type = DT_DIR;
      ent->d_name[0] = '.';
      ent->d_name[1] = '.';
      ent->d_name[2] = 0;
      n = 2;
    } else {
      uint8_t *s = ZIP_CFILE_NAME(dir->zip.zipos->map + dir->zip.offset);
      n = ZIP_CFILE_NAMESIZE(dir->zip.zipos->map + dir->zip.offset);
      if (n > dir->zip.prefixlen &&
          !memcmp(dir->zip.prefix, s, dir->zip.prefixlen)) {
        s += dir->zip.prefixlen;
        n -= dir->zip.prefixlen;
        uint8_t *p = memchr(s, '/', n);
        if (p) n = p - s;
        if ((n = MIN(n, sizeof(ent->d_name) - 1)) &&
            critbit0_emplace(&dir->zip.found, s, n) == 1) {
          ent = &dir->ent;
          ent->d_ino = dir->zip.offset;
          ent->d_off = dir->tell;
          ent->d_type =
              S_ISDIR(GetZipCfileMode(dir->zip.zipos->map + dir->zip.offset))
                  ? DT_DIR
                  : DT_REG;
          memcpy(ent->d_name, s, n);
          ent->d_name[n] = 0;
        }
      }
      dir->zip.offset +=
          ZIP_CFILE_HDRSIZE(dir->zip.zipos->map + dir->zip.offset);
    }
    dir->tell++;
  }
  return ent;
}

static struct dirent *readdir_unix(DIR *dir) {
  if (dir->buf_pos >= dir->buf_end) {
    long basep = dir->tell;
    int rc = sys_getdents(dir->fd, dir->buf, sizeof(dir->buf) - 256, &basep);
    STRACE("sys_getdents(%d) → %d% m", dir->fd, rc);
    if (!rc || rc == -1) {
      return NULL;
    }
    dir->buf_pos = 0;
    dir->buf_end = rc;
  }
  struct dirent *ent;
  if (IsLinux()) {
    ent = (struct dirent *)((char *)dir->buf + dir->buf_pos);
    dir->buf_pos += ent->d_reclen;
    dir->tell = ent->d_off;
  } else if (IsOpenbsd()) {
    struct dirent_openbsd *obsd =
        (struct dirent_openbsd *)((char *)dir->buf + dir->buf_pos);
    dir->buf_pos += obsd->d_reclen;
    dir->tell = obsd->d_off;
    ent = &dir->ent;
    ent->d_ino = obsd->d_fileno;
    ent->d_off = obsd->d_off;
    ent->d_reclen = obsd->d_reclen;
    ent->d_type = obsd->d_type;
    memcpy(ent->d_name, obsd->d_name, obsd->d_namlen + 1);
  } else if (IsNetbsd()) {
    struct dirent_netbsd *nbsd =
        (struct dirent_netbsd *)((char *)dir->buf + dir->buf_pos);
    dir->buf_pos += nbsd->d_reclen;
    ent = &dir->ent;
    ent->d_ino = nbsd->d_fileno;
    ent->d_off = (dir->tell += nbsd->d_reclen);
    ent->d_reclen = nbsd->d_reclen;
    ent->d_type = nbsd->d_type;
    size_t n =
        MIN(nbsd->d_namlen, MIN(sizeof(ent->d_name) - 1, sizeof(nbsd->d_name)));
    memcpy(ent->d_name, nbsd->d_name, n);
    ent->d_name[n] = 0;
  } else {
    struct dirent_bsd *bsd =
        (struct dirent_bsd *)((char *)dir->buf + dir->buf_pos);
    dir->buf_pos += bsd->d_reclen;
    ent = &dir->ent;
    ent->d_ino = bsd->d_fileno;
    ent->d_off = dir->tell++;
    ent->d_reclen = bsd->d_reclen;
    ent->d_type = bsd->d_type;
    memcpy(ent->d_name, bsd->d_name, bsd->d_namlen + 1);
  }
  if (ent) {
    ent->d_reclen =
        ROUNDUP(offsetof(struct dirent, d_name) + strlen(ent->d_name) + 1, 8);
  }
  return ent;
}

static struct dirent *readdir_impl(DIR *dir) {
  if (dir->iszip) {
    return readdir_zipos(dir);
  }
  if (IsWindows()) {
    return readdir_nt(dir);
  }
  return readdir_unix(dir);
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
  if (dir) {
    _lockdir(dir);
    e = readdir_impl(dir);
    _unlockdir(dir);
  } else {
    efault();
    e = 0;
  }
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
    if (entry->d_reclen) {
      memcpy(output, entry, entry->d_reclen);
    }
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
  int rc = 0;
  if (dir) {
    if (dir->iszip) {
      critbit0_clear(&dir->zip.found);
    }
    if (dir->fd != -1) {
      rc |= close(dir->fd);
    }
    if (IsWindows() && !dir->iszip) {
      if (!FindClose(dir->hand)) {
        rc = __winerr();
      }
    }
    free(dir);
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
  return dir->fd;
}

/**
 * Seeks to beginning of directory stream.
 * @threadsafe
 */
void rewinddir(DIR *dir) {
  _lockdir(dir);
  if (dir->iszip) {
    critbit0_clear(&dir->zip.found);
    dir->tell = 0;
    dir->zip.offset = GetZipCdirOffset(dir->zip.zipos->cdir);
  } else if (!IsWindows()) {
    if (!lseek(dir->fd, 0, SEEK_SET)) {
      dir->buf_pos = dir->buf_end = 0;
      dir->tell = 0;
    }
  } else {
    FindClose(dir->hand);
    if ((dir->hand = FindFirstFile(dir->name16, &dir->windata)) != -1) {
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
void seekdir(DIR *dir, long tell) {
  _lockdir(dir);
  if (dir->iszip) {
    critbit0_clear(&dir->zip.found);
    dir->tell = 0;
    dir->zip.offset = GetZipCdirOffset(dir->zip.zipos->cdir);
    while (dir->tell < tell) {
      if (!readdir_zipos(dir)) {
        break;
      }
    }
  } else if (!IsWindows()) {
    dir->tell = lseek(dir->fd, tell, SEEK_SET);
    dir->buf_pos = dir->buf_end = 0;
  } else {
    dir->tell = 0;
    dir->isdone = false;
    FindClose(dir->hand);
    if ((dir->hand = FindFirstFile(dir->name16, &dir->windata)) != -1) {
      for (; dir->tell < tell; ++dir->tell) {
        if (!FindNextFile(dir->hand, &dir->windata)) {
          dir->isdone = true;
          break;
        }
      }
    } else {
      dir->isdone = true;
    }
  }
  _unlockdir(dir);
}

__weak_reference(readdir, readdir64);
