#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_OSVERSIONINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_OSVERSIONINFO_H_
COSMOPOLITAN_C_START_

struct NtOsVersionInfo {
  uint32_t dwOSVersionInfoSize;
  uint32_t dwMajorVersion;
  uint32_t dwMinorVersion;
  uint32_t dwBuildNumber;
  uint32_t dwPlatformId;
  char16_t szCSDVersion[128];
  uint16_t wServicePackMajor;
  uint16_t wServicePackMinor;
  uint16_t wSuiteMask;
  uint8_t wProductType;
  uint8_t wReserved;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_OSVERSIONINFO_H_ */
