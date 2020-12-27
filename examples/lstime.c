#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/alg/alg.h"
#include "libc/alg/arraylist2.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/dt.h"
#include "libc/x/x.h"

struct stat st;
struct Files {
  size_t i, n;
  struct File {
    struct timespec mt;
    char *path;
  } * p;
} g_files;

int CompareFiles(struct File *a, struct File *b) {
  if (a->mt.tv_sec > b->mt.tv_sec) return 1;
  if (a->mt.tv_sec < b->mt.tv_sec) return -1;
  if (a->mt.tv_nsec > b->mt.tv_nsec) return 1;
  if (a->mt.tv_nsec < b->mt.tv_nsec) return -1;
  return strcmp(a->path, b->path);
}

void WalkPaths(const char *dirpath) {
  DIR *d;
  char *path;
  struct File f;
  struct dirent *e;
  CHECK((d = opendir(dirpath)));
  while ((e = readdir(d))) {
    if (strcmp(e->d_name, ".") == 0) continue;
    if (strcmp(e->d_name, "..") == 0) continue;
    path = xjoinpaths(dirpath, e->d_name);
    if (strcmp(e->d_name, "o") == 0) continue;
    if (strcmp(e->d_name, ".git") == 0) continue;
    if (e->d_type == DT_DIR) {
      WalkPaths(gc(path));
    } else {
      CHECK_NE(-1, lstat(path, &st), "%s", path);
      f.mt = st.st_mtim;
      f.path = path;
      APPEND(&g_files.p, &g_files.i, &g_files.n, &f);
    }
  }
  closedir(d);
}

void SortPaths(void) {
  qsort(g_files.p, g_files.i, sizeof(*g_files.p), (void *)CompareFiles);
}

void PrintPaths(void) {
  long i;
  char *ts;
  for (i = 0; i < g_files.i; ++i) {
    ts = xiso8601(&g_files.p[i].mt);
    printf("%s %s\n", ts, g_files.p[i].path);
    free(ts);
  }
}

void FreePaths(void) {
  long i;
  for (i = 0; i < g_files.i; ++i) free(g_files.p[i].path);
  g_files.i = 0;
}

void LsTime(void) {
  WalkPaths(".");
  SortPaths();
  PrintPaths();
  FreePaths();
}

int main(int argc, char *argv[]) {
  LsTime();
  return 0;
}
