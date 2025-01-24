#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CPINFOEX_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CPINFOEX_H_

#define MAX_LEADBYTES    12 
#define MAX_DEFAULTCHAR  2
#define MAX_PATH         260 

struct NtCpInfoEx {
  uint32_t MaxCharSize;
  uint8_t DefaultChar[MAX_DEFAULTCHAR];
  uint8_t LeadByte[MAX_LEADBYTES];
  char16_t UnicodeDefaultChar;
  uint32_t CodePage;
  char16_t CodePageName[MAX_PATH]
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CPINFOEX_H_ */
