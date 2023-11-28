#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_ACL_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_ACL_H_

struct NtAcl {
  uint8_t AclRevision;
  uint8_t Sbz1;
  uint16_t AclSize;
  uint16_t AceCount;
  uint16_t Sbz2;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_ACL_H_ */
