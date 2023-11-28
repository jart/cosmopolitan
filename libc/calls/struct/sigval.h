#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGVAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGVAL_H_
COSMOPOLITAN_C_START_

union sigval {
  int32_t sival_int;
  void *sival_ptr;
};

int sigqueue(int, int, const union sigval);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGVAL_H_ */
