#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILESTANDARDINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILESTANDARDINFORMATION_H_

struct NtFileStandardInformation {
  int64_t AllocationSize;
  int64_t EndOfFile;
  uint32_t NumberOfLinks;
  bool32 DeletePending;
  bool32 Directory;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILESTANDARDINFORMATION_H_ */
