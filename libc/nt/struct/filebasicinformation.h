#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILEBASICINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILEBASICINFORMATION_H_

struct NtFileBasicInformation {
  int64_t CreationTime;    /* in 100ns units */
  int64_t LastAccessTime;  /* in 100ns units */
  int64_t LastWriteTime;   /* in 100ns units */
  int64_t ChangeTime;      /* in 100ns units */
  uint32_t FileAttributes; /* kNtFileAttributeXXX */
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILEBASICINFORMATION_H_ */
