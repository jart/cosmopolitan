#ifndef COSMOPOLITAN_LIBC_NT_ENUM_REGTYPE_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_REGTYPE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtRegType {
  kNtRegNone = 0,
  kNtRegSz = 1,       /* UTF-16 NUL-terminated string */
  kNtRegExpandSz = 2, /* UTF-16 NUL-terminated string w/ env vars refs */
  kNtRegBinary = 3,
  kNtRegDword = 4,
  kNtRegDwordBigEndian = 5,
  kNtRegLink = 6,
  kNtRegMultiSz = 7, /* UTF-16 double-NUL-terminated string list */
  kNtRegResourceList = 8,
  kNtRegFullResourceDescriptor = 9,
  kNtRegResourceRequirementsList = 10,
  kNtRegQword = 11,
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_REGTYPE_H_ */
