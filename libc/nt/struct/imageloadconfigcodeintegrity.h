#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELOADCONFIGCODEINTEGRITY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELOADCONFIGCODEINTEGRITY_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageLoadConfigCodeIntegrity {
  uint16_t Flags;
  uint16_t Catalog;
  uint32_t CatalogOffset;
  uint32_t Reserved;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGELOADCONFIGCODEINTEGRITY_H_ */
