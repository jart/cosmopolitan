#ifndef COSMOPOLITAN_LIBC_NT_MESSAGEBOX_H_
#define COSMOPOLITAN_LIBC_NT_MESSAGEBOX_H_
#include "libc/nt/enum/dialogresult.h"
COSMOPOLITAN_C_START_

int MessageBox(int64_t hWnd, const char16_t *lpText, const char16_t *lpCaption,
               uint32_t mbType);
int MessageBoxEx(int64_t hWnd, const char16_t *lpText,
                 const char16_t *lpCaption, uint32_t mbType,
                 uint16_t wLanguageId);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_MESSAGEBOX_H_ */
