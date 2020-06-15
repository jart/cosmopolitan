#ifndef COSMOPOLITAN_LIBC_NT_ENUM_REGGETVALUEFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_REGGETVALUEFLAGS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtRegGetValueFlags {
  kNtRrfRtRegNone = 0x00000001,
  kNtRrfRtRegSz = 0x00000002,
  kNtRrfRtRegExpandSz = 0x00000004,
  kNtRrfRtRegBinary = 0x00000008,
  kNtRrfRtRegDword = 0x00000010,
  kNtRrfRtRegMultiSz = 0x00000020,
  kNtRrfRtRegQword = 0x00000040,
  kNtRrfRtDword = kNtRrfRtRegBinary | kNtRrfRtRegDword,
  kNtRrfRtQword = kNtRrfRtRegBinary | kNtRrfRtRegQword,
  kNtRrfRtAny = 0x0000ffff,
  kNtRrfSubkeyWow6464key = 0x00010000,
  kNtRrfSubkeyWow6432key = 0x00020000,
  kNtRrfWow64Mask = 0x00030000,
  kNtRrfNoexpand = 0x10000000,
  kNtRrfZeroonfailure = 0x20000000
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_REGGETVALUEFLAGS_H_ */
