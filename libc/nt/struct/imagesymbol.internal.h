#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESYMBOL_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESYMBOL_H_

struct NtImageSymbol {
  union {
    uint8_t ShortName[8];
    struct {
      uint32_t Short;
      uint32_t Long;
    } Name;
    uint32_t LongName[2];
  } N;
  uint32_t Value;
  uint16_t SectionNumber;
  uint16_t Type;
  uint8_t StorageClass;
  uint8_t NumberOfAuxSymbols;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGESYMBOL_H_ */
