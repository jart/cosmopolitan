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

#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/cosmo.h"
#include "libc/cosmotime.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/libgen.h"
#include "libc/intrin/bsr.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/serialize.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/str/tab.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/utime.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/time.h"
#include "libc/zip.h"
#include "net/http/escape.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/zlib/zlib.h"

static const char *const help = "\
Copyright 2025 Justine Alexandra Roberts Tunney - ISC license\n\
Cosmopolitan's multithreaded unzip command. Usage:\n\
fasterunzip [FLAGS...] ARCHIVE [ASSET...]\n\
  -j    remove directory components from extracted assets\n\
  -D    skip timestamp restoration on directories\n\
  -DD   skip timestamp restoration on dirs/files\n\
  -a    auto-convert line endings of text files\n\
  -K    preserve setuid/setgid/sticky bits\n\
  -aa   always auto-convert line endings\n\
  -f    freshen existing files on disk\n\
  -h    show this help information\n\
  -v    increase verbosity\n\
\n";

enum Newlines {
  NL_BINARY,
  NL_UNIX,
  NL_DOS,
};

struct Job {
  struct Job *next;
  const char *zpath;
  uint8_t *cfile;
};

struct Desire {
  char *name;
  bool found;
};

struct Desires {
  int n, c;
  struct Desire *p;
};

int zfd;
int gmtoff;
char *prog;
bool junkit;
bool freshen;
bool funbits;
uint8_t *cdir;
int verbosity;
int autolines;
int skiptimes;

struct Job *jobs;
struct Desires desires;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

[[noreturn]] void Die(const char *thing, const char *reason) {
  pthread_mutex_lock(&lock);
  fputs(thing, stderr);
  fputs(": fatal error: ", stderr);
  fputs(reason, stderr);
  fputs("\n", stderr);
  exit(1);
}

[[noreturn]] void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

[[noreturn]] void DieOom(void) {
  Die(prog, "out of memory");
}

void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n)))
    DieOom();
  return p;
}

char *StrDup(const char *s) {
  char *p;
  if (!(p = strdup(s)))
    DieOom();
  return p;
}

void *Calloc(size_t n, size_t z) {
  void *p;
  if (!(p = calloc(n, z)))
    DieOom();
  return p;
}

void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n)))
    DieOom();
  return p;
}

char *StrCat(const char *a, const char *b) {
  char *p;
  size_t n, m;
  n = strlen(a);
  m = strlen(b);
  p = Malloc(n + m + 1);
  if (n)
    memcpy(p, a, n);
  memcpy(p + n, b, m + 1);
  return p;
}

void AddDesire(char *name) {
  if (desires.n == desires.c) {
    desires.c += 1;
    desires.c += desires.c >> 1;
    desires.p = Realloc(desires.p, desires.c * sizeof(*desires.p));
  }
  desires.p[desires.n].name = name;
  desires.p[desires.n].found = false;
  desires.n += 1;
}

bool IsDesired(char *name) {
  if (!desires.n)
    return true;
  char *nam2 = StrCat(name, "/");
  for (int i = 0; i < desires.n; ++i) {
    if (!strcmp(name, desires.p[i].name) ||  //
        !strcmp(nam2, desires.p[i].name)) {
      desires.p[i].found = true;
      free(nam2);
      return true;
    }
  }
  free(nam2);
  return false;
}

int GetGmtOffset(int64_t t) {
  struct tm tm;
  localtime_r(&t, &tm);
  return tm.tm_gmtoff;
}

void CopyTimestamps(const char *path, const uint8_t *cfile) {
  struct timespec ts[2];
  GetZipCfileTimestamps(cfile, &ts[1], &ts[0], 0, gmtoff);
  if (!ts[0].tv_sec && !ts[0].tv_nsec)
    ts[0].tv_nsec = UTIME_OMIT;
  if (!ts[1].tv_sec && !ts[1].tv_nsec)
    ts[1].tv_nsec = UTIME_OMIT;
  if (utimensat(AT_FDCWD, path, ts, AT_SYMLINK_NOFOLLOW))
    DieSys(path);
}

