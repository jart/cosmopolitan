#ifndef COSMOPOLITAN_LIBC_NT_MESSAGEBOX_H_
#define COSMOPOLITAN_LIBC_NT_MESSAGEBOX_H_
#include "libc/nt/enum/dialogresult.h"
#include "libc/nt/enum/messageboxtype.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

enum DialogResult MessageBox(int64_t hWnd, const char16_t *lpText,
                             const char16_t *lpCaption,
                             enum MessageBoxType uType);
enum DialogResult MessageBoxEx(int64_t hWnd, const char16_t *lpText,
                               const char16_t *lpCaption,
                               enum MessageBoxType uType, uint16_t wLanguageId);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_MESSAGEBOX_H_ */
