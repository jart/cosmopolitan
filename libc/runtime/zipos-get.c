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
#include "libc/calls/metalfile.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/cosmo.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/mem/alg.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/thread/thread.h"
#include "libc/zip.internal.h"

#ifdef __x86_64__
__static_yoink(APE_COM_NAME);
#endif

struct ZiposPlanner {
  uint8_t buf[kZipLookbehindBytes];
  struct stat st;
};

static struct Zipos __zipos;

static void __zipos_wipe(void) {
  pthread_mutex_init(&__zipos.lock, 0);
}

void __zipos_lock(void) {
  pthread_mutex_lock(&__zipos.lock);
}

void __zipos_unlock(void) {
  pthread_mutex_unlock(&__zipos.lock);
}

static int __zipos_compare(const void *a, const void *b, void *c) {
  uint8_t *cdir = (uint8_t *)c;
  const int *x = (const int *)a;
  const int *y = (const int *)b;
  int xn = ZIP_CFILE_NAMESIZE(cdir + *x);
  int yn = ZIP_CFILE_NAMESIZE(cdir + *y);
  int n = MIN(xn, yn);
  if (n) {
    int res = memcmp(ZIP_CFILE_NAME(cdir + *x), ZIP_CFILE_NAME(cdir + *y), n);
    if (res) return res;
  }
  return xn - yn;  // xn and yn are 16-bit
}

static dontinline int __zipos_plan(int fd, struct ZiposPlanner *p) {

  // get file size and dev/inode
  // this might fail if a bad fd was passed via environment
  if (fstat(fd, &p->st)) {
    return kZipErrorOpenFailed;
  }

  // read the last 64kb of file
  // the zip file format magic can be anywhere in there
  int amt;
  int64_t off;
  if (p->st.st_size <= kZipLookbehindBytes) {
    off = 0;
    amt = p->st.st_size;
  } else {
    off = p->st.st_size - kZipLookbehindBytes;
    amt = p->st.st_size - off;
  }
  if (pread(fd, p->buf, amt, off) != amt) {
    return kZipErrorReadFailed;
  }

  // search backwards for the end-of-central-directory record
  // the eocd (cdir) says where the central directory (cfile) array is located
  // we consistency check some legacy fields, to be extra sure that it is eocd
  int cnt = 0;
  for (int i = amt - MIN(kZipCdirHdrMinSize, kZipCdir64LocatorSize); i; --i) {
    uint32_t magic = READ32LE(p->buf + i);
    if (magic == kZipCdir64LocatorMagic && i + kZipCdir64LocatorSize <= amt &&
        pread(fd, p->buf, kZipCdirHdrMinSize,
              ZIP_LOCATE64_OFFSET(p->buf + i)) == kZipCdirHdrMinSize &&
        READ32LE(p->buf) == kZipCdir64HdrMagic &&
        ZIP_CDIR64_RECORDS(p->buf) == ZIP_CDIR64_RECORDSONDISK(p->buf) &&
        ZIP_CDIR64_RECORDS(p->buf) && ZIP_CDIR64_SIZE(p->buf) <= INT_MAX) {
      cnt = ZIP_CDIR64_RECORDS(p->buf);
      off = ZIP_CDIR64_OFFSET(p->buf);
      amt = ZIP_CDIR64_SIZE(p->buf);
      break;
    }
    if (magic == kZipCdirHdrMagic && i + kZipCdirHdrMinSize <= amt &&
        ZIP_CDIR_RECORDS(p->buf + i) == ZIP_CDIR_RECORDSONDISK(p->buf + i) &&
        ZIP_CDIR_RECORDS(p->buf + i) && ZIP_CDIR_SIZE(p->buf + i) <= INT_MAX) {
      cnt = ZIP_CDIR_RECORDS(p->buf + i);
      off = ZIP_CDIR_OFFSET(p->buf + i);
      amt = ZIP_CDIR_SIZE(p->buf + i);
      break;
    }
  }
  if (cnt <= 0) {
    return kZipErrorEocdNotFound;
  }

  // we'll store the entire central directory in memory
  // in addition to a file name index that's bisectable
  void *memory;
  int cdirsize = amt;
  size_t indexsize = cnt * sizeof(int);
  size_t memorysize = indexsize + cdirsize;
  if (!(memory = _mapanon(memorysize))) {
    return kZipErrorMapFailed;
  }
  int *index = memory;
  uint8_t *cdir = (uint8_t *)memory + indexsize;

  // read the central directory
  if (pread(fd, cdir, cdirsize, off) != cdirsize) {
    return kZipErrorReadFailed;
  }

  // generate our file name index
  // smoothsort() isn't the fastest algorithm, but it guarantees
  // o(logn), won't smash the stack and doesn't depend on malloc
  int entry_index, entry_offset;
  for (entry_index = entry_offset = 0;
       entry_index < cnt && entry_offset + kZipCfileHdrMinSize <= cdirsize &&
       entry_offset + ZIP_CFILE_HDRSIZE(cdir + entry_offset) <= cdirsize;
       ++entry_index, entry_offset += ZIP_CFILE_HDRSIZE(cdir + entry_offset)) {
    index[entry_index] = entry_offset;
  }
  if (cnt != entry_index) {
    return kZipErrorZipCorrupt;
  }
  smoothsort_r(index, cnt, sizeof(int), __zipos_compare, cdir);

  // finally populate the global singleton
  __zipos.cnt = cnt;
  __zipos.cdir = cdir;
  __zipos.index = index;
  __zipos.dev = p->st.st_ino;
  __zipos.cdirsize = cdirsize;
  return kZipOk;
}

static dontinline int __zipos_setup(int fd) {
  // allocates 64kb on the stack as scratch memory
  // this should only be used from the main thread
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  struct ZiposPlanner p;
  CheckLargeStackAllocation(&p, sizeof(p));
#pragma GCC pop_options
  return __zipos_plan(fd, &p);
}

static void __zipos_init(void) {
  int status;
  if (getenv("COSMOPOLITAN_DISABLE_ZIPOS")) return;
  if (!(__zipos.progpath = getenv("COSMOPOLITAN_INIT_ZIPOS"))) {
    __zipos.progpath = GetProgramExecutableName();
  }
  int fd = open(__zipos.progpath, O_RDONLY);
  if (fd != -1) {
    if (!(status = __zipos_setup(fd))) {
      __zipos_wipe();
      pthread_atfork(__zipos_lock, __zipos_unlock, __zipos_wipe);
    }
    close(fd);
  } else {
    status = kZipErrorOpenFailed;
  }
  (void)status;
  STRACE("__zipos_init(%#s) → %d% m", __zipos.progpath, status);
}

/**
 * Returns pointer to zip central directory of current executable.
 * @asyncsignalsafe
 */
struct Zipos *__zipos_get(void) {
  cosmo_once(&__zipos.once, __zipos_init);
  return __zipos.cnt ? &__zipos : 0;
}