bool IsNewerThan(const char *path, const uint8_t *cfile) {
  struct stat st;
  struct timespec mtim;
  GetZipCfileTimestamps(cfile, &mtim, 0, 0, gmtoff);
  if (fstatat(AT_FDCWD, path, &st, AT_SYMLINK_NOFOLLOW)) {
    if (errno == ENOENT)
      return true;
    DieSys(path);
  }
  return timespec_cmp(mtim, st.st_mtim) > 0;
}

void FixSlashes(char *s) {
  for (; *s; ++s)
    if (*s == '\\')
      *s = '/';
}

bool IsNormalPath(const char *p) {
  bool r;
  size_t n;
  char *s, *t;
  s = StrDup(p);
  n = strlen(s);
  if (n && s[n - 1] == '/')
    s[--n] = 0;
  t = Malloc(n + 1);
  r = __zipos_normpath(t, s, n + 1) <= n && !strcmp(s, t);
  free(t);
  free(s);
  return r;
}

char *GetZipName(const char *path, const uint8_t *cfile) {
  char *u8str;
  const char *name = ZIP_CFILE_NAME(cfile);
  int namesize = ZIP_CFILE_NAMESIZE(cfile);
  if (ZIP_CFILE_EXTERNALATTRIBUTES(cfile) & kZipGflagUtf8) {
    if (!isutf8(name, namesize))
      Die(path, "archive contains invalid utf8 asset name");
    u8str = Malloc(namesize + 1);
    memcpy(u8str, name, namesize);
    u8str[namesize] = 0;
    // we don't support the infozip flag for allowing control codes (-^)
    // because cosmopolitan openat / mkdirat wouldn't allow that anyway.
    char *safestr;
    if (!(safestr = VisualizeControlCodes(u8str, namesize, 0)))
      DieOom();
    free(u8str);
    u8str = safestr;
  } else {
    u8str = Malloc(namesize * 3 + 4);
    int j = 0;
    for (int i = 0; i < namesize; ++i) {
      uint32_t w = tpenc(kCp437[name[i] & 255]);
      WRITE32LE(u8str + j, w);
      j += bsr(w) >> 3;
      j += 1;
    }
    u8str[namesize] = 0;
    realloc_in_place(u8str, j + 1);
    namesize = j;
  }
  FixSlashes(u8str);
  if (!u8str[0] || u8str[0] == '/')
    Die(path, "archive contains bad asset name");
  if (!IsNormalPath(u8str))
    Die(path, "archive contains denormalized path");
  return u8str;
}

int ConvertLinesToUnix(int *t, uint8_t *inout, int size) {
  int i, j;
  for (i = j = 0; i < size; ++i) {
    switch (*t) {
      case 0:
        if (inout[i] == '\r') {
          inout[j++] = '\n';
          *t = 1;
        } else {
          inout[j++] = inout[i];
        }
        break;
      case 1:
        if (inout[i] == '\n') {
          *t = 0;
        } else if (inout[i] == '\r') {
          inout[j++] = '\n';
        } else {
          inout[j++] = inout[i];
          *t = 0;
        }
        break;
      default:
        abort();
    }
  }
  return j;
}

int ConvertLinesToDos(int *t, uint8_t *out, const uint8_t *in, int size) {
  int i, j;
  for (i = j = 0; i < size; ++i) {
    switch (*t) {
      case 0:
        if (in[i] == '\r') {
          out[j++] = '\r';
          out[j++] = '\n';
          *t = 1;
        } else if (in[i] == '\n') {
          out[j++] = '\r';
          out[j++] = '\n';
        } else {
          out[j++] = in[i];
        }
        break;
      case 1:
        if (in[i] == '\n') {
          *t = 0;
        } else if (in[i] == '\r') {
          out[j++] = '\r';
          out[j++] = '\n';
        } else {
          out[j++] = in[i];
          *t = 0;
        }
        break;
      default:
        abort();
    }
  }
  return j;
}

