#ifndef COSMOPOLITAN_LIBC_CALLS_METALFILE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_METALFILE_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct MetalFile {
  char *base;
  size_t size;
  size_t pos;
};

extern void *__ape_com_base;
extern size_t __ape_com_size;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define APE_COM_NAME "/proc/self/exe"

#endif /* COSMOPOLITAN_LIBC_CALLS_METALFILE_INTERNAL_H_ */
