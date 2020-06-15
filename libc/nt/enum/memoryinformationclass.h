#ifndef COSMOPOLITAN_LIBC_NT_ENUM_MEMORYINFORMATIONCLASS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_MEMORYINFORMATIONCLASS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtMemoryInformationClass {
  kNtMemoryBasicInformation,
  kNtMemoryWorkingSetList,
  kNtMemorySectionName,
  kNtMemoryBasicVlmInformation
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_MEMORYINFORMATIONCLASS_H_ */
