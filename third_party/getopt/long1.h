#ifndef _GETOPT_CORE_H
#define _GETOPT_CORE_H
#ifdef COSMOPOLITAN_GETOPT_H_
#error "don't mix getopt_long() with cosmo's getopt()"
#endif
COSMOPOLITAN_C_START_

extern char *optarg;
extern int optind, opterr, optopt, optreset;

int getopt(int, char *const *, const char *);

COSMOPOLITAN_C_END_
#endif /* _GETOPT_CORE_H */
