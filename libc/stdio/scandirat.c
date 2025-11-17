/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/dirent.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

/**
 * Scans directory for matching entries.
 *
 * This function opens a directory, filters entries using `sel`, sorts
 * them using `cmp`, and then saves them to `res`, which is created by
 * malloc() and each element is made by malloc() too. For example:
 *
 *     int n;
 *     struct dirent **list;
 *     const char *path = ".";
 *     if ((n = scandirat(AT_FDCWD, path, &list, NULL, alphasort)) == -1) {
 *       perror(path);
 *       exit(1);
 *     }
 *     for (int i = 0; i < n; ++i)
 *       printf("%s\n", list[i]->d_name);
 *     for (int i = 0; i < n; ++i)
 *       free(list[i]);
 *     free(list);
 *
 * @param dirfd is normally `AT_FDCWD` but if it's an open directory and
 *     `path` names a relative path then it's opened relative to `dirfd`
 * @param path is a UTF-8 string naming a filesystem entity
 * @param list is used to return pointer to list of dirent pointers
 * @param sel if not null returns zero if entry should be ignored
 * @param cmp is sorting comparator, or null to not sort
 * @return number of elements stored to `list`, or -1 w/ errno
 * @cancelationpoint
 */
int scandirat(int dirfd, const char *path, struct dirent ***list,
              int sel(const struct dirent *),
              int cmp(const struct dirent **, const struct dirent **)) {

  // open directory
  DIR *dir;
  if (!(dir = __opendirat(dirfd, path)))
    return -1;

  // setup array
  size_t cnt = 0;
  size_t cap = 8;
  struct dirent **res;
  if (!(res = malloc(cap * sizeof(*res))))
    goto OnFailure;

  // crawl directory
  struct dirent *ent;
  while ((ent = readdir(dir))) {

    // filter entries
    if (sel && !sel(ent))
      continue;

    // append new pointer
    if (cnt + 1 >= cap) {
      cap += cap >> 1;
      struct dirent **t;
      if (!(t = realloc(res, cap * sizeof(*res))))
        goto OnFailure;
      res = t;
    }
    if (!(res[cnt] = malloc(ent->d_reclen)))
      goto OnFailure;

    // copy entry to last pointer in list
    memcpy(res[cnt++], ent, ent->d_reclen);
  }

  // shrink list memory
  realloc_in_place(res, (cnt + 1) * sizeof(*res));

  // null terminate the list. posix doesn't require this and other libc
  // implementations don't do this but it just isn't right to not do it
  res[cnt] = 0;

  // close the directory
  if (closedir(dir))
    goto OnFailure;

  // sort the list if a comparator is specified
  if (cmp)
    qsort(res, cnt, sizeof(*res), (int (*)(const void *, const void *))cmp);

  // return result
  *list = res;
  return cnt;

OnFailure:
  closedir(dir);
  while (cnt)
    free(res[--cnt]);
  free(res);
  return -1;
}
