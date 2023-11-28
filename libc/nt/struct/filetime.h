#ifndef COSMOPOLITAN_LIBC_NT_FILETIME_H_
#define COSMOPOLITAN_LIBC_NT_FILETIME_H_

struct NtFileTime {
  uint32_t dwLowDateTime;
  uint32_t dwHighDateTime;
};

#endif /* COSMOPOLITAN_LIBC_NT_FILETIME_H_ */
