#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_REPARSEDATABUFFER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_REPARSEDATABUFFER_H_
COSMOPOLITAN_C_START_

struct NtReparseDataBuffer {
  uint32_t ReparseTag;
  uint16_t ReparseDataLength;
  uint16_t Reserved;
  union {
    struct {
      uint16_t SubstituteNameOffset;
      uint16_t SubstituteNameLength;
      uint16_t PrintNameOffset;
      uint16_t PrintNameLength;
      uint32_t Flags;
      char16_t PathBuffer[1];
    } SymbolicLinkReparseBuffer;
    struct {
      uint16_t SubstituteNameOffset;
      uint16_t SubstituteNameLength;
      uint16_t PrintNameOffset;
      uint16_t PrintNameLength;
      char16_t PathBuffer[1];
    } MountPointReparseBuffer;
    struct {
      uint8_t DataBuffer[1];
    } GenericReparseBuffer;
  };
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_REPARSEDATABUFFER_H_ */
