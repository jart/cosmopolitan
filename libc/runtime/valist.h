#ifndef COSMOPOLITAN_LIBC_RUNTIME_VALIST_H_
#define COSMOPOLITAN_LIBC_RUNTIME_VALIST_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct __va {
  unsigned int gp_offset;
  unsigned int fp_offset;
  void *overflow_arg_area;
  void *reg_save_area;
};

void *__va_arg(struct __va *, unsigned long, unsigned, unsigned);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_VALIST_H_ */
