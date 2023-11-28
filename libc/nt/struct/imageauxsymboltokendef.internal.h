#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOLTOKENDEF_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOLTOKENDEF_H_

struct NtImageAuxSymbolTokenDef {
  uint8_t bAuxType;
  uint8_t bReserved;
  uint32_t SymbolTableIndex;
  uint8_t rgbReserved[12];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOLTOKENDEF_H_ */
