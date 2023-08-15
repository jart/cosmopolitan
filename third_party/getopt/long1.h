#ifndef COSMOPOLITAN_GETOPT_LONG1_H_
#define COSMOPOLITAN_GETOPT_LONG1_H_
#ifdef COSMOPOLITAN_GETOPT_H_
#error "don't mix getopt_long() with cosmo's getopt()"
#endif
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern char *optarg;
extern int optind, opterr, optopt, optreset;

int getopt(int, char *const *, const char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_GETOPT_LONG1_H_ */
