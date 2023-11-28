#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATIONV2_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATIONV2_H_

struct NtImageDynamicRelocationV2 {
  uint32_t HeaderSize;
  uint32_t FixupInfoSize;
  uint64_t Symbol;
  uint32_t SymbolGroup;
  uint32_t Flags;
  uint8_t FixupInfo[0];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATIONV2_H_ */
