#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOLEX_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOLEX_H_
#include "libc/nt/struct/imageauxsymboltokendef.internal.h"
#include "libc/nt/struct/imagesymbolex.internal.h"

union NtImageAuxSymbolEx {
  struct {
    uint32_t WeakDefaultSymIndex;
    uint32_t WeakSearchType;
    uint8_t rgbReserved[12];
  } Sym;
  struct {
    uint8_t Name[sizeof(struct NtImageSymbolEx)];
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
    uint8_t rgbReserved[2];
  } Section;
  struct {
    struct NtImageAuxSymbolTokenDef TokenDef;
    uint8_t rgbReserved[2];
  };
  struct {
    uint32_t crc;
    uint8_t rgbReserved[16];
  } CRC;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEAUXSYMBOLEX_H_ */
