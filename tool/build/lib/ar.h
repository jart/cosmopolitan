#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_AR_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_AR_H_
#include "libc/calls/struct/stat.h"
#include "libc/limits.h"
COSMOPOLITAN_C_START_

struct Ar {
  const char *path;
  int fd;
  struct stat st;
  char *map;
  size_t offset;
  const char *filenames;
  size_t filenames_size;
};

struct ArFile {
  void *data;
  size_t size;
  size_t offset;
  char name[PATH_MAX];
};

void openar(struct Ar *, const char *);
void closear(struct Ar *);
bool readar(struct Ar *, struct ArFile *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_AR_H_ */