int64_t Decompress(int infd, int outfd, int64_t size, int64_t offset,
                   enum Newlines nl) {
  // special
  if (!size)
    return 0;

  // initialize auto raw deflate decoding
  z_stream strm = {0};
  int ret = inflateInit2(&strm, -MAX_WBITS);
  if (ret != Z_OK)
    return enomem();

  // allocate buffers
  const int read_chunk_size = 65536;
  const int output_chunk_size = 65536;
  uint8_t *read_chunk = malloc(read_chunk_size);
  uint8_t *output_chunk = malloc(output_chunk_size);
  uint8_t *winchunk = 0;
  if (!read_chunk || !output_chunk) {
  OnFailure:
    inflateEnd(&strm);
    free(output_chunk);
    free(read_chunk);
    free(winchunk);
    return -1;
  }
  if (nl == NL_DOS)
    if (!(winchunk = malloc(output_chunk_size * 2)))
      goto OnFailure;

  // main loop
  int rc;
  int state = 0;
  uint32_t crc = 0;
  int64_t bytes_read = 0;
  do {
    // read compressed
    if (!strm.avail_in && bytes_read < size) {
      int want = MIN(read_chunk_size, size - bytes_read);
      rc = pread(infd, read_chunk, want, offset + bytes_read);
      if (rc == -1)
        goto OnFailure;
      if (!rc) {
        errno = EIO;
        goto OnFailure;
      }
      strm.next_in = read_chunk;
      strm.avail_in = rc;
      bytes_read += rc;
    }

    // decompress data
    strm.next_out = output_chunk;
    strm.avail_out = output_chunk_size;
    ret = inflate(&strm, Z_NO_FLUSH);
    switch (ret) {
      case Z_NEED_DICT:
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
      case Z_BUF_ERROR:
        errno = EIO;
        goto OnFailure;
      default:
        break;
    }

    // check for corruption
    int n = output_chunk_size - strm.avail_out;
    crc = crc32(crc, output_chunk, n);

    // perform line ending conversion
    int outlength;
    uint8_t *outbytes;
    switch (nl) {
      case NL_BINARY:
        outlength = n;
        outbytes = output_chunk;
        break;
      case NL_UNIX:
        outlength = ConvertLinesToUnix(&state, output_chunk, n);
        outbytes = output_chunk;
        break;
      case NL_DOS:
        outlength = ConvertLinesToDos(&state, winchunk, output_chunk, n);
        outbytes = winchunk;
        break;
      default:
        abort();
    }

    // write uncompressed
    for (int i = 0; i < outlength; i += rc)
      if ((rc = write(outfd, outbytes + i, outlength - i)) <= 0)
        goto OnFailure;
  } while (ret != Z_STREAM_END && (strm.avail_in > 0 || bytes_read < size));

  // finish up
  free(winchunk);
  free(read_chunk);
  free(output_chunk);
  if (inflateEnd(&strm) != Z_OK)
    return eio();
  return crc;
}

