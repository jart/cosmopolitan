#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERUNTIMEFUNCTIONENTRY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERUNTIMEFUNCTIONENTRY_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageRuntimeFunctionEntry {
  uint32_t BeginAddress;
  uint32_t EndAddress;
  union {
    uint32_t UnwindInfoAddress;
    uint32_t UnwindData;
  };
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERUNTIMEFUNCTIONENTRY_H_ */
