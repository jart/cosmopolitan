#ifndef COSMOPOLITAN_LIBC_NT_ENUM_VALUEINFORMATIONCLASS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_VALUEINFORMATIONCLASS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtKeyValueInformationClass {
  kNtKeyValueBasicInformation,
  kNtKeyValueFullInformation,
  kNtKeyValuePartialInformation,
  kNtKeyValueFullInformationAlign64,
  kNtKeyValuePartialInformationAlign64
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_VALUEINFORMATIONCLASS_H_ */
