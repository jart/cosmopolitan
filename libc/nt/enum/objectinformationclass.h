#ifndef COSMOPOLITAN_LIBC_NT_ENUM_OBJECTINFORMATIONCLASS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_OBJECTINFORMATIONCLASS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtObjectInformationClass {
  kNtObjectBasicInformation, /*→ struct NtObjectBasicInformation */
  kNtObjectNameInformation,  /*→ etc. */
  kNtObjectTypeInformation,
  kNtObjectAllInformation,
  kNtObjectDataInformation
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_OBJECTINFORMATIONCLASS_H_ */
