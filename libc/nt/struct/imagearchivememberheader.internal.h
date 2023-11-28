#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEARCHIVEMEMBERHEADER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEARCHIVEMEMBERHEADER_H_

struct NtImageArchiveMemberHeader {
  uint8_t Name[16];
  uint8_t Date[12];
  uint8_t UserID[6];
  uint8_t GroupID[6];
  uint8_t Mode[8];
  uint8_t Size[10];
  uint8_t EndHeader[2];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEARCHIVEMEMBERHEADER_H_ */
