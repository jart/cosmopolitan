#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILEDIRECTORYINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILEDIRECTORYINFORMATION_H_

struct NtFileDirectoryInformation {
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
  char16_t FileName[1];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILEDIRECTORYINFORMATION_H_ */
