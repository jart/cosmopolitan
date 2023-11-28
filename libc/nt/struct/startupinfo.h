#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_STARTUPINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_STARTUPINFO_H_

struct NtStartupInfo {
  uint32_t cb /* = sizeof(struct NtStartupInfo) */;
  uint16_t *lpReserved;
  char16_t *lpDesktop;
  char16_t *lpTitle; /* title of *new* console window only */
  uint32_t dwX;      /* position of window on screen */
  uint32_t dwY;
  uint32_t dwXSize;
  uint32_t dwYSize;
  uint32_t dwXCountChars; /* used to dimension the dos terminal */
  uint32_t dwYCountChars;
  uint32_t dwFillAttribute;
  uint32_t dwFlags;
  uint16_t wShowWindow;
  uint16_t cbReserved2;
  uint8_t *lpReserved2;
  union {
    struct {
      int64_t hStdInput;
      int64_t hStdOutput;
      int64_t hStdError;
    };
    int64_t stdiofds[3];
  };
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_STARTUPINFO_H_ */
