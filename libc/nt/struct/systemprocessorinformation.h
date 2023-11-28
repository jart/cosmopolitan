#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMPROCESSORINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMPROCESSORINFORMATION_H_

struct NtSystemProcessorInformation {
  unsigned short ProcessorArchitecture;
  unsigned short ProcessorLevel;
  unsigned short ProcessorRevision;
  unsigned short Unknown;
  uint32_t FeatureBits;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMPROCESSORINFORMATION_H_ */
