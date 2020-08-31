#ifndef COSMOPOLITAN_LIBC_NT_THUNK_MSABI_H_
#define COSMOPOLITAN_LIBC_NT_THUNK_MSABI_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Defines function as using Microsoft x64 calling convention.
 *
 * This can be used to define prototypes that allow modern compilers to
 * generate code that calls MS ABI functions directly, without needing
 * to jump through the assembly thunks.
 */
#if __GNUC__ * 100 + __GNUC_MINOR__ >= 408 || \
    (__has_attribute(__ms_abi__) || defined(__llvm__))
#define __msabi __attribute__((__ms_abi__))
#endif

/**
 * Returns true if header should provide MS-ABI overrides.
 */
#ifndef ShouldUseMsabiAttribute
#if defined(__msabi) && defined(NDEBUG) && !defined(__STRICT_ANSI__)
#define ShouldUseMsabiAttribute() 1
#else
#define ShouldUseMsabiAttribute() 0
#endif
#endif

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_THUNK_MSABI_H_ */
