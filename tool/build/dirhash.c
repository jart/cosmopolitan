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
#include "libc/cosmo.h"
#include "libc/fmt/libgen.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/ftw.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/haclstar/haclstar.h"

struct Path {
  char *path;
};

struct Paths {
  size_t n, c;
  struct Path **p;
};

static char *prog;
static struct Paths paths;

[[noreturn]] static void Die(const char *thing, const char *reason) {
  fprintf(stderr, "%s: fatal error: %s\n", thing, reason);
  exit(1);
}

[[noreturn]] static void DieOom(void) {
  Die(prog, "out of memory");
}

[[noreturn]] static void DieSys(const char *thing) {
  perror(thing);
  exit(1);
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

static char *StrDup(const char *s) {
  char *p;
  if (!(p = strdup(s)))
    DieOom();
  return p;
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

static void AddPath(const char *path, const struct stat *st) {
  if (!S_ISREG(st->st_mode))
    return;
  if (paths.n == paths.c) {
    paths.c += 1;
    paths.c += paths.c >> 1;
    paths.p = Realloc(paths.p, paths.c * sizeof(*paths.p));
  }
  paths.p[paths.n] = Malloc(sizeof(struct Path));
  paths.p[paths.n]->path = StrDup(path);
  paths.n++;
}

static int VisitFile(const char *path, const struct stat *st, int typeflag) {
  if (typeflag == FTW_F)
    AddPath(path, st);
  return 0;
}

static void Crawl(const char *path) {
  struct stat st;
  if (fstatat(AT_FDCWD, path, &st, AT_SYMLINK_NOFOLLOW))
    DieSys(path);
  if (S_ISDIR(st.st_mode)) {
    if (ftw(path, VisitFile, 1024) == -1)
      DieSys(path);
  } else {
    AddPath(path, &st);
  }
}

static void Clear(void) {
  for (size_t i = 0; i < paths.n; ++i) {
    free(paths.p[i]->path);
    free(paths.p[i]);
  }
  free(paths.p);
  bzero(&paths, sizeof(paths));
}

int main(int argc, char *argv[]) {

  // get program name
  if ((prog = argv[0])) {
    prog = basename(prog);
  } else {
    prog = "dirhash";
  }

  // parse flags
  int opt;
  bool hash_mode = false;
  bool hash_user = false;
  bool hash_group = false;
  while ((opt = getopt(argc, argv, "mug")) != -1) {
    switch (opt) {
      case 'm':
        hash_mode = true;
        break;
      case 'u':
        hash_user = true;
        break;
      case 'g':
        hash_group = true;
        break;
      default:
        exit(1);
    }
  }
  if (optind == argc)
    Die(prog, "missing operand");

  // hash directories
  size_t bufsize = 2 * 1024 * 1024;
  char *buf = Malloc(bufsize);
  for (int i = optind; i < argc; ++i) {
    Crawl(argv[i]);
    qsort(paths.p, paths.n, sizeof(*paths.p), ComparePathPointers);
    Hacl_Hash_Blake2b_state_t *hasher;
    if (!(hasher = Hacl_Hash_Blake2b_malloc_256()))
      DieOom();
    for (size_t i = 0; i < paths.n; ++i) {
      int fd = open(paths.p[i]->path, O_RDONLY);
      if (hash_mode || hash_group || hash_user) {
        struct stat st;
        if (fstat(fd, &st))
          DieSys(paths.p[i]->path);
        if (hash_mode)
          Hacl_Hash_Blake2b_update(hasher, &st.st_mode, sizeof(st.st_mode));
        if (hash_user)
          Hacl_Hash_Blake2b_update(hasher, &st.st_uid, sizeof(st.st_uid));
        if (hash_group)
          Hacl_Hash_Blake2b_update(hasher, &st.st_gid, sizeof(st.st_gid));
      }
      if (fd == -1)
        DieSys(paths.p[i]->path);
      for (;;) {
        ssize_t rc = read(fd, buf, bufsize);
        if (rc == -1)
          DieSys(paths.p[i]->path);
        if (!rc)
          break;
        Hacl_Hash_Blake2b_update(hasher, buf, rc);
      }
      if (close(fd))
        DieSys(paths.p[i]->path);
    }
    uint8_t digest[32];
    Hacl_Hash_Blake2b_digest(hasher, digest);
    Hacl_Hash_Blake2b_free(hasher);
    char hash[32 * 2 + 1];
    hexpcpy(hash, digest, 32);
    printf("%s %s\n", hash, argv[i]);
    Clear();
  }
  free(buf);

  // sanity check
#ifndef NDEBUG
  CheckForMemoryLeaks();
#endif
}
