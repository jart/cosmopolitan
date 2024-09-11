#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <assert.h>
#include <cosmo.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

/**
 * @fileoverview File metadata viewer.
 *
 * This demonstrates the more powerful aspects of the printf() DSL.
 */

bool numeric;

char *xiso8601(struct timespec ts) {
  struct tm tm;
  if (!localtime_r(&ts.tv_sec, &tm))
    return 0;
  int len = 128;
  char *res = malloc(len);
  char *ptr = res;
  char *end = res + len;
  if (!res)
    return 0;
  ptr += strftime(ptr, end - ptr, "%Y-%m-%d %H:%M:%S", &tm);
  ptr += snprintf(ptr, end - ptr, ".%09ld", ts.tv_nsec);
  ptr += strftime(ptr, end - ptr, "%z %Z", &tm);
  unassert(ptr + 1 <= end);
  return res;
}

const char *DescribeFileType(unsigned mode) {
  switch (mode & S_IFMT) {
    case S_IFIFO:
      return "S_IFIFO"; /* pipe */
    case S_IFCHR:
      return "S_IFCHR"; /* character device */
    case S_IFDIR:
      return "S_IFDIR"; /* directory */
    case S_IFBLK:
      return "S_IFBLK"; /* block device */
    case S_IFREG:
      return "S_IFREG"; /* regular file */
    case S_IFLNK:
      return "S_IFLNK"; /* symbolic link */
    case S_IFSOCK:
      return "S_IFSOCK"; /* socket */
    default:
      return "wut";
  }
}

void PrintFileMetadata(const char *pathname, struct stat *st) {
  int fd;
  printf("\n%s:", pathname);
  if (numeric) {
    fd = atoi(pathname);
    if (fstat(fd, st)) {
      perror(pathname);
      exit(1);
    }
  } else {
    if (stat(pathname, st)) {
      perror(pathname);
      exit(1);
    }
  }
  printf("\n"
         "%-32s%,ld\n"
         "%-32s%,ld\n"
         "%-32s%#lx\n"
         "%-32s%#lx\n"
         "%-32s%ld\n"
         "%-32s%#o (%s)\n"
         "%-32s%d\n"
         "%-32s%d\n"
         "%-32s%d\n"
         "%-32s%d\n"
         "%-32s%ld\n"
         "%-32s%ld\n"
         "%-32s%s\n"
         "%-32s%s\n"
         "%-32s%s\n"
         "%-32s%s\n",
         "bytes in file:", st->st_size, "physical bytes:", st->st_blocks * 512,
         "device id w/ file:", st->st_dev, "inode:", st->st_ino,
         "hard link count:", st->st_nlink, "mode / permissions:", st->st_mode,
         DescribeFileType(st->st_mode), "owner id:", st->st_uid,
         "group id:", st->st_gid, "flags:", st->st_flags, "gen:", st->st_gen,
         "device id (if special):", st->st_rdev, "block size:", st->st_blksize,
         "access time:", gc(xiso8601(st->st_atim)),
         "modified time:", gc(xiso8601(st->st_mtim)),
         "c[omplicated]time:", gc(xiso8601(st->st_ctim)),
         "[birthtime]:", gc(xiso8601(st->st_birthtim)));
}

int main(int argc, char *argv[]) {
  size_t i;
  struct stat st;
  if (argc <= 1) {
    PrintFileMetadata(".", &st);
    return 0;
  }
  for (i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-n")) {
      numeric = true;
    } else {
      PrintFileMetadata(argv[i], &st);
    }
  }
  return 0;
}
