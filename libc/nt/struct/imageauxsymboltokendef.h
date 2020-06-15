#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOLTOKENDEF_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOLTOKENDEF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageAuxSymbolTokenDef {
  uint8_t bAuxType;
  uint8_t bReserved;
  uint32_t SymbolTableIndex;
  uint8_t rgbReserved[12];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOLTOKENDEF_H_ */
