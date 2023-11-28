#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMPORTOBJECTHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMPORTOBJECTHEADER_H_

struct NtImportObjectHeader {
  /* TODO(jart): No bitfields. */
  uint16_t Sig1;
  uint16_t Sig2;
  uint16_t Version;
  uint16_t Machine;
  uint32_t TimeDateStamp;
  uint32_t SizeOfData;
  union {
    uint16_t Ordinal;
    uint16_t Hint;
  };
  uint16_t Type : 2;
  uint16_t NameType : 3;
  uint16_t Reserved : 11;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMPORTOBJECTHEADER_H_ */
