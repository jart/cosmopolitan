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

#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.macros.h"
#include "libc/calls/weirdtypes.h"
#include "libc/cosmo.h"
#include "libc/cosmotime.h"
#include "libc/ctype.h"
#include "libc/dos.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/libgen.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/bsr.h"
#include "libc/limits.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/ftw.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/temp.h"
#include "libc/thread/semaphore.h"
#include "libc/thread/thread.h"
#include "libc/time.h"
#include "libc/zip.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/zlib/zlib.h"

/**
 * @fileoverview Fast ZIP Archive Creator
 *
 * This is like the standard `zip` command except it's multithreaded.
 * Justine made this program because `zip -ry9 ../cosmocc.zip .` takes
 * 198 seconds to run whereas `fasterzip -ry9 ../ccc.zip .` takes two.
 * We're talking about a 422mb .zip archive here. It's also interesting
 * that our command only needs 18mb of peak RSS memory, whereas Info-ZIP
 * consumes 198 megabytes. Lastly, our archive comes out 500kb smaller.
 *
 * Alternatively, I could have used `zip -ry4` which takes 16 seconds,
 * or `zip -ry1` which 11 seconds. The issue is then the cosmocc archive
 * sizes would be 440mb and 469mb. However it's important to use maximum
 * compression when generating permanent release artifacts, that will
 * need to be stored on multiple servers forever, and downloaded by
 * thousands of people.
 *
 * Another benefit of this tool is it produces deterministic output.
 * When running in `-r` (recursive) mode, the standard `zip` command
 * orders files in the archive based on the undefined order readdir()
 * returns names from the operating system. That's bad if your build
 * system needs to create zip files, since you'll get different results
 * on different OSes. What this tool does instead, is it'll order names
 * the same as the argument list by default. The exception is when the
 * `-r` or `-S` flag is passed. In that case, appended zip names are
 * sorted. Furthermore, the modified time is always set to 2025-03-22
 * and chmod bits are normalized to only include the execute bit, unless
 * the `-N` flag is passed for nondeterministic mode.
 *
 * It's worth noting that this tool treats the output archive as an
 * append-only database. For example, the following commands:
 *
 *     fasterzip archive.zip x.txt
 *     fasterzip archive.zip y.txt
 *     fasterzip archive.zip x.txt
 *
 * Will produce a zip file that officially has two files, where x.txt
 * contains the most recent content. However the binary content in the
 * archive.zip file still contains the data for the previous revision of
 * x.txt that was added first. The previous version of the zip central
 * directory is also preserved. However it's currently not possible to
 * access that information without writing a custom tool.
 *
 * The benefit to this approach is that new content can be added much
 * quicker than the traditional `zip` command, which rebuilds the entire
 * archive every time entries are inserted or removed. This is useful
 * when using .zip files for huge content like language models weights.
 * However the drawback is it makes the .zip file bigger, unless the
 * archive is rebuilt from scratch.
 *
 * Another powerful feature this tool offers is the ability to align
 * file content. With large files like LLM weights, it is sometimes
 * desirable to be able to use mmap() to load them into memory. To make
 * it possible, the `-0 -a 65536` flags may be passed to store without
 * compression and guarantee alignment on any system page size.
 */

static const char *const help = "\
Copyright 2025 Justine Alexandra Roberts Tunney - ISC license\n\
FasterZIP 1.0 (2025-03-21). Usage:\n\
fasterzip [FLAGS...] [-a alignment] [-n suffixes] [PATH...]\n\
  The default action is to add or replace zipfile entries from list.\n\
  -r   recurse into directories     -j   junk (don't record) directory names\n\
  -0   store only                   -q   quiet operation\n\
  -1   compress faster              -9   compress better\n\
  -q   quiet operation              -v   verbose operation\n\
  -@   read names from stdin        -D   do not add directory entries\n\
  -N   nondeterministic mode        -h   show this helpful information\n\
  -S   sort new names in archive    -p   parallelism number of threads\n\
  -a   align content on two power   -n   don't compress these suffixes\n\
  -y   store symbolic links as the link instead of the referenced file\n\
  -x   exclude input file paths that match glob pattern (repeatable)\n\
\n";

#define CHUNK 16384
#define HUNK  4194304
#define TIME  1742634000

enum {
  BINARY = -1,
  ASCII = 0,
  UTF8 = 1,
};

struct Strings {
  size_t n;
  const char **p;
};

struct File {
  char *path;
  char *dpath;
  char *name;
  uint32_t crc;
  uint32_t mode;
  uint16_t mtime;
  uint16_t mdate;
  size_t size;
  struct timespec modified;
  pthread_mutex_t mu;
  pthread_cond_t cv;
  bool delete;
  bool ready;
  int comp;
  int cat;
};

struct Hunk {
  pthread_t th;
  int fd;
  char *path;
  char *dpath;
  size_t off;
  size_t size;
};

static struct {
  size_t i;
  size_t n;
  struct File **p;
  pthread_mutex_t mu;
} g_files;

static char *prog;
static sem_t g_sem;
static int flag_junk;
static int flag_level = Z_DEFAULT_COMPRESSION;
static int flag_verbose;
static int flag_threads;
static bool flag_symbolic;
static bool flag_sortnames;
static bool flag_recursive;
static bool flag_stdinargs;
static int flag_alignment = 1;
static bool flag_nodirectories;
static bool flag_nondeterministic;
static struct Strings flag_excludes;
static const char *flag_nocompsuffixes;

