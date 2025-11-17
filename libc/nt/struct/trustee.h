#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_TRUSTEE_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_TRUSTEE_H_
#include "libc/nt/struct/sid.h"

struct NtTrustee {
  struct NtTrustee *pMultipleTrustee;

#define kNtNoMultipleTrustee    0
#define kNtTrusteeIsImpersonate 1
  uint32_t MultipleTrusteeOperation;

#define kNtTrusteeIsSid            0
#define kNtTrusteeIsName           1
#define kNtTrusteeBadForm          2
#define kNtTrusteeIsObjectsAndSid  3
#define kNtTrusteeIsObjectsAndName 4
  uint32_t TrusteeForm;

#define kNtTrusteeIsUnknown        0
#define kNtTrusteeIsUser           1
#define kNtTrusteeIsGroup          2
#define kNtTrusteeIsDomain         3
#define kNtTrusteeIsAlias          4
#define kNtTrusteeIsWellKnownGroup 5
#define kNtTrusteeIsDeleted        6
#define kNtTrusteeIsInvalid        7
#define kNtTrusteeIsComputer       8
  uint32_t TrusteeType;

  union {
    char16_t *ptstrName;
    struct NtSid *pSid;
  };
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_TRUSTEE_H_ */
