#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGECERUNTIMEFUNCTIONENTRY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGECERUNTIMEFUNCTIONENTRY_H_

struct NtImageCeRuntimeFunctionEntry {
  uint32_t FuncStart;
  uint32_t PrologLen : 8;
  uint32_t FuncLen : 22;
  uint32_t ThirtyTwoBit : 1;
  uint32_t ExceptionFlag : 1;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGECERUNTIMEFUNCTIONENTRY_H_ */
