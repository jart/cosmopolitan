#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELINENUMBER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELINENUMBER_H_

struct NtImageLinenumber {
  union {
    uint32_t SymbolTableIndex;
    uint32_t VirtualAddress;
  } Type;
  uint16_t Linenumber;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELINENUMBER_H_ */
