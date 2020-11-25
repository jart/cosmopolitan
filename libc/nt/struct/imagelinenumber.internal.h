#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELINENUMBER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELINENUMBER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageLinenumber {
  union {
    uint32_t SymbolTableIndex;
    uint32_t VirtualAddress;
  } Type;
  uint16_t Linenumber;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELINENUMBER_H_ */
