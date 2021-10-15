#ifndef COSMOPOLITAN_LIBC_RUNTIME_STACK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_STACK_H_
#include "ape/config.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Tunes APE stack maximum size.
 *
 * This defaults to `STACKSIZE`. The bottom-most page will be protected
 * to ensure your stack does not magically grow beyond this value. It's
 * possible to detect stack overflows, by calling `ShowCrashReports()`.
 *
 * If you want to know how much stack your programs needs, then
 *
 *     STATIC_YOINK("stack_usage_logging");
 *
 * will install an atexit() handler that appends to `o/$MODE/stack.log`
 *
 * @see libc/sysv/systemfive.S
 * @see ape/ape.lds
 */
#define STATIC_STACK_SIZE(BYTES) \
  STATIC_SYMBOL("ape_stack_memsz", _STACK_STRINGIFY(BYTES) _STACK_EXTRA)

/**
 * Tunes APE stack virtual address.
 *
 * This defaults to `0x700000000000 - STACKSIZE`. The value defined by
 * this macro will be respected, with two exceptions: (1) in MODE=tiny
 * the operating system provided stack is used instead and (2) Windows
 * Seven doesn't support 64-bit addresses so 0x10000000 - GetStackSize
 * is used instead.
 *
 * @see libc/sysv/systemfive.S
 * @see libc/nt/winmain.greg.c
 * @see ape/ape.lds
 */
#define STATIC_STACK_ADDR(ADDR) \
  STATIC_SYMBOL("ape_stack_vaddr", _STACK_STRINGIFY(ADDR))

#define _STACK_STRINGIFY(ADDR) #ADDR

#if IsAsan()
#define _STACK_EXTRA "*2"
#else
#define _STACK_EXTRA ""
#endif

#if defined(__GNUC__) && defined(__ELF__)
COSMOPOLITAN_C_START_

extern char ape_stack_memsz[] __attribute__((__weak__));

#define GetStackSize() ((uintptr_t)ape_stack_memsz)

/**
 * Returns address of bottom of stack.
 */
#define GetStackAddr(ADDEND) \
  (((intptr_t)__builtin_frame_address(0) & -GetStackSize()) + (ADDEND))

/**
 * Returns preferred bottom address of stack.
 */
#define GetStaticStackAddr(ADDEND)                               \
  ({                                                             \
    intptr_t vAddr;                                              \
    if (!IsWindows() || NtGetVersion() >= kNtVersionWindows10) { \
      asm(".weak\tape_stack_vaddr\n\t"                           \
          "movabs\t%1+ape_stack_vaddr,%0"                        \
          : "=r"(vAddr)                                          \
          : "i"(ADDEND));                                        \
    } else {                                                     \
      vAddr = 0x10000000;                                        \
    }                                                            \
    vAddr;                                                       \
  })

COSMOPOLITAN_C_END_
#endif /* GNU ELF */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_STACK_H_ */
