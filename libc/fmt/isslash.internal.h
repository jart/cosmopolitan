#ifndef COSMOPOLITAN_LIBC_FMT_ISSLASH_H_
#define COSMOPOLITAN_LIBC_FMT_ISSLASH_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline bool isslash(int c) {
  return c == '/' || c == '\\';
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_ISSLASH_H_ */
