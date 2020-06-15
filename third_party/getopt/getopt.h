#ifndef COSMOPOLITAN_THIRD_PARTY_GETOPT_GETOPT_H_
#define COSMOPOLITAN_THIRD_PARTY_GETOPT_GETOPT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern char *optarg;
extern int optind, opterr, optopt, optreset;
int getopt(int nargc, char *const nargv[], const char *ostr);

#define no_argument 0
#define required_argument 1
#define optional_argument 2
struct option {
  const char *name;
  int has_arg;
  int *flag;
  int val;
};
int getopt_long(int nargc, char *const *nargv, const char *options,
                const struct option *long_options, int *idx);
int getopt_long_only(int nargc, char *const *nargv, const char *options,
                     const struct option *long_options, int *idx);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_GETOPT_GETOPT_H_ */
