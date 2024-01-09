#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_H_
COSMOPOLITAN_C_START_

struct sigaltstack {
  void *ss_sp;
  int ss_flags;
  size_t ss_size;
};

typedef struct sigaltstack stack_t;

int sigaltstack(const struct sigaltstack *, struct sigaltstack *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_H_ */
