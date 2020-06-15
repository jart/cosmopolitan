#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMPROCESSORINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMPROCESSORINFORMATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtSystemProcessorInformation {
  unsigned short ProcessorArchitecture;
  unsigned short ProcessorLevel;
  unsigned short ProcessorRevision;
  unsigned short Unknown;
  uint32_t FeatureBits;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMPROCESSORINFORMATION_H_ */
