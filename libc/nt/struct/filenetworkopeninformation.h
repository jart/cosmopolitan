#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILENETWORKOPENINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILENETWORKOPENINFORMATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtFileNetworkOpenInformation {
  int64_t CreationTime;
  int64_t LastAccessTime;
  int64_t LastWriteTime;
  int64_t ChangeTime;
  int64_t AllocationSize;
  int64_t EndOfFile;
  uint32_t FileAttributes;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILENETWORKOPENINFORMATION_H_ */
