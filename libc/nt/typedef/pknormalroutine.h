#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_PKNORMALROUTINE_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_PKNORMALROUTINE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef void (*NtPkNormalRoutine)(void *NormalContext, void *SystemArgument1,
                                  void *SystemArgument2);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_PKNORMALROUTINE_H_ */
