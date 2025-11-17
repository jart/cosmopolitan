#ifndef COSMOPOLITAN_LIBC_RESOURCE_H_
#define COSMOPOLITAN_LIBC_RESOURCE_H_
COSMOPOLITAN_C_START_

#define RLIMIT_CPU     0
#define RLIMIT_FSIZE   1
#define RLIMIT_DATA    2
#define RLIMIT_STACK   3
#define RLIMIT_CORE    4
#define RLIMIT_RSS     5
#define RLIMIT_NPROC   6
#define RLIMIT_NOFILE  7
#define RLIMIT_MEMLOCK 8
#define RLIMIT_AS      9
#define RLIM_NLIMITS   10

#define RLIM_INFINITY  (~0ul)
#define RLIM_SAVED_CUR RLIM_INFINITY
#define RLIM_SAVED_MAX RLIM_INFINITY

struct rlimit {
  uint64_t rlim_cur;
  uint64_t rlim_max;
};

int getrlimit(int, struct rlimit *) libcesque;
int setrlimit(int, const struct rlimit *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RESOURCE_H_ */
