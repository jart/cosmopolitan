#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CHARINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CHARINFO_H_

struct NtCharInfo {
  union {
    char16_t UnicodeChar;
    char AsciiChar;
  } Char;
  uint16_t Attributes;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CHARINFO_H_ */
