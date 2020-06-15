#ifndef COSMOPOLITAN_LIBC_NT_UNICODE_H_
#define COSMOPOLITAN_LIBC_NT_UNICODE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtUnicodeString {
  uint16_t Length;
  uint16_t MaxLength;
  char16_t *Data;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_UNICODE_H_ */
