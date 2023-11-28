#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_GETARGS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_GETARGS_H_
COSMOPOLITAN_C_START_

struct GetArgs {
  size_t i, j;
  char **args;
  char *path;
  char *map;
  size_t mapsize;
};

void getargs_init(struct GetArgs *, char **);
const char *getargs_next(struct GetArgs *);
void getargs_destroy(struct GetArgs *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_GETARGS_H_ */
