#ifndef COSMOPOLITAN_LIBC_NT_SHELL32_H_
#define COSMOPOLITAN_LIBC_NT_SHELL32_H_
#include "libc/nt/struct/guid.h"
COSMOPOLITAN_C_START_

int32_t SHGetKnownFolderPath(const struct NtGuid *rfid, uint32_t dwFlags,
                             intptr_t opt_hToken, char16_t **out_ppszPath);
void CoTaskMemFree(void *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_SHELL32_H_ */
