#ifndef COSMOPOLITAN_GETOPT_H_
#define COSMOPOLITAN_GETOPT_H_
#ifdef COSMOPOLITAN_GETOPT_LONG_H_
#error "don't mix getopt_long() with cosmo's getopt()"
#endif

#define optarg   __optarg
#define optind   __optind
#define opterr   __opterr
#define optopt   __optopt
#define optreset __optreset
#define getopt   __getopt

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern char *optarg;
extern int optind, opterr, optopt, optreset;

int getopt(int, char *const[], const char *) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_GETOPT_H_ */