void *Worker(void *arg) {
  for (;;) {
    struct Job *job;
    pthread_mutex_lock(&lock);
    if ((job = jobs))
      jobs = job->next;
    pthread_mutex_unlock(&lock);
    if (!job)
      return 0;

    // get asset name
    char *name = GetZipName(job->zpath, job->cfile);

    // strip directory components
    if (junkit && strchr(name, '/')) {
      char *base = StrDup(basename(name));
      free(name);
      name = base;
    }

    // check if we can skip extraction
    if (freshen && !IsNewerThan(name, job->cfile)) {
      free(name);
      free(job);
      continue;
    }

    // make directory components
    if (strchr(name, '/')) {
      char *dir = StrDup(name);
      dirname(dir);
      if (makedirs(dir, 0755))
        DieSys(dir);
      free(dir);
    }

    // log asset name
    if (verbosity) {
      flockfile(stderr);
      fputs_unlocked(name, stderr);
      fputs_unlocked("\n", stderr);
      funlockfile(stderr);
    }

    // handle asset directory components
    if (strchr(name, '/')) {
      if (junkit) {
        char *base = StrDup(basename(name));
        free(name);
        name = base;
      } else {
        char *dir = StrDup(name);
        dirname(dir);
        if (makedirs(dir, 0755))
          DieSys(dir);
        free(dir);
      }
    }

    // determine how to transform content
    enum Newlines nl;
    if (autolines > 1 ||
        (autolines &&
         (ZIP_CFILE_INTERNALATTRIBUTES(job->cfile) & kZipIattrText))) {
      if (IsWindows()) {
        nl = NL_DOS;
      } else {
        nl = NL_UNIX;
      }
    } else {
      nl = NL_BINARY;
    }

    // get information from central directory entry
    int mode;
    int64_t lfile_offset;
    int compression_method;
    int64_t compressed_size;
    int64_t uncompressed_size;
    mode = GetZipCfileMode(job->cfile);
    compression_method = ZIP_CFILE_COMPRESSIONMETHOD(job->cfile);
    if (compression_method != kZipCompressionNone &&
        compression_method != kZipCompressionDeflate)
      Die(job->zpath, "unsupported compression method");
    if ((compressed_size = GetZipCfileCompressedSize(job->cfile)) < 0)
      Die(job->zpath, "bad cfile compressed size");
    if ((uncompressed_size = GetZipCfileUncompressedSize(job->cfile)) < 0)
      Die(job->zpath, "bad cfile uncompressed size");
    if ((lfile_offset = GetZipCfileOffset(job->cfile)) < 0)
      Die(job->zpath, "bad lfile offset");
    if (compressed_size != uncompressed_size &&
        compression_method == kZipCompressionNone)
      Die(job->zpath, "compressed and uncompressed sizes not same");

    // read local file entry header
    uint8_t *lfile = Malloc(kZipLfileHdrMinSize);
    if (pread(zfd, lfile, kZipLfileHdrMinSize, lfile_offset) !=
        kZipLfileHdrMinSize)
      DieSys(job->zpath);
    lfile = Realloc(lfile, ZIP_LFILE_HDRSIZE(lfile));
    if (pread(zfd, lfile + kZipLfileHdrMinSize,
              ZIP_LFILE_HDRSIZE(lfile) - kZipLfileHdrMinSize,
              lfile_offset + kZipLfileHdrMinSize) !=
        ZIP_LFILE_HDRSIZE(lfile) - kZipLfileHdrMinSize)
      DieSys(job->zpath);

    // create file on filesystem
    int rc;
    int64_t crc = 0;
    int64_t data_offset = lfile_offset + ZIP_LFILE_HDRSIZE(lfile);
    if (S_ISLNK(mode)) {
      // create symbolic link
      if (compression_method != kZipCompressionNone)
        Die(job->zpath, "compressed symlinks not supported");
      if (uncompressed_size > PATH_MAX)
        Die(job->zpath, "symlink too long");
      char *target = Malloc(uncompressed_size + 1);
      if ((rc = pread(zfd, target, uncompressed_size, data_offset)) == -1)
        DieSys(job->zpath);
      if (rc != uncompressed_size)
        Die(job->zpath, "symlink overlaps end of file");
      crc = crc32(crc, (const uint8_t *)target, uncompressed_size);
      target[uncompressed_size] = 0;
      if (freshen)
        if (unlink(name))
          if (errno != ENOENT)
            DieSys(name);
      if (symlink(target, name))
        DieSys(name);
      free(target);
    } else {
      // create regular file
      int fd;
      int oflags = O_CREAT | O_WRONLY | O_TRUNC | O_NOFOLLOW | O_NOCTTY |
                   (freshen ? 0 : O_EXCL);
      if ((fd = open(name, oflags, mode & 0777)) == -1)
        DieSys(name);
      if (!compression_method) {
        // uncompressed data - read and write in chunks
        int state = 0;
        int wc, chunk_size = 65536;
        uint8_t *chunk = Malloc(chunk_size);
        uint8_t *chunk2 = nl == NL_DOS ? Malloc(chunk_size * 2) : 0;
        for (int64_t i = 0; i < compressed_size; i += rc) {
          int want = MIN(chunk_size, compressed_size - i);
          if ((rc = pread(zfd, chunk, want, data_offset + i)) == -1)
            DieSys(job->zpath);
          if (!rc)
            Die(job->zpath, "zip asset overlaps end of archive");
          crc = crc32(crc, chunk, rc);
          int outlength;
          uint8_t *outbytes;
          switch (nl) {
            case NL_BINARY:
              outlength = rc;
              outbytes = chunk;
              break;
            case NL_UNIX:
              outlength = ConvertLinesToUnix(&state, chunk, rc);
              outbytes = chunk;
              break;
            case NL_DOS:
              outlength = ConvertLinesToDos(&state, chunk2, chunk, rc);
              outbytes = chunk2;
              break;
            default:
              abort();
          }
          for (int j = 0; j < outlength; j += wc)
            if ((wc = write(fd, outbytes + j, outlength - j)) <= 0)
              DieSys(name);
        }
        free(chunk2);
        free(chunk);
      } else {
        // compressed data - use streaming decompression
        if ((crc = Decompress(zfd, fd, compressed_size, data_offset, nl)) == -1)
          DieSys(name);
      }
      if (funbits)
        if (mode & 07000)
          if (fchmod(fd, mode & 07777))
            DieSys(name);
      if (close(fd))
        DieSys(name);
    }

    // set access and modified times
    if (skiptimes < 2)
      CopyTimestamps(name, job->cfile);

    // verify data wasn't corrupted
    if (ZIP_CFILE_CRC32(job->cfile) && crc != ZIP_CFILE_CRC32(job->cfile))
      Die(job->zpath, "archive had asset with bad crc32 code");

    // clean up
    free(lfile);
    free(name);
    free(job);
  }
}

