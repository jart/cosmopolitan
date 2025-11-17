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
#include "libc/bsdstdlib.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/statfs.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/libgen.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/tree.h"
#include "libc/limits.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/ftw.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/haclstar/haclstar.h"

static const char *help = "\
Copyright 2025 Justine Alexandra Roberts Tunney - ISC license\n\
Deduplicator 1.0 (2025-04-18). Usage:\n\
dedupe [FLAGS...] [FILES...]\n\
  The default action is to create hard links between duplicate files.\n\
  -v   increase verbosity           -h   show help\n\
  -d   direct i/o                   -s   create symbolic links\n\
  -n   dry run                      -p   run with low priority\n\
  -S   specify minimum file size    -M   specify maximum file size\n\
  -C   specify good chunk size      -B   specify big file size\n\
  -f   skip access check            -e   ignore permission errors\n\
\n";

#define MAX_PATH             ((size_t)4096)
#define GOOD_CHUNK_SIZE      65536
#define BIG_FILE_SIZE        100000000
#define EPHEMERAL_RATE_NANOS 40000000

#define FILE_BY_ID_CONTAINER(e)   TREE_CONTAINER(struct File, elem_id, e)
#define FILE_BY_SIZE_CONTAINER(e) TREE_CONTAINER(struct File, elem_size, e)

struct Path {
  char *path;
  struct stat st;
};

struct Paths {
  size_t n, c;
  struct Path **p;
};

struct File {
  struct Tree elem_id;
  struct Tree elem_size;
  struct File *extras;
  char *path;
  size_t size;
  int64_t dev;
  int64_t ino;
  uint32_t uid;
  uint32_t gid;
  size_t nlink;
  size_t waste;
  size_t blksize;
  bool have_hash;
  bool is_linked;
  bool is_readonly;
  uint8_t hash[32];
};

static int myuid;
static int mygid;
static char *prog;
static int verbose;
static bool dryrun;
static int o_direct;
static bool symbolic;
static int got_signal;
static long errors_ignored;
static bool has_ephemeral;
static int64_t file_count;
static long good_chunk_size;
static int64_t big_file_size;
static int64_t min_file_size;
static int64_t max_file_size = INT64_MAX;
static int64_t last_ephemeral;
static bool skip_access_check;
static bool ignore_access_errors;
static struct Paths paths;
static struct Tree *files_by_id;
static struct Tree *files_by_size;
static char ephemeral_line[512];

static void OnSignal(int sig) {
  got_signal = sig;
}

static int64_t GetNanos(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000000ll + ts.tv_nsec;
}

static void ClearEphemeral(void) {
  if (has_ephemeral) {
    fputs("\r\e[K", stderr);
    has_ephemeral = false;
  }
}

static void Log(const char *fmt, ...) {
  va_list va;
  if (has_ephemeral)
    fputs("\r\e[K", stderr);
  va_start(va, fmt);
  vfprintf(stderr, fmt, va);
  va_end(va);
  fputc('\n', stderr);
  if (has_ephemeral) {
    fputs(ephemeral_line, stderr);
    fflush(stderr);
  }
}

static void SetEphemeral(const char *fmt, ...) {
  va_list va;
  size_t size;
  int64_t now;
  struct winsize ws;
  now = GetNanos();
  if (now - last_ephemeral < EPHEMERAL_RATE_NANOS)
    return;
  ClearEphemeral();
  if (tcgetwinsize(fileno(stderr), &ws))
    return;
  size = MIN(ws.ws_col, sizeof(ephemeral_line));
  va_start(va, fmt);
  vsnprintf(ephemeral_line, size, fmt, va);
  va_end(va);
  fputs(ephemeral_line, stderr);
  fflush(stderr);
  has_ephemeral = true;
  last_ephemeral = now;
}

[[noreturn]] static void Die(const char *thing, const char *reason) {
  Log("%s: fatal error: %s", thing, reason);
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

static void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n)))
    DieOom();
  return p;
}

static void *MemAlign(size_t a, size_t n) {
  void *p;
  if (!(p = memalign(a, n)))
    DieOom();
  return p;
}

