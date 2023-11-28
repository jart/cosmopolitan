#ifndef _GETOPT_EXT_H
#define _GETOPT_EXT_H
#ifdef COSMOPOLITAN_GETOPT_H_
#error "don't mix getopt_long() with cosmo's getopt()"
#endif

#define no_argument       0
#define required_argument 1
#define optional_argument 2

COSMOPOLITAN_C_START_

struct option {
  const char *name;
  int has_arg;
  int *flag;
  int val;
};

int getopt_long(int, char *const *, const char *, const struct option *, int *);
int getopt_long_only(int, char *const *, const char *, const struct option *,
                     int *);

COSMOPOLITAN_C_END_
#endif /* _GETOPT_EXT_H */
