#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGECOFFSYMBOLSHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGECOFFSYMBOLSHEADER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageCoffSymbolsHeader {
  uint32_t NumberOfSymbols;
  uint32_t LvaToFirstSymbol;
  uint32_t NumberOfLinenumbers;
  uint32_t LvaToFirstLinenumber;
  uint32_t RvaToFirstByteOfCode;
  uint32_t RvaToLastByteOfCode;
  uint32_t RvaToFirstByteOfData;
  uint32_t RvaToLastByteOfData;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGECOFFSYMBOLSHEADER_H_ */
