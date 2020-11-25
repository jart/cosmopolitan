#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDEBUGMISC_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDEBUGMISC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageDebugMisc {
  uint32_t DataType;
  uint32_t Length;
  bool32 Unicode;
  uint8_t Reserved[3];
  uint8_t Data[1];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDEBUGMISC_H_ */
