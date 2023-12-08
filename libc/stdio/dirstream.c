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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/critbit0.h"
#include "libc/mem/mem.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/byhandlefileinformation.h"
#include "libc/nt/struct/win32finddata.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
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
      struct ZiposUri prefix;
      struct critbit0 found;
    } zip;
    struct {
      unsigned buf_pos;
      unsigned buf_end;
      uint64_t buf[4096];
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

static void lockdir(DIR *dir) {
  pthread_mutex_lock(&dir->lock);
}

static void unlockdir(DIR *dir) {
  pthread_mutex_unlock(&dir->lock);
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
TryAgain:
  while (!dir->isdone &&
         (dir->windata.dwFileAttributes & kNtFileAttributeSystem)) {
    dir->isdone = !FindNextFile(dir->hand, &dir->windata);
  }
  if (dir->isdone) {
    return NULL;
  }

  // join absolute path that's already normalized
  uint64_t ino = 0;
  char16_t jp[PATH_MAX];
  size_t i = dir->name16len - 1;  // foo\* -> foo\ (strip star)
  bool pretend_this_file_doesnt_exist = false;
  memcpy(jp, dir->name16, i * sizeof(char16_t));
  char16_t *p = dir->windata.cFileName;
  if (p[0] == u'.' && p[1] == u'\0') {
    // join("foo\\", ".") -> "foo\\"
  } else if (p[0] == u'.' && p[1] == u'.' && p[2] == u'\0') {
    if (i == 7 &&         //
        jp[0] == '\\' &&  //
        jp[1] == '\\' &&  //
        jp[2] == '?' &&   //
        jp[3] == '\\' &&  //
        jp[5] == ':' &&   //
        jp[6] == '\\') {
      // e.g. \\?\C:\ stays the same
    } else {
      --i;  // foo\bar\ -> foo\ (parent)
      while (i && jp[i - 1] != '\\') --i;
    }
  } else {
    while (*p) {
      if (i + 1 < ARRAYLEN(jp)) {
        jp[i++] = *p++;
      } else {
        pretend_this_file_doesnt_exist = true;
        goto GiveUpOnGettingInode;
      }
    }
  }
  jp[i] = u'\0';

  // get inode such that it's consistent with stat()
  // it's important that we not follow symlinks here
  int64_t fh =
      CreateFile(jp, kNtFileReadAttributes,
                 kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete, 0,
                 kNtOpenExisting,
                 kNtFileAttributeNormal | kNtFileFlagBackupSemantics |
                     kNtFileFlagOpenReparsePoint,
                 0);
  if (fh != kNtInvalidHandleValue) {
    struct NtByHandleFileInformation wst;
    if (GetFileInformationByHandle(fh, &wst)) {
      ino = (uint64_t)wst.nFileIndexHigh << 32 | wst.nFileIndexLow;
    }
    CloseHandle(fh);
  } else {
    // ignore errors and set inode to zero
    STRACE("failed to get inode of path join(%#hs, %#hs) -> %#hs %m",
           dir->name16, dir->windata.cFileName, jp);
    pretend_this_file_doesnt_exist = true;
  }

GiveUpOnGettingInode:
  // create result object
  bzero(&dir->ent, sizeof(dir->ent));
  dir->ent.d_ino = ino;
  dir->ent.d_off = dir->tell++;
  tprecode16to8(dir->ent.d_name, sizeof(dir->ent.d_name),
                dir->windata.cFileName);
  dir->ent.d_type = GetNtDirentType(&dir->windata);
  dir->isdone = !FindNextFile(dir->hand, &dir->windata);
  if (pretend_this_file_doesnt_exist) goto TryAgain;
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

  // sanity check file descriptor
  struct stat st;
  if (fstat(fd, &st) == -1) {
    return 0;
  }
  if (!S_ISDIR(st.st_mode)) {
    enotdir();
    return 0;
  }
  if (IsLinux() && (__sys_fcntl(fd, F_GETFL) & O_PATH)) {
    ebadf();
    return 0;
  }

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
  const char *name;
  if (h->cfile != ZIPOS_SYNTHETIC_DIRECTORY) {
    len = ZIP_CFILE_NAMESIZE(h->zipos->map + h->cfile);
    name = ZIP_CFILE_NAME(h->zipos->map + h->cfile);
  } else {
    len = h->size;
    name = (const char *)h->data;
  }
  if (len + 2 > ZIPOS_PATH_MAX) {
    free(dir);
    enametoolong();
    return 0;
  }
  if (len) memcpy(dir->zip.prefix.path, name, len);
  if (len && dir->zip.prefix.path[len - 1] != '/') {
    dir->zip.prefix.path[len++] = '/';
  }
  dir->zip.prefix.path[len] = 0;
  dir->zip.prefix.len = len;

  // setup state values for directory iterator
  dir->zip.zipos = h->zipos;
  dir->zip.offset = GetZipCdirOffset(h->zipos->cdir);
  dir->zip.records = GetZipCdirRecords(h->zipos->cdir);
  dir->zip.inode = __zipos_inode(h->zipos, h->cfile, dir->zip.prefix.path,
                                 dir->zip.prefix.len);

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
 * @cancelationpoint
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
    if (!dir->tell) {
      ent = &dir->ent;
      ent->d_off = dir->tell;
      ent->d_ino = dir->zip.inode;
      ent->d_type = DT_DIR;
      ent->d_name[0] = '.';
      ent->d_name[1] = 0;
    } else if (dir->tell == 1) {
      ent = &dir->ent;
      ent->d_off = dir->tell;
      ent->d_type = DT_DIR;
      ent->d_name[0] = '.';
      ent->d_name[1] = '.';
      ent->d_name[2] = 0;
      struct ZiposUri p;
      p.len = dir->zip.prefix.len;
      if (p.len) memcpy(p.path, dir->zip.prefix.path, p.len);
      while (p.len && p.path[p.len - 1] == '/') --p.len;
      while (p.len && p.path[p.len - 1] != '/') --p.len;
      while (p.len && p.path[p.len - 1] == '/') --p.len;
      p.path[p.len] = 0;
      ent->d_ino = __zipos_inode(
          dir->zip.zipos, __zipos_scan(dir->zip.zipos, &p), p.path, p.len);
    } else {
      const char *s = ZIP_CFILE_NAME(dir->zip.zipos->map + dir->zip.offset);
      size_t n = ZIP_CFILE_NAMESIZE(dir->zip.zipos->map + dir->zip.offset);
      if (n > dir->zip.prefix.len &&
          !memcmp(dir->zip.prefix.path, s, dir->zip.prefix.len)) {
        s += dir->zip.prefix.len;
        n -= dir->zip.prefix.len;
        const char *p = memchr(s, '/', n);
        int d_type;
        if (p) {
          n = p - s;
          d_type = DT_DIR;
        } else if (S_ISDIR(GetZipCfileMode(dir->zip.zipos->map +
                                           dir->zip.offset))) {
          d_type = DT_DIR;
        } else {
          d_type = DT_REG;
        }
        if ((n = MIN(n, sizeof(ent->d_name) - 1)) &&
            critbit0_emplace(&dir->zip.found, s, n) == 1) {
          ent = &dir->ent;
          ent->d_ino = dir->zip.offset;
          ent->d_off = dir->tell;
          ent->d_type = d_type;
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
    if (IsNetbsd() || IsFreebsd()) {
      unsigned long seeky = lseek(dir->fd, 0, SEEK_CUR);
      unassert(seeky <= INT_MAX);
      dir->tell = seeky << 32;
    }
    int rc = sys_getdents(dir->fd, dir->buf, sizeof(dir->buf), &basep);
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
  } else if (IsWindows()) {
    return readdir_nt(dir);
  } else {
    return readdir_unix(dir);
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
 * @threadunsafe
 */
struct dirent *readdir(DIR *dir) {
  struct dirent *e;
  if (dir) {
    lockdir(dir);
    e = readdir_impl(dir);
    unlockdir(dir);
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
 */
errno_t readdir_r(DIR *dir, struct dirent *output, struct dirent **result) {
  int err, olderr;
  struct dirent *entry;
  lockdir(dir);
  olderr = errno;
  errno = 0;
  entry = readdir_impl(dir);
  err = errno;
  errno = olderr;
  if (err) {
    unlockdir(dir);
    return err;
  }
  if (entry) {
    if (entry->d_reclen) {
      memcpy(output, entry, entry->d_reclen);
    }
  } else {
    output = 0;
  }
  unlockdir(dir);
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
 */
long telldir(DIR *dir) {
  long rc;
  lockdir(dir);
  rc = dir->tell;
  unlockdir(dir);
  return rc;
}

/**
 * Returns file descriptor associated with DIR object.
 */
int dirfd(DIR *dir) {
  return dir->fd;
}

/**
 * Seeks to beginning of directory stream.
 */
void rewinddir(DIR *dir) {
  lockdir(dir);
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
  unlockdir(dir);
}

/**
 * Seeks in directory stream.
 */
void seekdir(DIR *dir, long tell) {
  lockdir(dir);
  if (dir->iszip) {
    critbit0_clear(&dir->zip.found);
    dir->tell = 0;
    dir->zip.offset = GetZipCdirOffset(dir->zip.zipos->cdir);
    while (dir->tell < tell) {
      if (!readdir_zipos(dir)) {
        break;
      }
    }
  } else if (IsNetbsd() || IsFreebsd()) {
    dir->buf_pos = dir->buf_end = 0;
    dir->tell = lseek(dir->fd, tell >> 32, SEEK_SET) << 32;
    while (dir->tell < tell) {
      if (!readdir_unix(dir)) {
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
  unlockdir(dir);
}

__weak_reference(readdir, readdir64);
__weak_reference(readdir_r, readdir_r64);