void Unzip(const char *zpath) {
  // open executable
  int64_t zsize;
  if ((zfd = open(zpath, O_RDONLY)) == -1)
    DieSys(zpath);
  if ((zsize = lseek(zfd, 0, SEEK_END)) == -1)
    DieSys(zpath);

  // read last 64kb of file
  int64_t amt, off;
  if (zsize <= 65536) {
    off = 0;
    amt = zsize;
  } else {
    off = zsize - 65536;
    amt = zsize - off;
  }
  char last64[65536];
  if (pread(zfd, last64, amt, off) != amt)
    DieSys(zpath);

  // search backwards for end-of-central-directory (eocd) record
  // the eocd (cdir) says where the central directory (cfile) array is located
  // we consistency check some legacy fields, to be extra sure that it is eocd
  int64_t cnt = -1;
  for (int i = amt - MIN(kZipCdirHdrMinSize, kZipCdir64LocatorSize); i >= 0;
       --i) {
    uint32_t magic = ZIP_READ32(last64 + i);
    if (magic == kZipCdir64LocatorMagic && i + kZipCdir64LocatorSize <= amt &&
        pread(zfd, last64, kZipCdir64HdrMinSize,
              ZIP_LOCATE64_OFFSET(last64 + i)) == (long)kZipCdir64HdrMinSize &&
        ZIP_READ32(last64) == kZipCdir64HdrMagic &&
        ZIP_CDIR64_VERSIONNEED(last64) >= kZipEra2001 &&
        ZIP_CDIR64_RECORDS(last64) == ZIP_CDIR64_RECORDSONDISK(last64)) {
      if (ZIP_CDIR64_VERSIONNEED(last64) > kZipEra2001)
        Die(zpath, "zip archive requires unsupported features");
      cnt = ZIP_CDIR64_RECORDS(last64);
      off = ZIP_CDIR64_OFFSET(last64);
      amt = ZIP_CDIR64_SIZE(last64);
      break;
    }
    if (magic == kZipCdirHdrMagic && i + kZipCdirHdrMinSize <= amt &&
        ZIP_CDIR_RECORDS(last64 + i) == ZIP_CDIR_RECORDSONDISK(last64 + i) &&
        ZIP_CDIR_OFFSET(last64 + i) != 0xffffffffu) {
      cnt = ZIP_CDIR_RECORDS(last64 + i);
      off = ZIP_CDIR_OFFSET(last64 + i);
      amt = ZIP_CDIR_SIZE(last64 + i);
      break;
    }
  }
  if (cnt < 0 || amt < 0)
    Die(zpath, "not a zip archive");

  // read cfiles
  ssize_t rc;
  cdir = Malloc(amt);
  for (int64_t i = 0; i < amt; i += rc) {
    if ((rc = pread(zfd, cdir + i, amt - i, off + i)) == -1)
      Die(zpath, "failed to read zip central directory");
    if (!rc)
      Die(zpath, "zip central directory overlaps eof");
  }

  // create jobs
  int64_t got, entry_offset, enqueued;
  for (entry_offset = enqueued = got = 0;
       entry_offset + kZipCfileHdrMinSize <= amt && got < cnt &&
       entry_offset + ZIP_CFILE_HDRSIZE(cdir + entry_offset) <= amt;
       entry_offset += ZIP_CFILE_HDRSIZE(cdir + entry_offset), ++got) {
    if (ZIP_CFILE_MAGIC(cdir + entry_offset) != kZipCfileHdrMagic)
      Die(zpath, "corrupted zip central directory entry magic");
    char *name = GetZipName(zpath, cdir + entry_offset);
    if (!(endswith(name, "/") ||
          S_ISDIR(GetZipCfileMode(cdir + entry_offset)))) {
      if (IsDesired(name)) {
        if (ZIP_CFILE_VERSIONNEED(last64) > kZipEra2001)
          Die(zpath, "zip artifact requires unsupported features");
        struct Job *job = Malloc(sizeof(struct Job));
        job->cfile = cdir + entry_offset;
        job->zpath = zpath;
        job->next = jobs;
        jobs = job;
        ++enqueued;
      }
    }
    free(name);
  }

  // launch threads
  //
  // on a linux system with a 96 core threadripper and an nvme m.2
  // stick, eight was the optimal number of threads. anything more
  // caused too much contention inside the kernel to do much help.
  int err;
  int optimal = 8;
  int cpu_count = cosmo_cpu_count();
  int thread_count = MIN(enqueued, MAX(MIN(cpu_count, optimal), 0));
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 65536 - getpagesize());
  pthread_attr_setguardsize(&attr, getpagesize());
  bool *te = Calloc(thread_count, sizeof(bool));
  pthread_t *th = Calloc(thread_count, sizeof(pthread_t));
  for (int i = 0; i < thread_count; ++i)
    if (!pthread_create(&th[i], &attr, Worker, 0))
      te[i] = true;
  Worker(0);
  for (int i = 0; i < thread_count; ++i)
    if (te[i])
      if ((err = pthread_join(th[i], 0)))
        Die("pthread_join", strerror(err));
  pthread_attr_destroy(&attr);
  free(th);
  free(te);

  // adjust directory mode and timestamps
  if (!junkit) {
    for (entry_offset = got = 0;
         entry_offset + kZipCfileHdrMinSize <= amt && got < cnt &&
         entry_offset + ZIP_CFILE_HDRSIZE(cdir + entry_offset) <= amt;
         entry_offset += ZIP_CFILE_HDRSIZE(cdir + entry_offset), ++got) {
      char *name = GetZipName(zpath, cdir + entry_offset);
      if (endswith(name, "/") ||
          S_ISDIR(GetZipCfileMode(cdir + entry_offset))) {
        if (IsDesired(name) &&
            !(freshen && !IsNewerThan(name, cdir + entry_offset))) {
          int mode = GetZipCfileMode(cdir + entry_offset);
          if (makedirs(name, 0755))
            DieSys(name);
          if (chmod(name, mode & (funbits ? 07777 : 0777)))
            DieSys(name);
          if (skiptimes < 1)
            CopyTimestamps(name, cdir + entry_offset);
        }
      }
      free(name);
    }
  }

  // cleanup
  close(zfd);
  free(cdir);
}

int main(int argc, char *argv[]) {

  // get prog
  prog = argv[0];
  if (!prog)
    prog = "fasterunzip";

  // get flags
  int opt;
  while ((opt = getopt(argc, argv, "afhjvDK")) != -1) {
    switch (opt) {
      case 'a':
        ++autolines;
        break;
      case 'j':
        junkit = true;
        break;
      case 'K':
        funbits = true;
        break;
      case 'f':
        freshen = true;
        break;
      case 'D':
        ++skiptimes;
        break;
      case 'v':
        ++verbosity;
        break;
      case 'h':
        fputs(help, stdout);
        exit(0);
      default:
        fputs(help, stderr);
        exit(1);
    }
  }

  // get archive argument
  if (optind == argc)
    Die(prog, "missing archive argument");
  char *archive = argv[optind++];

  // get optional list of desired names to extract
  for (int i = optind; i < argc; ++i)
    AddDesire(argv[i]);

  // unzip archive
  Unzip(archive);

  // make sure desires were satisfied
  if (desires.n)
    for (int i = 0; i < desires.n; ++i)
      if (!desires.p[i].found)
        Die(desires.p[i].name, "asset not found in zip archive");
}
