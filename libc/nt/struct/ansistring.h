#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_ANSISTRING_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_ANSISTRING_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtAnsiString {
  unsigned short Length;
  unsigned short MaximumLength;
  char *Buffer;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_ANSISTRING_H_ */
