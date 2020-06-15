#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CHARINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CHARINFO_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtCharInfo {
  union {
    char16_t UnicodeChar;
    char AsciiChar;
  } Char;
  uint16_t Attributes;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CHARINFO_H_ */
