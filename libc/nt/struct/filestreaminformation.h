#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILESTREAMINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILESTREAMINFORMATION_H_

struct NtFileStreamInformation {
  uint32_t NextEntryOffset;
  uint32_t StreamNameLength;
  int64_t StreamSize;
  int64_t StreamAllocationSize;
  char16_t StreamName[1];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILESTREAMINFORMATION_H_ */
