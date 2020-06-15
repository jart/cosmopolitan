#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TMS_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TMS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct tms {
  int64_t tms_utime;  /* userspace time */
  int64_t tms_stime;  /* kernelspace time */
  int64_t tms_cutime; /* children userspace time */
  int64_t tms_cstime; /* children kernelspace time */
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TMS_H_ */
