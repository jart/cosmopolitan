#ifndef COSMOPOLITAN_LIBC_NT_ENUM_FORMATMESSAGEFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_FORMATMESSAGEFLAGS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtFormatMessageFlags {
  kNtFormatMessageAllocateBuffer = 0x100,
  kNtFormatMessageIgnoreInserts = 0x200,
  kNtFormatMessageFromString = 0x400,
  kNtFormatMessageFromHmodule = 0x800,
  kNtFormatMessageFromSystem = 0x1000,
  kNtFormatMessageArgumentArray = 0x2000,
  kNtFormatMessageMaxWidthMask = 0xff
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_FORMATMESSAGEFLAGS_H_ */
