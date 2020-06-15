#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILEFULLEAINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILEFULLEAINFORMATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtFileFullEaInformation {
  uint32_t NextEntryOffset;
  uint8_t Flags;
  uint8_t EaNameLength;
  uint16_t EaValueLength;
  char EaName[1];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILEFULLEAINFORMATION_H_ */
