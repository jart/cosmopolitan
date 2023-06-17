#ifndef COSMOPOLITAN_THIRD_PARTY_GETOPT_GETOPT_H_
#define COSMOPOLITAN_THIRD_PARTY_GETOPT_GETOPT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern char *optarg;
extern int optind, opterr, optopt, optreset;

int getopt(int, char *const[], const char *) paramsnonnull();
int getsubopt(char **, char *const *, char **) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_GETOPT_GETOPT_H_ */
