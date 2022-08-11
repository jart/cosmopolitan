#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct timespec {
  int64_t tv_sec;
  int64_t tv_nsec; /* nanoseconds */
};

int sys_futex(int *, int, int, const struct timespec *, int *);
int64_t _timespec_tonanos(struct timespec) pureconst;
int64_t _timespec_tomicros(struct timespec) pureconst;
int64_t _timespec_tomillis(struct timespec) pureconst;
struct timespec _timespec_frommicros(int64_t) pureconst;
struct timespec _timespec_frommillis(int64_t) pureconst;
bool _timespec_eq(struct timespec, struct timespec) pureconst;
bool _timespec_gte(struct timespec, struct timespec) pureconst;
struct timespec _timespec_add(struct timespec, struct timespec) pureconst;
struct timespec _timespec_sub(struct timespec, struct timespec) pureconst;
struct timespec _timespec_real(void);
struct timespec _timespec_mono(void);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_ */
