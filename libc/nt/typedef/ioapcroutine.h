#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_IOAPCROUTINE_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_IOAPCROUTINE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtIoStatusBlock;

typedef void (*NtIoApcRoutine)(void *ApcContext,
                               struct NtIoStatusBlock *IoStatusBlock,
                               uint32_t Reserved);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_IOAPCROUTINE_H_ */
