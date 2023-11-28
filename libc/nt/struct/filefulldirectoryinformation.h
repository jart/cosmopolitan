#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILEFULLDIRECTORYINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILEFULLDIRECTORYINFORMATION_H_

struct NtFileFullDirectoryInformation {
  uint32_t NextEntryOffset;
  uint32_t FileIndex;
  int64_t CreationTime;
  int64_t LastAccessTime;
  int64_t LastWriteTime;
  int64_t ChangeTime;
  int64_t EndOfFile;
  int64_t AllocationSize;
  uint32_t FileAttributes;
  uint32_t FileNameLength;
  uint32_t EaSize;
  char16_t FileName[1];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILEFULLDIRECTORYINFORMATION_H_ */
