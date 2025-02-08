#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CPINFOEX_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CPINFOEX_H_

struct NtCpInfoEx {
  uint32_t MaxCharSize;
  uint8_t DefaultChar[2];
  uint8_t LeadByte[12];
  char16_t UnicodeDefaultChar;
  uint32_t CodePage;
  char16_t CodePageName[260];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CPINFOEX_H_ */
