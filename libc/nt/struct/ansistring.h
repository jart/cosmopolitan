#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_ANSISTRING_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_ANSISTRING_H_

struct NtAnsiString {
  unsigned short Length;
  unsigned short MaximumLength;
  char *Buffer;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_ANSISTRING_H_ */