[[noreturn]] static void Die(const char *thing, const char *reason) {
  static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&mu);
  tinyprint(2, thing, ": fatal error: ", reason, "\n", NULL);
  exit(1);
}

[[noreturn]] static void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

[[noreturn]] static void DieOom(void) {
  Die(prog, "out of memory");
}

static void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n)))
    DieOom();
  return p;
}

static char *StrDup(const char *s) {
  char *p;
  if (!(p = strdup(s)))
    DieOom();
  return p;
}

static void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n)))
    DieOom();
  return p;
}

static void AppendString(struct Strings *l, const char *s) {
  l->p = Realloc(l->p, (l->n + 1) * sizeof(*l->p));
  l->p[l->n++] = s;
}

static int CompareZipNames(const void *a, const void *b) {
  const char *x = (*(const struct File **)a)->name;
  const char *y = (*(const struct File **)b)->name;
  for (;;) {
    int xn = strchrnul(x, '/') - x;
    int yn = strchrnul(y, '/') - y;
    int n = MIN(xn, yn);
    int c;
    if ((c = !!x[xn] - !!y[yn]))
      return c;
    if (n)
      if ((c = memcmp(x, y, n)))
        return c;
    if ((c = xn - yn))
      return c;
    if (!x[xn])
      return 0;
    x += xn + 1;
    y += yn + 1;
  }
}

