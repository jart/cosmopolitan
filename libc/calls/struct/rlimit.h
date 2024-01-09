#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_H_
COSMOPOLITAN_C_START_

struct rlimit {
  uint64_t rlim_cur; /* current (soft) limit in bytes */
  uint64_t rlim_max; /* maximum limit in bytes */
};

int getrlimit(int, struct rlimit *) libcesque;
int setrlimit(int, const struct rlimit *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_H_ */
