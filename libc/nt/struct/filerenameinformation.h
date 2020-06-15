#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILERENAMEINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILERENAMEINFORMATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtFileRenameInformation {
  bool32 Replace;
  void *RootDir;
  uint32_t FileNameLength;
  char16_t FileName[1];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILERENAMEINFORMATION_H_ */
