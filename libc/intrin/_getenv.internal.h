#ifndef COSMOPOLITAN_LIBC_INTRIN__GETENV_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN__GETENV_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Env {
  char *s;
  int i;
};

struct Env _getenv(char **, const char *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN__GETENV_INTERNAL_H_ */
