#ifndef COSMOPOLITAN_LIBC_NT_ENUM_ERRORMODEFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_ERRORMODEFLAGS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

enum NtErrorModeFlags {
  kNtErrorModeDefault = 0x0,
  kNtSemFailcriticalerrors = 0x1,
  kNtSemNogpfaulterrorbox = 0x2,
  kNtSemNoopenfileerrorbox = 0x8000
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_ERRORMODEFLAGS_H_ */