static char *StrDup(const char *s) {
  char *p;
  if (!(p = strdup(s)))
    DieOom();
  return p;
}

static int accessdir(const char *path, int mode) {
  char *dir = StrDup(path);
  int rc = access(dirname(dir), W_OK);
  free(dir);
  return rc;
}

// files come before directories
// name sorting happens per component
static int ComparePaths(const char *x, const char *y) {
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

static int ComparePathPointers(const void *a, const void *b) {
  return ComparePaths((*(const struct Path **)a)->path,
                      (*(const struct Path **)b)->path);
}

// computes target name for symlink to turn frompath into topath
// e.g SymPath("/a/b/foo.txt", "/a/c/bar.txt") -> "../b/foo.txt"
// this uses an strlcpy() api that returns >= dsize if truncated
size_t SymPath(char *dst, const char *to, const char *from, size_t dsize) {

  const char *t = to + 1;
  const char *te = to + strlen(to);

  const char *f = from + 1;
  const char *fe = from + strlen(from);

  // chomp common prefix
  for (;;) {
    size_t i = 0;
    for (;;) {
      if (t + i >= te || f + i >= fe || t[i] != f[i]) {
        i = 0;
        break;
      }
      if (t[i++] == '/')
        break;
    }
    if (!i)
      break;
    t += i;
    f += i;
  }

  // remove last component in from path
  while (fe > f && fe[-1] != '/')
    --fe;
  while (fe > f && fe[-1] == '/')
    --fe;

  // add dubbles
  size_t dlen = 0;
  while (fe > f) {
    if (dlen < dsize)
      dst[dlen] = '.';
    ++dlen;
    if (dlen < dsize)
      dst[dlen] = '.';
    ++dlen;
    if (dlen < dsize)
      dst[dlen] = '/';
    ++dlen;
    while (fe > f && fe[-1] != '/')
      --fe;
    while (fe > f && fe[-1] == '/')
      --fe;
  }

  // add to suffix
  if (!dlen)
    while (t < te && *t == '/')
      ++t;
  while (t < te) {
    if (dlen < dsize)
      dst[dlen] = *t;
    ++dlen;
    ++t;
  }

  // nul-terminate output
  if (dlen < dsize) {
    dst[dlen] = 0;
  } else if (dsize) {
    dst[dsize - 1] = 0;
  }
  return dlen;
}

static const char *BaseName(const char *path) {
  size_t i;
  if (!path || !*path)
    return ".";
  i = strlen(path);
  while (i && path[i - 1] != '/')
    --i;
  return path + i;
}

static int FileSearchById(const void *key, const struct Tree *node) {
  int rc;
  const struct File *x = (const struct File *)key;
  const struct File *y = FILE_BY_ID_CONTAINER(node);
  if ((rc = (x->dev > y->dev) - (x->dev < y->dev)))
    return rc;
  return (x->ino > y->ino) - (x->ino < y->ino);
}

static int FileCompareById(const struct Tree *a, const struct Tree *b) {
  int rc;
  const struct File *x = FILE_BY_ID_CONTAINER(a);
  const struct File *y = FILE_BY_ID_CONTAINER(b);
  if ((rc = (x->dev > y->dev) - (x->dev < y->dev)))
    return rc;
  return (x->ino > y->ino) - (x->ino < y->ino);
}

static int FileCompareBySize(const struct Tree *a, const struct Tree *b) {
  int rc;
  const struct File *x = FILE_BY_SIZE_CONTAINER(a);
  const struct File *y = FILE_BY_SIZE_CONTAINER(b);
  if ((rc = (x->size > y->size) - (x->size < y->size)))
    return rc;
  if ((rc = -((x->nlink > y->nlink) - (x->nlink < y->nlink))))
    return rc;
  // e.g. x86_64-linux-cosmo-as -> ../libexec/gcc/x86_64-linux-cosmo/14.1.0/as
  size_t xlen = strlen(BaseName(x->path));
  size_t ylen = strlen(BaseName(y->path));
  if ((rc = (xlen > ylen) - (xlen < ylen)))
    return rc;
  xlen = strlen(x->path);
  ylen = strlen(y->path);
  if ((rc = (xlen > ylen) - (xlen < ylen)))
    return rc;
  return ComparePaths(x->path, y->path);
}

static struct File *FileGetById(const struct File *key) {
  struct Tree *e;
  if ((e = tree_get(files_by_id, key, FileSearchById)))
    return FILE_BY_ID_CONTAINER(e);
  return 0;
}

static struct File *FileFirstBySize(void) {
  struct Tree *e;
  if ((e = tree_first(files_by_size)))
    return FILE_BY_SIZE_CONTAINER(e);
  return 0;
}

static struct File *FileNextBySize(struct File *f) {
  struct Tree *e;
  if ((e = tree_next(&f->elem_size)))
    return FILE_BY_SIZE_CONTAINER(e);
  return 0;
}

static void AddFile(const char *path, const struct stat *st) {
  struct File *f, *g;
  f = Malloc(sizeof(struct File));
  bzero(f, sizeof(*f));
  f->dev = st->st_dev;
  f->ino = st->st_ino;
  f->uid = st->st_uid;
  f->gid = st->st_gid;
  f->size = st->st_size;
  f->path = StrDup(path);
  f->nlink = st->st_nlink;
  f->waste = st->st_blocks * 512;
  if (!(f->blksize = st->st_blksize))
    f->blksize = 4096;
  if ((g = FileGetById(f))) {
    f->extras = g->extras;
    g->extras = f;
    return;
  }
  if (verbose > 2)
    Log("add %s", path);
  tree_insert(&files_by_size, &f->elem_size, FileCompareBySize);
  tree_insert(&files_by_id, &f->elem_id, FileCompareById);
  ++file_count;
}

static void AddPath(const char *path, const struct stat *st) {
  if (!st->st_blocks)
    return;
  if (!S_ISREG(st->st_mode))
    return;
  if (st->st_size < min_file_size)
    return;
  if (st->st_size > max_file_size)
    return;
  if (paths.n == paths.c) {
    paths.c += 1;
    paths.c += paths.c >> 1;
    paths.p = Realloc(paths.p, paths.c * sizeof(*paths.p));
  }
  paths.p[paths.n] = Malloc(sizeof(struct Path));
  paths.p[paths.n]->path = StrDup(path);
  paths.p[paths.n]->st = *st;
  paths.n++;
}

static int VisitFile(const char *path, const struct stat *st, int typeflag) {
  if (typeflag == FTW_F)
    AddPath(path, st);
  if (typeflag == FTW_D)
    if (verbose > 0)
      SetEphemeral("scanning %s", path);
  return 0;
}

static void Crawl(const char *path) {
  struct stat st;
  if (fstatat(AT_FDCWD, path, &st, AT_SYMLINK_NOFOLLOW)) {
    perror(path);
    exit(1);
  }
  if (S_ISDIR(st.st_mode)) {
    if (ftw(path, VisitFile, 1024) == -1) {
      perror(path);
      exit(1);
    }
  } else {
    AddPath(path, &st);
  }
}

static void *HasherNew(void) {
  void *hasher;
  if (X86_HAVE(SHA)) {
    hasher = Hacl_Hash_SHA2_malloc_256();
  } else {
    hasher = Hacl_Hash_Blake2b_malloc_256();
  }
  if (!hasher)
    DieOom();
  return hasher;
}

static int HasherUpdate(void *hasher, const void *data, uint32_t size) {
  if (X86_HAVE(SHA)) {
    return Hacl_Hash_SHA2_update_256(hasher, data, size);
  } else {
    return Hacl_Hash_Blake2b_update(hasher, data, size);
  }
}

static void HasherDigest(void *hasher, uint8_t digest[32]) {
  if (X86_HAVE(SHA)) {
    Hacl_Hash_SHA2_digest_256(hasher, digest);
  } else {
    Hacl_Hash_Blake2b_digest(hasher, digest);
  }
}

static void HasherFree(void *hasher) {
  if (X86_HAVE(SHA)) {
    Hacl_Hash_SHA2_free_256(hasher);
  } else {
    Hacl_Hash_Blake2b_free(hasher);
  }
}

static bool FileEqual(struct File *x, struct File *y, bool want_hash,
                      int64_t *total_compares) {
  if (x->dev != y->dev)
    return false;
  if (x->size != y->size)
    return false;
  ++*total_compares;
  if (x->have_hash)
    if (y->have_hash)
      return !memcmp(x->hash, y->hash, 32);
  size_t minchunk = good_chunk_size;
  minchunk += x->blksize - 1;
  minchunk &= -x->blksize;
  size_t chunk = MAX(x->blksize, minchunk);
  if (verbose > 2)
    Log("%s %s vs. %s in %'zu chunks", want_hash ? "hash" : "compare", x->path,
        y->path, chunk);
  int xfd, yfd;
  if ((xfd = open(x->path, O_RDONLY | o_direct | O_NOCTTY)) == -1)
    goto xfail;
  if ((yfd = open(y->path, O_RDONLY | o_direct | O_NOCTTY)) == -1)
    goto yfail;
  char *xbuf = MemAlign(x->blksize, chunk);
  char *ybuf = MemAlign(x->blksize, chunk);
  bool equal = true;
  bool finished_x = false;
  bool finished_y = false;
  bool should_hash_x = want_hash && !x->have_hash;
  bool should_hash_y = want_hash && !y->have_hash;
  void *hasher_x = 0;
  void *hasher_y = 0;
  if (should_hash_x)
    if (!(hasher_x = HasherNew()))
      DieOom();
  if (should_hash_y)
    if (!(hasher_y = HasherNew()))
      DieOom();
  size_t mychunk = want_hash ? chunk : x->blksize;
  for (;;) {
    if (got_signal)
      return false;
    ssize_t rc;
    ssize_t xrc = 0;
    bool should_read_x = !finished_x && (equal || should_hash_x);
    if (should_read_x)
      if ((xrc = read(xfd, xbuf, mychunk)) == -1)
        goto xfail;
    ssize_t yrc = 0;
    bool should_read_y = !finished_y && (equal || should_hash_y);
    if (should_read_y)
      if ((yrc = read(yfd, ybuf, mychunk)) == -1)
        goto yfail;
    while (yrc && xrc && xrc < yrc) {
      if ((rc = read(xfd, xbuf + xrc, yrc - xrc)) == -1)
        goto xfail;
      if (!rc)
        break;
      xrc += rc;
    }
    while (yrc && xrc && yrc < xrc) {
      if ((rc = read(yfd, ybuf + yrc, xrc - yrc)) == -1)
        goto yfail;
      if (!rc)
        break;
      yrc += rc;
    }
    if (should_read_x)
      if (!xrc)
        finished_x = true;
    if (should_read_y)
      if (!yrc)
        finished_y = true;
    if (!xrc && !yrc)
      break;
    if (should_read_x)
      if (should_read_y)
        if (xrc != yrc)
          equal = false;
    if (should_read_x ^ should_read_y)
      equal = false;
    if (equal)
      equal = !memcmp(xbuf, ybuf, xrc);
    if (should_hash_x)
      if (xrc)
        if (HasherUpdate(hasher_x, xbuf, xrc))
          DieOom();
    if (should_hash_y)
      if (yrc)
        if (HasherUpdate(hasher_y, ybuf, yrc))
          DieOom();
    mychunk = chunk;
  }
  free(xbuf);
  free(ybuf);
  if (close(xfd))
    goto xfail;
  if (close(yfd))
    goto yfail;
  if (finished_x && should_hash_x) {
    HasherDigest(hasher_x, x->hash);
    HasherFree(hasher_x);
    x->have_hash = true;
  }
  if (finished_y && should_hash_y) {
    HasherDigest(hasher_y, y->hash);
    HasherFree(hasher_y);
    y->have_hash = true;
  }
  if (x->have_hash)
    if (y->have_hash)
      npassert(equal == !memcmp(x->hash, y->hash, 32));
  return equal;
xfail:
  if (got_signal)
    return false;
  perror(x->path);
  exit(1);
yfail:
  if (got_signal)
    return false;
  perror(y->path);
  exit(1);
}

static bool LinkImpl(struct File *to, struct File *from) {
  if (!skip_access_check) {
    // make sure we have permission to destroy from path
    if (access(from->path, W_OK) || accessdir(from->path, W_OK)) {
      if (errno == EACCES) {
        if (!ignore_access_errors) {
          ++errors_ignored;
          Log("%s: %s", from->path, strerror(errno));
        }
        from->is_readonly = true;
        return false;
      } else {
        perror(from->path);
        exit(1);
      }
    }
    // we don't want the ownership of the from file to change
    if (!symbolic && (from->uid != to->uid ||  //
                      from->gid != to->gid)) {
      if (!ignore_access_errors) {
        ++errors_ignored;
        Log("%s: Ownership would change", from->path);
      }
      return false;
    }
  }
  char *tmp = Malloc(MAX_PATH);
  if (snprintf(tmp, MAX_PATH, "%s.%x", from->path, arc4random()) >= MAX_PATH) {
    Log("%s: Path too long", from->path);
    ++errors_ignored;
    return false;
  }
  if (rename(from->path, tmp)) {
    perror(tmp);
    exit(1);
  }
  int rc;
  if (!symbolic) {
    rc = link(to->path, from->path);
  } else {
    char *sympath = Malloc(MAX_PATH);
    npassert(SymPath(sympath, to->path, from->path, MAX_PATH) < MAX_PATH);
    rc = symlink(sympath, from->path);
    free(sympath);
  }
  if (rc) {
    rename(tmp, from->path);
    perror(from->path);
    exit(1);
  }
  if (unlink(tmp)) {
    ++errors_ignored;
    perror(tmp);
  }
  free(tmp);
  if (symbolic && (from->uid != myuid || from->gid != mygid)) {
    if (fchownat(AT_FDCWD, from->path, from->uid, from->gid,
                 AT_SYMLINK_NOFOLLOW)) {
      ++errors_ignored;
      perror(from->path);
    }
  }
  return !rc;
}

static bool Link(struct File *to, struct File *from) {
  bool ok;
  sigset_t ss, os;
  sigfillset(&ss);
  sigprocmask(SIG_SETMASK, &ss, &os);
  ok = LinkImpl(to, from);
  sigprocmask(SIG_SETMASK, &os, 0);
  return ok;
}

static void Dedupe(void) {
  struct File *e, *f, *g;
  int64_t bytes_saved = 0;
  int64_t files_processed = 0;
  int64_t duplicate_files = 0;
  int64_t file_comparisons = 0;
  for (f = FileFirstBySize(); f && !got_signal; f = FileNextBySize(f)) {
    ++files_processed;
    if (f->is_linked)
      continue;
    for (g = FileNextBySize(f); g && !got_signal; g = FileNextBySize(g)) {
      if (g->size != f->size)
        break;
      if (g->is_linked)
        continue;
      if (g->is_readonly)
        continue;
      if (verbose > 0)
        SetEphemeral("%.3f%% complete - %,ld size - %,ld files - "
                     "%,ld compares - %,ld duplicates - %,ld bytes saved",
                     (double)files_processed / file_count * 100, f->size,
                     files_processed, file_comparisons, duplicate_files,
                     bytes_saved);
      bool want_hash = false;
      if (f->size < big_file_size) {
        for (e = FileNextBySize(g); e; e = FileNextBySize(e)) {
          if (e->size != f->size)
            break;
          if (e->is_linked)
            continue;
          if (e->is_readonly)
            continue;
          want_hash = true;
          break;
        }
      }
      if (FileEqual(f, g, want_hash, &file_comparisons)) {
        bool once = false;
        ++duplicate_files;
        for (e = g; e; e = e->extras) {
          if (!dryrun) {
            e->is_linked = Link(f, e);
          } else {
            e->is_linked = true;
          }
          if (e->is_linked) {
            ++f->nlink;
            if (!once) {
              once = true;
              bytes_saved += e->waste;
            }
          }
          if (verbose > 1)
            if (e->is_linked)
              Log("linked %s <- %s", f->path, e->path);
        }
      }
    }
  }
  ClearEphemeral();
  if (verbose > 0) {
    if (got_signal)
      Log("interrupted by signal");
    if (errors_ignored)
      Log("%,ld errors ignored", errors_ignored);
    Log("%,ld files processed", files_processed);
    Log("%,ld file comparisons", file_comparisons);
    Log("%,ld duplicate files", duplicate_files);
    Log("%,ld bytes saved", bytes_saved);
  }
}

int main(int argc, char *argv[]) {

  // get name of program
  if ((prog = argv[0])) {
    prog = basename(prog);
  } else {
    prog = "dedupe";
  }

  // show help
  if (argc == 2 && !strcmp(argv[1], "--help")) {
    fputs(help, stdout);
    exit(0);
  }

  // parse flags
  int opt;
  while ((opt = getopt(argc, argv, "defhnpsvB:C:S:M:")) != -1) {
    switch (opt) {
      case 'v':
        ++verbose;
        break;
      case 'p':
        verynice();
        break;
      case 'n':
        dryrun = true;
        break;
      case 's':
        symbolic = true;
        break;
      case 'd':
        o_direct = O_DIRECT;
        break;
      case 'f':
        skip_access_check = true;
        break;
      case 'e':
        ignore_access_errors = true;
        break;
      case 'S':
        min_file_size = sizetol(optarg, 1000);
        break;
      case 'M':
        max_file_size = sizetol(optarg, 1000);
        break;
      case 'B':
        big_file_size = sizetol(optarg, 1000);
        break;
      case 'C':
        good_chunk_size = sizetol(optarg, 1000);
        break;
      case 'h':
        fputs(help, stdout);
        exit(0);
      default:
        exit(1);
    }
  }
  if (optind == argc)
    Die(prog, "missing path argument");

  if (good_chunk_size <= 0)
    good_chunk_size = GOOD_CHUNK_SIZE;
  if (good_chunk_size > 0x40000000)
    good_chunk_size = 0x40000000;

  if (big_file_size <= 0)
    big_file_size = BIG_FILE_SIZE;
  if (big_file_size > 0x40000000)
    big_file_size = 0x40000000;

  // scan
  if (verbose > 0)
    Log("scanning files...");
  for (int i = optind; i < argc; ++i) {
    char *path;
    if (!(path = realpath(argv[i], 0))) {
      perror(argv[i]);
      exit(1);
    }
    Crawl(path);
    free(path);
  }
  ClearEphemeral();

  // sort
  if (verbose > 0)
    Log("sorting files...");
  qsort(paths.p, paths.n, sizeof(*paths.p), ComparePathPointers);

  // index
  if (verbose > 0)
    Log("indexing files...");
  for (size_t i = 0; i < paths.n; ++i) {
    AddFile(paths.p[i]->path, &paths.p[i]->st);
    free(paths.p[i]->path);
    free(paths.p[i]);
  }
  free(paths.p);

  // setup
  myuid = getuid();
  mygid = getgid();
  atexit(ClearEphemeral);
  signal(SIGINT, OnSignal);
  signal(SIGTERM, OnSignal);

  // deduplicate
  if (verbose > 0)
    Log("deduplicating files...");
  Dedupe();

  // rethrow signal
  if (got_signal) {
    ClearEphemeral();
    fflush(stdout);
    fflush(stderr);
    sigset_t ss;
    sigemptyset(&ss);
    sigaddset(&ss, got_signal);
    sigprocmask(SIG_BLOCK, &ss, 0);
    raise(got_signal);
    exit(128 + got_signal);
  }

  // free memory
  if (IsModeDbg()) {
    struct File *f;
    while ((f = FileFirstBySize())) {
      struct File *e, *e2;
      tree_remove(&files_by_size, &f->elem_size);
      for (e = f->extras; e; e = e2) {
        e2 = e->extras;
        free(e->path);
        free(e);
      }
      free(f->path);
      free(f);
    }
    CheckForMemoryLeaks();
  }

  // return status
  if (errors_ignored)
    exit(1);
}
