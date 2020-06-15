#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILESTREAMINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILESTREAMINFORMATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtFileStreamInformation {
  uint32_t NextEntryOffset;
  uint32_t StreamNameLength;
  int64_t StreamSize;
  int64_t StreamAllocationSize;
  char16_t StreamName[1];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILESTREAMINFORMATION_H_ */
