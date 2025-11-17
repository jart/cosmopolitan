#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_ACEHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_ACEHEADER_H_

struct NtAceHeader {
  uint8_t AceType;
  uint8_t AceFlags;
  uint16_t AceSize;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_ACEHEADER_H_ */
