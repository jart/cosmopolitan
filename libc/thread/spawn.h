#ifndef COSMOPOLITAN_LIBC_THREAD_SPAWN_H_
#define COSMOPOLITAN_LIBC_THREAD_SPAWN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct spawn {
  int ptid;
  int *ctid;
  char *stk;
  char *tls;
  char *tib;
};

int _spawn(int (*)(void *, int), void *, struct spawn *) hidden;
int _join(struct spawn *) hidden;
char *_mktls(char **) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_SPAWN_H_ */
