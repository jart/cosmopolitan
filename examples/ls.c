#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/log/check.h"
#include "libc/mem/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/s.h"
#include "libc/x/xasprintf.h"

struct stat st;

const char *TypeToString(uint8_t type) {
  switch (type) {
    case DT_UNKNOWN:
      return "DT_UNKNOWN";
    case DT_FIFO:
      return "DT_FIFO";
    case DT_CHR:
      return "DT_CHR";
    case DT_DIR:
      return "DT_DIR";
    case DT_BLK:
      return "DT_BLK";
    case DT_REG:
      return "DT_REG";
    case DT_LNK:
      return "DT_LNK";
    case DT_SOCK:
      return "DT_SOCK";
    default:
      return "UNKNOWN";
  }
}

void List(const char *path) {
  DIR *d;
  struct dirent *e;
  const char *vpath;
  if (strcmp(path, ".") == 0) {
    vpath = "";
  } else if (!endswith(path, "/")) {
    vpath = gc(xasprintf("%s/", path));
  } else {
    vpath = path;
  }
  if (stat(path, &st) != -1) {
    if (S_ISDIR(st.st_mode)) {
      CHECK((d = opendir(path)));
      while ((e = readdir(d))) {
        printf("0x%016x 0x%016x %-10s %s%s\n", e->d_ino, e->d_off,
               TypeToString(e->d_type), vpath, e->d_name);
      }
      closedir(d);
    } else {
      printf("%s\n", path);
    }
  } else {
    fprintf(stderr, "not found: %s\n", path);
  }
}

int main(int argc, char *argv[]) {
  int i;
  if (argc == 1) {
    List(".");
  } else {
    for (i = 1; i < argc; ++i) {
      List(argv[i]);
    }
  }
  return 0;
}
