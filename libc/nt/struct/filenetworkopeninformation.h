#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILENETWORKOPENINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILENETWORKOPENINFORMATION_H_

struct NtFileNetworkOpenInformation {
  int64_t CreationTime;
  int64_t LastAccessTime;
  int64_t LastWriteTime;
  int64_t ChangeTime;
  int64_t AllocationSize;
  int64_t EndOfFile;
  uint32_t FileAttributes;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILENETWORKOPENINFORMATION_H_ */
