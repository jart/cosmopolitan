#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SECURITYDESCRIPTOR_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SECURITYDESCRIPTOR_H_
#include "libc/nt/struct/acl.h"

struct NtSecurityDescriptor {
  uint8_t Revision;
  uint8_t Sbz1;
  uint16_t Control;
  void *Owner;
  void *Group;
  struct NtAcl *Sacl;
  struct NtAcl *Dacl;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SECURITYDESCRIPTOR_H_ */
