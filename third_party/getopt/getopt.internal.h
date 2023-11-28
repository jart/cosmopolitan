#ifndef COSMOPOLITAN_GETOPT_H_
#define COSMOPOLITAN_GETOPT_H_
#if defined(_GETOPT_CORE_H) || defined(_GETOPT_EXT_H)
#error "don't mix getopt_long() with cosmo's getopt()"
#endif

#define optarg   __optarg
#define optind   __optind
#define opterr   __opterr
#define optopt   __optopt
#define optreset __optreset
#define getopt   __getopt

COSMOPOLITAN_C_START_

extern char *optarg;
extern int optind, opterr, optopt, optreset;

int getopt(int, char *const[], const char *) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_GETOPT_H_ */
