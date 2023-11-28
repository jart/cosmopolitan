#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERELOCATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERELOCATION_H_

struct NtImageRelocation {
  union {
    uint32_t VirtualAddress;
    uint32_t RelocCount;
  };
  uint32_t SymbolTableIndex;
  uint16_t Type;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERELOCATION_H_ */
