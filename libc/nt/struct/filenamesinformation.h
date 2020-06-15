#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILENAMESINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILENAMESINFORMATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtFileNamesInformation {
  uint32_t NextEntryOffset;
  uint32_t FileIndex;
  uint32_t FileNameLength;
  char16_t FileName[1];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILENAMESINFORMATION_H_ */
