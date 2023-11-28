#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOL_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOL_H_
#include "libc/nt/pedef.internal.h"
#include "libc/nt/struct/imageauxsymboltokendef.internal.h"

union NtImageAuxSymbol {
  struct {
    uint32_t TagIndex;
    union {
      struct {
        uint16_t Linenumber;
        uint16_t Size;
      } LnSz;
      uint32_t TotalSize;
    } Misc;
    union {
      struct {
        uint32_t PointerToLinenumber;
        uint32_t PointerToNextFunction;
      } Function;
      struct {
        uint16_t Dimension[4];
      } Array;
    } FcnAry;
    uint16_t TvIndex;
  } Sym;
  struct {
    uint8_t Name[kNtImageSizeofSymbol];
  } File;
  struct {
    uint32_t Length;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t CheckSum;
    uint16_t Number;
    uint8_t Selection;
    uint8_t bReserved;
    uint16_t HighNumber;
  } Section;
  struct NtImageAuxSymbolTokenDef TokenDef;
  struct {
    uint32_t crc;
    uint8_t rgbReserved[14];
  } CRC;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOL_H_ */
