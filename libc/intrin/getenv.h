#ifndef COSMOPOLITAN_LIBC_INTRIN_GETENV_H_
#define COSMOPOLITAN_LIBC_INTRIN_GETENV_H_
COSMOPOLITAN_C_START_

struct Env {
  char *s;
  int i;
};

struct Env __getenv(char **, const char *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_GETENV_H_ */
