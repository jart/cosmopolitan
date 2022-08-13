#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TMS_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TMS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct tms {
  int64_t tms_utime;  /* userspace time */
  int64_t tms_stime;  /* kernelspace time */
  int64_t tms_cutime; /* children userspace time */
  int64_t tms_cstime; /* children kernelspace time */
};

long times(struct tms *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TMS_H_ */
