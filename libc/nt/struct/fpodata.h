#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FPODATA_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FPODATA_H_

struct NtFpoData {
  /* TODO(jart): No bitfields. */
  uint32_t ulOffStart;
  uint32_t cbProcSize;
  uint32_t cdwLocals;
  uint16_t cdwParams;
  uint16_t cbProlog : 8;
  uint16_t cbRegs : 3;
  uint16_t fHasSEH : 1;
  uint16_t fUseBP : 1;
  uint16_t reserved : 1;
  uint16_t cbFrame : 2;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FPODATA_H_ */
