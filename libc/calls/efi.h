#ifndef COSMOPOLITAN_LIBC_CALLS_EFI_H_
#define COSMOPOLITAN_LIBC_CALLS_EFI_H_
#include "libc/nt/efi.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern EFI_HANDLE __efi_image_handle;
extern EFI_SYSTEM_TABLE *__efi_system_table;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_EFI_H_ */
