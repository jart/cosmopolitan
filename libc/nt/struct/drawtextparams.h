#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_DRAWTEXTPARAMS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_DRAWTEXTPARAMS_H_
COSMOPOLITAN_C_START_

struct NtDrawTextParams {
  uint32_t cbSize;
  int32_t iTabLength;
  int32_t iLeftMargin;
  int32_t iRightMargin;
  uint32_t uiLengthDrawn;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_DRAWTEXTPARAMS_H_ */
