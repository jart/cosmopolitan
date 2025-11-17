#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_ACLSIZEINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_ACLSIZEINFO_H_

struct NtAclSizeInformation {
  uint32_t AceCount;
  uint32_t AclBytesInUse;
  uint32_t AclBytesFree;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_ACLSIZEINFO_H_ */
