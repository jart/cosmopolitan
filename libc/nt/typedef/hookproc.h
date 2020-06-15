#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_HOOKPROC_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_HOOKPROC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef intptr_t (*NtHookProc)(int code, uintptr_t wParam, intptr_t lParam);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_HOOKPROC_H_ */
