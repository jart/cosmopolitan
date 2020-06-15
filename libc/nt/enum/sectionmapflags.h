#ifndef COSMOPOLITAN_LIBC_NT_ENUM_SECTIONMAPFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_SECTIONMAPFLAGS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtSectionMapFlags {
  kNtSectionQuery = 0x0001,
  kNtSectionMapWrite = 0x0002,
  kNtSectionMapRead = 0x0004,
  kNtSectionMapExecute = 0x0008,
  kNtSectionExtendSize = 0x0010,
  kNtSectionMapExecuteExplicit = 0x0020
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_SECTIONMAPFLAGS_H_ */
