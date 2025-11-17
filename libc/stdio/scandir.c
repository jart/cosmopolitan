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
#include "libc/sysv/consts/at.h"

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
 *     if ((n = scandir(path, &list, NULL, alphasort)) == -1) {
 *       perror(path);
 *       exit(1);
 *     }
 *     for (int i = 0; i < n; ++i)
 *       printf("%s\n", list[i]->d_name);
 *     for (int i = 0; i < n; ++i)
 *       free(list[i]);
 *     free(list);
 *
 * @param path is a UTF-8 string naming a filesystem entity
 * @param list is used to return pointer to list of dirent pointers
 * @param sel if not null returns zero if entry should be ignored
 * @param cmp is sorting comparator, or null to not sort
 * @return number of elements stored to `list`, or -1 w/ errno
 * @cancelationpoint
 */
int scandir(const char *path, struct dirent ***list,
            int sel(const struct dirent *),
            int cmp(const struct dirent **, const struct dirent **)) {
  return scandirat(AT_FDCWD, path, list, sel, cmp);
}
