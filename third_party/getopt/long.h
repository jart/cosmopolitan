#ifndef COSMOPOLITAN_GETOPT_LONG_H_
#define COSMOPOLITAN_GETOPT_LONG_H_
#ifdef COSMOPOLITAN_GETOPT_H_
#error "don't mix getopt_long() with cosmo's getopt()"
#endif

#define no_argument       0
#define required_argument 1
#define optional_argument 2

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct option {
  const char *name;
  int has_arg;
  int *flag;
  int val;
};

extern char *optarg;
extern int optind, opterr, optopt, optreset;

int getopt(int, char *const *, const char *);
int getopt_long(int, char *const *, const char *, const struct option *, int *);
int getopt_long_only(int, char *const *, const char *, const struct option *,
                     int *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_GETOPT_LONG_H_ */
