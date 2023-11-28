#ifndef COSMOPOLITAN_LIBC_NT_UNICODE_H_
#define COSMOPOLITAN_LIBC_NT_UNICODE_H_

struct NtUnicodeString {
  uint16_t Length;
  uint16_t MaxLength;
  char16_t *Data;
};

#endif /* COSMOPOLITAN_LIBC_NT_UNICODE_H_ */
