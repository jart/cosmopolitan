#ifndef COSMOPOLITAN_LIBC_NT_ENUM_KEYACCESS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_KEYACCESS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtKeyAccess {
  kNtKeyRead = 0x00020019,
  kNtKeyWrite = 0x00020006,
  kNtKeyExecute = 0x00020019,
  kNtKeyAllAccess = 0x000f003f
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_KEYACCESS_H_ */