static void GetDosLocalTime(int64_t utcunixts, uint16_t *out_time,
                            uint16_t *out_date) {
  struct tm tm;
  localtime_r(&utcunixts, &tm);
  *out_time = DOS_TIME(tm.tm_hour, tm.tm_min, tm.tm_sec);
  *out_date = DOS_DATE(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

static int FixMode(int mode) {
  if (flag_nondeterministic)
    return mode;
  int res = mode & S_IFMT;
  if (mode & 0111)
    res |= 0111;
  return res | 0644;
}

static bool IsGlobMatch(const char *p, const char *s) {
  while (*p) {
    switch (*p) {
      case '?':
        if (!*s)
          return false;
        p++;
        s++;
        break;
      case '*':
        if (IsGlobMatch(p + 1, s))
          return true;
        if (*s && IsGlobMatch(p, s + 1))
          return true;
        return false;
      default:
        if (!*s || *p != *s)
          return false;
        p++;
        s++;
        break;
    }
  }
  return !*s;
}

static bool ShouldExcludePath(const char *path) {
  for (size_t i = 0; i < flag_excludes.n; ++i)
    if (IsGlobMatch(flag_excludes.p[i], path))
      return true;
  return false;
}

static size_t Normalize(char *d, const char *s, size_t n) {
  char *p, *e;
  for (p = d, e = d + n; p < e && *s; ++s) {
    if ((p == d || p[-1] == '/') && *s == '/') {
      // matched "^/" or "//"
    } else if ((p == d || p[-1] == '/') &&  //
               s[0] == '.' &&               //
               (!s[1] || s[1] == '/')) {
      // matched "/./" or "^.$" or "^./" or "/.$"
    } else if ((p == d || p[-1] == '/') &&  //
               s[0] == '.' &&               //
               s[1] == '.' &&               //
               (!s[2] || s[2] == '/')) {
      // matched "/../" or "^..$" or "^../" or "/..$"
      while (p > d && p[-1] == '/')
        --p;
      while (p > d && p[-1] != '/')
        --p;
    } else {
      *p++ = *s;
    }
  }
  // if we didn't overflow
  if (p < e) {
    *p = '\0';
  } else {
    // force nul-terminator to exist if possible
    if (p > d) {
      p[-1] = '\0';
    }
  }
  return p - d;
}

static bool StringListContains(const char *list, const char *thing) {
  size_t wantlen = strlen(thing);
  if (wantlen) {
    for (;;) {
      while (isspace(*list))
        ++list;
      const char *end = list + strcspn(list, ":;,");
      size_t len = end - list;
      if (len == wantlen && !memcmp(list, thing, len))
        return true;
      if (!*end)
        break;
      list = end + 1;
    }
  }
  return false;
}

static bool ShouldCompressName(const char *name) {
  const char *ext;
  if (!flag_nocompsuffixes)
    return true;
  if ((ext = strrchr(name, '.')) ||  //
      (ext = strrchr(name, '/'))) {
    ++ext;
  } else {
    ext = name;
  }
  return !StringListContains(flag_nocompsuffixes, ext);
}

static void AddFile(const char *path, const struct stat *st) {
  if (flag_nodirectories && S_ISDIR(st->st_mode))
    return;
  if (ShouldExcludePath(path))
    return;
  struct File *file = Malloc(sizeof(struct File));
  bzero(file, sizeof(*file));
  size_t i = g_files.n++;
  g_files.p = Realloc(g_files.p, g_files.n * sizeof(*g_files.p));
  g_files.p[i] = file;
  pthread_cond_init(&file->cv, 0);
  pthread_mutex_init(&file->mu, 0);
  file->mode = st->st_mode;
  if (!S_ISDIR(file->mode))
    file->size = st->st_size;
  if (!S_ISDIR(file->mode) &&  //
      !S_ISREG(file->mode) &&  //
      !S_ISLNK(file->mode))
    Die(path, "not a regular file or directory");
  if (flag_nondeterministic) {
    file->modified = st->st_mtim;
  } else {
    file->modified = timespec_fromseconds(TIME);
  }
  GetDosLocalTime(file->modified.tv_sec, &file->mtime, &file->mdate);
  file->path = StrDup(path);
  if (flag_junk) {
    file->name = basename(file->path);
  } else {
    size_t n = strlen(path) + 2;
    file->name = Malloc(n);
    n = Normalize(file->name, file->path, n - 1);
    while (file->name[n - 1] == '/')
      --n;
    if (S_ISDIR(file->mode))
      file->name[n++] = '/';
    file->name[n] = 0;
  }
  file->dpath = file->path;
  if (!flag_level || !file->size || S_ISLNK(file->mode) ||
      S_ISDIR(file->mode) || !ShouldCompressName(file->name)) {
    file->comp = kZipCompressionNone;
  } else {
    file->comp = kZipCompressionDeflate;
  }
  if (strlen(file->name) > 65535)
    Die(file->name, "name too long to be stored in zip archive");
  if (!strcmp(file->name, "/"))
    g_files.n--;  // avoid `zip -r .` adding entry for `.`
}

static int VisitFile(const char *path,       //
                     const struct stat *st,  //
                     int typeflag,           //
                     struct FTW *ftwbuf) {   //
  AddFile(path, st);
  return 0;
}

typedef struct {
  char *name;
  int fd;
  unsigned left;
  unsigned char *next;
  unsigned char *buf;
} bin;

static void bclose(bin *in) {
  if (in != NULL) {
    if (in->fd != -1)
      close(in->fd);
    if (in->buf != NULL)
      free(in->buf);
    free(in);
  }
}

static bin *bopen(char *name) {
  bin *in;
  in = Malloc(sizeof(bin));
  in->buf = Malloc(CHUNK);
  in->fd = open(name, O_RDONLY | O_NOCTTY, 0);
  if (in->fd == -1)
    DieSys(name);
  in->left = 0;
  in->next = in->buf;
  in->name = name;
  return in;
}

static int bload(bin *in) {
  long len;
  if (in == NULL)
    return -1;
  if (in->left != 0)
    return 0;
  in->next = in->buf;
  do {
    len = (long)read(in->fd, in->buf + in->left, CHUNK - in->left);
    if (len < 0)
      return -1;
    in->left += (unsigned)len;
  } while (len != 0 && in->left < CHUNK);
  return len == 0 ? 1 : 0;
}

static int bail(char *why1, char *why2) {
  tinyprint(2, "error: ", why1, why2, ", output incomplete\n", NULL);
  exit(1);
  return 0;
}

#define bget(in)                          \
  (in->left ? 0 : bload(in),              \
   in->left ? (in->left--, *(in->next)++) \
            : bail("unexpected end of file on ", in->name))

static unsigned long bget4(bin *in) {
  unsigned long val;
  val = bget(in);
  val += (unsigned long)(bget(in)) << 8;
  val += (unsigned long)(bget(in)) << 16;
  val += (unsigned long)(bget(in)) << 24;
  return val;
}

static void bskip(bin *in, unsigned skip) {
  if (in == NULL)
    return;
  if (skip <= in->left) {
    in->left -= skip;
    in->next += skip;
    return;
  }
  skip -= in->left;
  in->left = 0;
  if (skip > CHUNK) {
    unsigned left;
    left = skip & (CHUNK - 1);
    if (left == 0) {
      lseek(in->fd, skip - 1, SEEK_CUR);
      if (read(in->fd, in->buf, 1) != 1)
        bail("unexpected end of file on ", in->name);
      return;
    }
    lseek(in->fd, skip - left, SEEK_CUR);
    skip = left;
  }
  bload(in);
  if (skip > in->left)
    bail("unexpected end of file on ", in->name);
  in->left -= skip;
  in->next += skip;
}

static void gzhead(bin *in) {
  int flags;
  if (bget(in) != 0x1f || bget(in) != 0x8b || bget(in) != 8)
    bail(in->name, " is not a valid gzip file");
  flags = bget(in);
  if ((flags & 0xe0) != 0)
    bail("unknown reserved bits set in ", in->name);
  bskip(in, 6);
  if (flags & 4) {
    unsigned len;
    len = bget(in);
    len += (unsigned)(bget(in)) << 8;
    bskip(in, len);
  }
  if (flags & 8)
    while (bget(in) != 0)
      ;
  if (flags & 16)
    while (bget(in) != 0)
      ;
  if (flags & 2)
    bskip(in, 2);
}

static void put4(unsigned long val, FILE *out) {
  putc(val & 0xff, out);
  putc((val >> 8) & 0xff, out);
  putc((val >> 16) & 0xff, out);
  putc((val >> 24) & 0xff, out);
}

static void zpull(z_streamp strm, bin *in) {
  if (in->left == 0)
    bload(in);
  if (in->left == 0)
    bail("unexpected end of file on ", in->name);
  strm->avail_in = in->left;
  strm->next_in = in->next;
}

static void gzcopy(char *name, int clr, uint32_t *crc, size_t *tot, FILE *out) {
  int ret;
  int pos;
  int last;
  bin *in;
  unsigned char *start;
  unsigned char *junk;
  int64_t len;
  z_stream strm;
  in = bopen(name);
  gzhead(in);
  junk = Malloc(CHUNK);
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  ret = inflateInit2(&strm, -15);
  if (ret != Z_OK)
    DieOom();
  len = 0;
  zpull(&strm, in);
  start = in->next;
  last = start[0] & 1;
  if (last && clr)
    start[0] &= ~1;
  strm.avail_out = 0;
  for (;;) {
    if (strm.avail_in == 0 && strm.avail_out != 0) {
      fwrite(start, 1, strm.next_in - start, out);
      start = in->buf;
      in->left = 0;
      zpull(&strm, in);
    }
    strm.avail_out = CHUNK;
    strm.next_out = junk;
    ret = inflate(&strm, Z_BLOCK);
    switch (ret) {
      case Z_MEM_ERROR:
        DieOom();
      case Z_DATA_ERROR:
        bail("invalid compressed data in ", in->name);
    }
    len += CHUNK - strm.avail_out;
    if (strm.data_type & 128) {
      if (last)
        break;
      pos = strm.data_type & 7;
      if (pos != 0) {
        pos = 0x100 >> pos;
        last = strm.next_in[-1] & pos;
        if (last && clr)
          in->buf[strm.next_in - in->buf - 1] &= ~pos;
      } else {
        if (strm.avail_in == 0) {
          fwrite(start, 1, strm.next_in - start, out);
          start = in->buf;
          in->left = 0;
          zpull(&strm, in);
        }
        last = strm.next_in[0] & 1;
        if (last && clr)
          in->buf[strm.next_in - in->buf] &= ~1;
      }
    }
  }
  in->left = strm.avail_in;
  in->next = in->buf + (strm.next_in - in->buf);
  pos = strm.data_type & 7;
  fwrite(start, 1, in->next - start - 1, out);
  last = in->next[-1];
  if (pos == 0 || !clr) {
    putc(last, out);
  } else {
    last &= ((0x100 >> pos) - 1);
    if (pos & 1) {
      putc(last, out);
      if (pos == 1)
        putc(0, out);
      fwrite("\0\0\xff\xff", 1, 4, out);
    } else {
      switch (pos) {
        case 6:
          putc(last | 8, out);
          last = 0;
        case 4:
          putc(last | 0x20, out);
          last = 0;
        case 2:
          putc(last | 0x80, out);
          putc(0, out);
      }
    }
  }
  *crc = crc32_combine(*crc, bget4(in), len);
  *tot += (unsigned long)len;
  inflateEnd(&strm);
  free(junk);
  bclose(in);
}

static void NewDeflate(z_stream *zs, int windowbits) {
  zs->zfree = 0;
  zs->zalloc = 0;
  zs->opaque = 0;
  switch (deflateInit2(zs, flag_level, Z_DEFLATED, windowbits,  //
                       DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY)) {
    case Z_OK:
      break;
    case Z_MEM_ERROR:
      DieOom();
    default:
      npassert(!"deflateInit2() called with invalid parameters");
  }
}

static void RunDeflate(z_stream *zs, bool last) {
  switch (deflate(zs, last ? Z_FINISH : Z_NO_FLUSH)) {
    case Z_MEM_ERROR:
      DieOom();
    case Z_STREAM_ERROR:
      npassert(!"deflate() stream error");
    default:
      break;
  }
}

static char *StrCat(const char *a, const char *b) {
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

static char *MakeTempPathTemplate(void) {
  return StrCat(__get_tmpdir(), "fasterzip.XXXXXX");
}

static void HunkerImpl(struct Hunk *hunk) {
  uint8_t *iobuf = Malloc(CHUNK);
  uint8_t *cdbuf = Malloc(CHUNK);

  // open output file
  int dfd;
  hunk->dpath = MakeTempPathTemplate();
  if ((dfd = mkstemp(hunk->dpath)) == -1)
    DieSys(hunk->dpath);

  // initialize zlib in gzip mode
  z_stream zs;
  NewDeflate(&zs, MAX_WBITS + 16);

  // copy hunk
  size_t need;
  for (size_t i = hunk->off; i < hunk->off + hunk->size; i += need) {
    // read chunk
    need = MIN(CHUNK, hunk->off + hunk->size - i);
    ssize_t rc = pread(hunk->fd, iobuf, need, i);
    if (rc != need)
      DieSys(hunk->path);
    zs.avail_in = need;
    zs.next_in = iobuf;
    do {
      zs.next_out = cdbuf;
      zs.avail_out = CHUNK;
      RunDeflate(&zs, i + need >= hunk->off + hunk->size);
      ssize_t have = CHUNK - zs.avail_out;
      if (write(dfd, cdbuf, have) != have)
        DieSys(hunk->dpath);
    } while (!zs.avail_out);
  }

  // cleanup
  unassert(deflateEnd(&zs) == Z_OK);
  if (close(dfd))
    DieSys(hunk->dpath);
  free(cdbuf);
  free(iobuf);
}

static void *Hunker(void *arg) {
  struct Hunk *hunk = arg;
  sem_wait(&g_sem);
  HunkerImpl(hunk);
  sem_post(&g_sem);
  return 0;
}

static int ClassifyData(const uint8_t *p, size_t n, int cat, int *conts) {
  if (cat != BINARY) {
    for (size_t i = 0; i < n; ++i) {
      int c = p[i];
      if (c < 4)
        return BINARY;
      if (!*conts) {
        if (c < 0200)
          continue;
        if (c < 0300)
          return BINARY;
        if (c > 0364)
          return BINARY;
        cat = UTF8;
        *conts = __builtin_clz(~c & 255) - 25;
      } else {
        if ((c & 0300) != 0200)
          return BINARY;
        --*conts;
      }
    }
  }
  return cat;
}

static void *Kompressor(void *arg) {
  uint8_t *iobuf = Malloc(CHUNK);
  uint8_t *cdbuf = Malloc(CHUNK);
  for (;;) {

    // get file
    struct File *file = 0;
    pthread_mutex_lock(&g_files.mu);
    if (g_files.i < g_files.n)
      file = g_files.p[g_files.i++];
    pthread_mutex_unlock(&g_files.mu);
    if (!file)
      break;

    // check for huge file
    if (S_ISREG(file->mode) && file->size > HUNK &&
        file->comp != kZipCompressionNone) {

      // open input
      int fd;
      if ((fd = open(file->path, O_RDONLY | O_NOCTTY)) == -1)
        DieSys(file->path);

      // spawn thread to compress each 4mb separately
      int err;
      size_t count = (file->size + HUNK - 1) / HUNK;
      struct Hunk *hunks = Malloc(count * sizeof(struct Hunk));
      for (size_t i = 0; i < count; ++i) {
        hunks[i].fd = fd;
        hunks[i].path = file->path;
        hunks[i].off = i * HUNK;
        hunks[i].size = MIN(HUNK, file->size - i * HUNK);
        unassert(hunks[i].off + hunks[i].size <= file->size);
        if ((err = pthread_create(&hunks[i].th, 0, Hunker, hunks + i)))
          Die("pthread_create", strerror(err));
      }
      for (size_t i = 0; i < count; ++i)
        if ((err = pthread_join(hunks[i].th, 0)))
          Die("pthread_join", strerror(err));

      // close input
      if (close(fd))
        DieSys(file->path);

      // open output file
      int dfd;
      file->delete = true;
      file->dpath = MakeTempPathTemplate();
      if ((dfd = mkstemp(file->dpath)) == -1)
        DieSys(file->dpath);
      FILE *df = fdopen(dfd, "w");

      // concatenate gzip files to raw deflate
      size_t tot = 0;
      for (size_t i = 0; i < count; ++i) {
        int clr = !(i == count - 1);
        gzcopy(hunks[i].dpath, clr, &file->crc, &tot, df);
        unlink(hunks[i].dpath);
      }

      // cleanup
      if (fclose(df))
        DieSys(file->dpath);
      free(hunks);
    } else if (S_ISREG(file->mode)) {

      // open input file
      int fd;
      if ((fd = open(file->path, O_RDONLY | O_NOCTTY)) == -1)
        DieSys(file->path);

      // open output file
      int dfd;
      file->delete = true;
      file->dpath = MakeTempPathTemplate();
      if ((dfd = mkstemp(file->dpath)) == -1)
        DieSys(file->dpath);

      // initialize zlib in raw deflate mode
      z_stream zs;
      NewDeflate(&zs, -MAX_WBITS);

      // copy file
      size_t need;
      int conts = 0;
      uint64_t compsize = 0;
      for (size_t i = 0; i < file->size; i += need) {
        // read chunk
        need = MIN(CHUNK, file->size - i);
        ssize_t rc = pread(fd, iobuf, need, i);
        if (rc != need)
          DieSys(file->path);
        // discern binary from ascii from utf8
        file->cat = ClassifyData(iobuf, need, file->cat, &conts);
        // hash original content
        file->crc = crc32_z(file->crc, iobuf, need);
        if (file->comp == kZipCompressionNone) {
          // copy uncompressed data to output
          if (write(dfd, iobuf, need) != need)
            DieSys(file->dpath);
        } else {
          // compress chunk and write to output
          zs.avail_in = need;
          zs.next_in = iobuf;
          do {
            zs.next_out = cdbuf;
            zs.avail_out = CHUNK;
            RunDeflate(&zs, i + need >= file->size);
            ssize_t have = CHUNK - zs.avail_out;
            if (write(dfd, cdbuf, have) != have)
              DieSys(file->dpath);
            compsize += have;
          } while (!zs.avail_out);
        }
      }

      // cleanup
      unassert(deflateEnd(&zs) == Z_OK);
      if (close(dfd))
        DieSys(file->dpath);
      if (close(fd))
        DieSys(file->path);
      if (conts)
        file->cat = BINARY;

      // throw away compressed file if it's bigger
      if (file->comp == kZipCompressionDeflate && compsize > file->size) {
        unlink(file->dpath);
        file->delete = false;
        file->dpath = file->path;
        file->comp = kZipCompressionNone;
      }
    }

    // tell main thread file is ready
    pthread_mutex_lock(&file->mu);
    file->ready = true;
    pthread_cond_signal(&file->cv);
    pthread_mutex_unlock(&file->mu);
  }
  free(cdbuf);
  free(iobuf);
  return 0;
}

static void HandleArg(const char *arg) {
  if (flag_recursive) {
    if (nftw(arg, VisitFile, 1024, flag_symbolic ? FTW_PHYS : 0) == -1) {
      perror(arg);
      exit(1);
    }
  } else {
    struct stat st;
    if (fstatat(AT_FDCWD, arg, &st,  //
                flag_symbolic ? AT_SYMLINK_NOFOLLOW : 0) == -1)
      DieSys(arg);
    AddFile(arg, &st);
  }
}

int main(int argc, char *argv[]) {

  // get name of program
  if ((prog = argv[0])) {
    prog = basename(prog);
  } else {
    prog = "fasterzip";
  }

  // show help
  if (argc < 2) {
    tinyprint(2, help, NULL);
    return 1;
  }
  if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
    tinyprint(1, help, NULL);
    return 0;
  }

  // parse flags
  int opt;
  while ((opt = getopt(argc, argv, "@0123456789vjryqSDNa:n:p:x:")) != -1) {
    switch (opt) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        flag_level = opt - '0';
        break;
      case 'v':
        ++flag_verbose;
        break;
      case 'j':
        flag_junk = true;
        break;
      case 'y':
        flag_symbolic = true;
        break;
      case 'r':
        flag_recursive = true;
        break;
      case 'S':
        flag_sortnames = true;
        break;
      case '@':
        flag_stdinargs = true;
        break;
      case 'D':
        flag_nodirectories = true;
        break;
      case 'N':
        flag_nondeterministic = true;
        break;
      case 'n':
        flag_nocompsuffixes = optarg;
        break;
      case 'q':
        break;
      case 'p':
        flag_threads = atoi(optarg);
        break;
      case 'a':
        flag_alignment = atoi(optarg);
        if (flag_alignment < 1)
          Die(prog, "flag_alignment must be at least 1");
        if (flag_alignment & (flag_alignment - 1))
          Die(prog, "flag_alignment must be two power");
        break;
      case 'x':
        AppendString(&flag_excludes, optarg);
        break;
      default:
        return 1;
    }
  }
  if (optind == argc)
    Die(prog, "missing output argument");

  // get path of output file
  const char *zpath = argv[optind++];

  // create object for each input file
  for (int i = optind; i < argc; ++i)
    HandleArg(argv[i]);
  if (flag_recursive || flag_sortnames)
    qsort(g_files.p, g_files.n, sizeof(*g_files.p), CompareZipNames);
  const char *arg;
  if (flag_stdinargs)
    while ((arg = chomp(fgetln(stdin, 0))))
      HandleArg(arg);

  // launch worker threads
  if (flag_level) {
    if (flag_threads < 1) {
      flag_threads = cosmo_cpu_count();
      if (flag_threads > g_files.n)
        flag_threads = g_files.n;
      if (flag_threads < 1)
        flag_threads = 1;
    }
    sem_init(&g_sem, 0, flag_threads);
    pthread_t *th = Malloc(sizeof(pthread_t) * flag_threads);
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    for (int i = 0; i < flag_threads; ++i) {
      errno_t err;
      if ((err = pthread_create(&th[i], &tattr, Kompressor, 0)))
        Die("pthread_create", strerror(err));
    }
    pthread_attr_destroy(&tattr);
  } else {
    for (size_t i = 0; i < g_files.n; ++i)
      g_files.p[i]->ready = true;
  }

  // open output file
  int zfd;
  ssize_t zsize;
  if ((zfd = open(zpath, O_CREAT | O_RDWR, 0644)) == -1)
    DieSys(zpath);
  if ((zsize = lseek(zfd, 0, SEEK_END)) == -1)
    DieSys(zpath);

  // read last 64kb of file
  int amt;
  off_t off;
  if (zsize <= 65536) {
    off = 0;
    amt = zsize;
  } else {
    off = zsize - 65536;
    amt = zsize - off;
  }
  static char last64[65536];
  if (pread(zfd, last64, amt, off) != amt)
    DieSys(zpath);

  // search backwards for the end-of-central-directory record
  // the eocd (cdir) says where the central directory (cfile) array is located
  // we consistency check some legacy fields, to be extra sure that it is eocd
  unsigned cnt = 0;
  for (int i = amt - MIN(kZipCdirHdrMinSize, kZipCdir64LocatorSize); i >= 0;
       --i) {
    uint32_t magic = ZIP_READ32(last64 + i);
    if (magic == kZipCdir64LocatorMagic && i + kZipCdir64LocatorSize <= amt &&
        pread(zfd, last64, kZipCdir64HdrMinSize,
              ZIP_LOCATE64_OFFSET(last64 + i)) == (long)kZipCdir64HdrMinSize &&
        ZIP_READ32(last64) == kZipCdir64HdrMagic &&
        ZIP_CDIR64_RECORDS(last64) == ZIP_CDIR64_RECORDSONDISK(last64) &&
        ZIP_CDIR64_RECORDS(last64) && ZIP_CDIR64_SIZE(last64) <= INT_MAX) {
      if (ZIP_CDIR64_VERSIONNEED(last64) > kZipEra2001)
        Die(zpath, "zip archive requires unsupported features");
      cnt = ZIP_CDIR64_RECORDS(last64);
      off = ZIP_CDIR64_OFFSET(last64);
      amt = ZIP_CDIR64_SIZE(last64);
      break;
    }
    if (magic == kZipCdirHdrMagic && i + kZipCdirHdrMinSize <= amt &&
        ZIP_CDIR_RECORDS(last64 + i) == ZIP_CDIR_RECORDSONDISK(last64 + i) &&
        ZIP_CDIR_RECORDS(last64 + i) && ZIP_CDIR_SIZE(last64 + i) <= INT_MAX &&
        ZIP_CDIR_OFFSET(last64 + i) != 0xffffffffu) {
      cnt = ZIP_CDIR_RECORDS(last64 + i);
      off = ZIP_CDIR_OFFSET(last64 + i);
      amt = ZIP_CDIR_SIZE(last64 + i);
      break;
    }
  }
  if (!cnt)
    amt = 0;

  // read central directory
  size_t cdirsize = amt;
  uint8_t *cdir = Malloc(cdirsize);
  if (pread(zfd, cdir, cdirsize, off) != cdirsize)
    DieSys(zpath);

  // verify there's no duplicate zip asset names
  // this can happen innocently when using the junk flag
  for (int i = 0; i < g_files.n; ++i)
    for (int j = i + 1; j < g_files.n; ++j)
      if (!strcmp(g_files.p[i]->name, g_files.p[j]->name))
        Die(g_files.p[i]->name, "zip asset name specified multiple times");

  // delete central directory entries about to be replaced
  int new_count = 0;
  off_t new_index = 0;
  unsigned entry_index, entry_offset;
  uint8_t *new_cdir = Malloc(cdirsize);
  for (entry_index = entry_offset = 0;
       entry_index < cnt && entry_offset + kZipCfileHdrMinSize <= cdirsize &&
       entry_offset + ZIP_CFILE_HDRSIZE(cdir + entry_offset) <= cdirsize;
       ++entry_index, entry_offset += ZIP_CFILE_HDRSIZE(cdir + entry_offset)) {
    if (ZIP_CFILE_MAGIC(cdir + entry_offset) != kZipCfileHdrMagic)
      Die(zpath, "corrupted zip central directory entry magic");

    // check if entry name matches any of the new names
    bool found = false;
    for (int i = 0; i < g_files.n; ++i)
      if (ZIP_CFILE_NAMESIZE(cdir + entry_offset) ==
              strlen(g_files.p[i]->name) &&
          (!ZIP_CFILE_NAMESIZE(cdir + entry_offset) ||
           !memcmp(ZIP_CFILE_NAME(cdir + entry_offset), g_files.p[i]->name,
                   ZIP_CFILE_NAMESIZE(cdir + entry_offset)))) {
        found = true;
        break;
      }

    // copy back central directory entry
    if (!found) {
      memcpy(new_cdir + new_index, cdir + entry_offset,
             ZIP_CFILE_HDRSIZE(cdir + entry_offset));
      new_index += ZIP_CFILE_HDRSIZE(cdir + new_index);
      ++new_count;
    }
  }
  free(cdir);
  cdir = new_cdir;
  cdirsize = new_index;
  cnt = new_count;

  // add inputs
  for (size_t i = 0; i < g_files.n; ++i) {
    struct File *file = g_files.p[i];

    // wait for file to finish compressing
    pthread_mutex_lock(&file->mu);
    while (!file->ready)
      pthread_cond_wait(&file->cv, &file->mu);
    pthread_mutex_unlock(&file->mu);

    // determine size and alignment of local file header
    char *name = file->name;
    size_t namlen = strlen(name);
    size_t extlen = (2 + 2 + 8 * 2);
    size_t hdrlen = kZipLfileHdrMinSize + namlen + extlen;
    while ((zsize + hdrlen) & (flag_alignment - 1))
      ++zsize;

    // copy file data into zip
    uint64_t compsize = 0;
    uint32_t crc = file->crc;
    if (S_ISLNK(file->mode)) {
      char *linkdata = Malloc(file->size);
      if (readlink(file->path, linkdata, file->size) != file->size)
        DieSys(file->path);
      crc = crc32_z(0, linkdata, file->size);
      if (pwrite(zfd, linkdata, file->size, zsize + hdrlen) != file->size)
        DieSys(zpath);
      free(linkdata);
      compsize = file->size;
    } else if (S_ISREG(file->mode)) {
      int fd;
      if ((fd = open(file->dpath, O_RDONLY | O_NOCTTY)) == -1)
        DieSys(file->dpath);
      static uint8_t iobuf[65536];
      for (;;) {
        ssize_t rc = read(fd, iobuf, sizeof(iobuf));
        if (rc == -1)
          DieSys(file->dpath);
        if (!rc)
          break;
        if (!flag_level)
          crc = crc32_z(crc, iobuf, rc);
        if (pwrite(zfd, iobuf, rc, zsize + hdrlen + compsize) != rc)
          DieSys(zpath);
        compsize += rc;
      }
      if (close(fd))
        DieSys(file->dpath);
      if (file->delete)
        unlink(file->dpath);
    }

    // determine general flags
    int gflags = 0;
    if (file->cat == UTF8)
      gflags |= kZipGflagUtf8;

    // determine internal attributes
    int iattrs = 0;
    if (file->cat >= ASCII)
      iattrs = kZipIattrText;

    // determine external attributes
    int eattrs = 0;
    if (file->mode & 0200)
      eattrs |= kNtFileAttributeReadonly;
    eattrs |= FixMode(file->mode) << 16;

    // write local file header
    uint8_t *lochdr = Malloc(hdrlen);
    uint8_t *p = lochdr;

    p = ZIP_WRITE32(p, kZipLfileHdrMagic);
    p = ZIP_WRITE16(p, kZipEra2001);
    p = ZIP_WRITE16(p, gflags);
    p = ZIP_WRITE16(p, file->comp);
    p = ZIP_WRITE16(p, file->mtime);
    p = ZIP_WRITE16(p, file->mdate);
    p = ZIP_WRITE32(p, crc);
    p = ZIP_WRITE32(p, 0xffffffffu);  // compressed size
    p = ZIP_WRITE32(p, 0xffffffffu);  // uncompressed size
    p = ZIP_WRITE16(p, namlen);
    p = ZIP_WRITE16(p, extlen);
    p = mempcpy(p, name, namlen);

    p = ZIP_WRITE16(p, kZipExtraZip64);
    p = ZIP_WRITE16(p, 8 + 8);
    p = ZIP_WRITE64(p, file->size);  // uncompressed size
    p = ZIP_WRITE64(p, compsize);    // compressed size

    unassert(p == lochdr + hdrlen);
    if (pwrite(zfd, lochdr, hdrlen, zsize) != hdrlen)
      DieSys(zpath);
    free(lochdr);

    // create central directory entry
    extlen = (2 + 2 + 8 * 3) + (2 + 2 + 4 + 2 + 2 + 8 * 1);
    hdrlen = kZipCfileHdrMinSize + namlen + extlen;
    cdir = Realloc(cdir, cdirsize + hdrlen);
    uint64_t mt = TimeSpecToWindowsTime(file->modified);
    uint8_t *cdirhdr = cdir + cdirsize;
    cdirsize += hdrlen;
    p = cdirhdr;

    p = ZIP_WRITE32(p, kZipCfileHdrMagic);
    p = ZIP_WRITE16(p, kZipOsUnix << 8 | kZipEra2001);  // version made by
    p = ZIP_WRITE16(p, kZipEra2001);  // version needed to extract
    p = ZIP_WRITE16(p, gflags);
    p = ZIP_WRITE16(p, file->comp);
    p = ZIP_WRITE16(p, file->mtime);
    p = ZIP_WRITE16(p, file->mdate);
    p = ZIP_WRITE32(p, crc);
    p = ZIP_WRITE32(p, 0xffffffffu);  // compressed size
    p = ZIP_WRITE32(p, 0xffffffffu);  // uncompressed size
    p = ZIP_WRITE16(p, namlen);
    p = ZIP_WRITE16(p, extlen);
    p = ZIP_WRITE16(p, 0);            // comment length
    p = ZIP_WRITE16(p, 0);            // disk number start
    p = ZIP_WRITE16(p, iattrs);       // internal file attributes
    p = ZIP_WRITE32(p, eattrs);       // external file attributes
    p = ZIP_WRITE32(p, 0xffffffffu);  // lfile offset
    p = mempcpy(p, name, namlen);

    p = ZIP_WRITE16(p, kZipExtraZip64);
    p = ZIP_WRITE16(p, 8 + 8 + 8);
    p = ZIP_WRITE64(p, file->size);  // uncompressed size
    p = ZIP_WRITE64(p, compsize);    // compressed size
    p = ZIP_WRITE64(p, zsize);       // lfile offset

    p = ZIP_WRITE16(p, kZipExtraNtfs);
    p = ZIP_WRITE16(p, 4 + 2 + 2 + 8 * 1);
    p = ZIP_WRITE32(p, 0);      // ntfs reserved
    p = ZIP_WRITE16(p, 1);      // ntfs attribute tag
    p = ZIP_WRITE16(p, 8 * 1);  // ntfs attribute size
    p = ZIP_WRITE64(p, mt);     // last modification time

    // finish up
    unassert(p == cdirhdr + hdrlen);
    ++cnt;
    zsize += hdrlen + compsize;

    // log asset creation
    if (flag_verbose)
      tinyprint(2, file->path, " -> ", name, "\n", NULL);
  }

  // ensure remainder of last page in last file mapped is zero
  while (zsize & (flag_alignment - 1))
    ++zsize;

  // write out central directory
  if (pwrite(zfd, cdir, cdirsize, zsize) != cdirsize)
    DieSys(zpath);
  free(cdir);

  // write out end of central directory
  uint8_t
      eocd[kZipCdirHdrMinSize + kZipCdir64HdrMinSize + kZipCdir64LocatorSize];
  uint8_t *p = eocd;
  p = ZIP_WRITE32(p, kZipCdir64HdrMagic);
  p = ZIP_WRITE64(p, kZipCdir64HdrMinSize - 12);      // size of eocd64
  p = ZIP_WRITE16(p, kZipOsUnix << 8 | kZipEra2001);  // version made by
  p = ZIP_WRITE16(p, kZipEra2001);  // version needed to extract
  p = ZIP_WRITE32(p, 0);            // number of this disk
  p = ZIP_WRITE32(p, 0);    // number of disk with start of central directory
  p = ZIP_WRITE64(p, cnt);  // number of records on disk
  p = ZIP_WRITE64(p, cnt);  // number of records
  p = ZIP_WRITE64(p, cdirsize);  // size of central directory
  p = ZIP_WRITE64(p, zsize);     // offset of start of central directory
  p = ZIP_WRITE32(p, kZipCdir64LocatorMagic);
  p = ZIP_WRITE32(p, 0);                 // number of disk with eocd64
  p = ZIP_WRITE64(p, zsize + cdirsize);  // offset of eocd64
  p = ZIP_WRITE32(p, 1);                 // total number of disks
  p = ZIP_WRITE32(p, kZipCdirHdrMagic);
  p = ZIP_WRITE16(p, 0);            // number of this disk
  p = ZIP_WRITE16(p, 0);            // number of disks
  p = ZIP_WRITE16(p, cnt);          // number of records on disk
  p = ZIP_WRITE16(p, cnt);          // number of records
  p = ZIP_WRITE32(p, cdirsize);     // size of central directory
  p = ZIP_WRITE32(p, 0xffffffffu);  // offset of central directory
  p = ZIP_WRITE16(p, 0);            // comment length
  unassert(p == eocd + sizeof(eocd));
  if (pwrite(zfd, eocd, sizeof(eocd), zsize + cdirsize) != sizeof(eocd))
    DieSys(zpath);

  // close output
  if (close(zfd))
    DieSys(zpath);
}
