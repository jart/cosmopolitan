#ifndef COSMOPOLITAN_LIBC_RUNTIME_STACK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_STACK_H_
#include "ape/config.h"
#include "libc/dce.h"
#include "libc/nt/version.h"
#include "libc/runtime/runtime.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Tunes APE stack maximum size.
 *
 * This defaults to `STACKSIZE`. The bottom-most page will be protected
 * to ensure your stack does not magically grow beyond this value. It's
 * possible to detect stack overflows, by calling `ShowCrashReports()`.
 * Your stack size must be a power of two; the linker will check this.
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
 * This value must be aligned according to your stack size, and that's
 * checked by your linker script. This defaults to `0x700000000000` so
 *
 * 1. It's easy to see how close you are to the bottom
 * 2. The linker script error is unlikely to happen
 *
 * This macro will be respected, with two exceptions
 *
 * 1. In MODE=tiny the operating system provided stack is used instead
 * 2. Windows 7 doesn't support 64-bit addresses, so we'll instead use
 *    `0x10000000 - GetStackSize()` as the stack address
 *
 * @see libc/sysv/systemfive.S
 * @see libc/nt/winmain.greg.c
 * @see ape/ape.lds
 */
#define STATIC_STACK_ADDR(ADDR) \
  STATIC_SYMBOL("ape_stack_vaddr", _STACK_STRINGIFY(ADDR))

/**
 * Makes program stack executable if declared, e.g.
 *
 *     STATIC_EXEC_STACK();
 *     int main() {
 *       char code[16] = {
 *           0x55,                          // push %rbp
 *           0xb8, 0007, 0x00, 0x00, 0x00,  // mov  $7,%eax
 *           0x5d,                          // push %rbp
 *           0xc3,                          // ret
 *       };
 *       int (*func)(void) = (void *)code;
 *       printf("result %d should be 7\n", func());
 *     }
 */
#define STATIC_EXEC_STACK() STATIC_SYMBOL("ape_stack_pf", "7")

#define _STACK_STRINGIFY(ADDR) #ADDR

#if IsAsan()
#define _STACK_EXTRA "*2"
#else
#define _STACK_EXTRA ""
#endif

#if defined(__GNUC__) && defined(__ELF__) && !defined(__STRICT_ANSI__)
COSMOPOLITAN_C_START_

extern char ape_stack_prot[] __attribute__((__weak__));
extern char ape_stack_memsz[] __attribute__((__weak__));
extern char ape_stack_align[] __attribute__((__weak__));

/**
 * Returns size of stack, which is always a two power.
 */
#define GetStackSize() ((uintptr_t)ape_stack_memsz)

/**
 * Returns address of bottom of stack.
 *
 * This takes into consideration threads and sigaltstack. This is
 * implemented as a fast pure expression, since we're able to make the
 * assumption that stack sizes are two powers and aligned. This is
 * thanks to (1) the linker script checks the statically chosen sizes,
 * and (2) the mmap() address picker will choose aligned addresses when
 * the provided size is a two power.
 */
#define GetStackAddr() \
  (((intptr_t)__builtin_frame_address(0) - 1) & -GetStackSize())

/**
 * Returns preferred bottom address of stack.
 *
 * This is the stakc address of the main process. The only time that
 * isn't guaranteed to be the case is in MODE=tiny, since it doesn't
 * link the code for stack creation at startup. This generally isn't
 * problematic, since MODE=tiny doesn't use any of the runtime codes
 * which want the stack to be cheaply knowable, e.g. ftrace, kprintf
 */
#define GetStaticStackAddr(ADDEND)          \
  ({                                        \
    intptr_t vAddr;                         \
    __asm__(".weak\tape_stack_vaddr\n\t"    \
            "movabs\t%1+ape_stack_vaddr,%0" \
            : "=r"(vAddr)                   \
            : "i"(ADDEND));                 \
    vAddr;                                  \
  })

/**
 * Returns true if at least `n` bytes of stack are available.
 */
#define HaveStackMemory(n) \
  (IsTiny() ||             \
   (intptr_t)__builtin_frame_address(0) >= GetStackAddr() + GUARDSIZE + (n))

forceinline void CheckLargeStackAllocation(void *p, ssize_t n) {
  for (; n > 0; n -= 4096) {
    ((char *)p)[n - 1] = 0;
  }
}

COSMOPOLITAN_C_END_
#endif /* GNU ELF */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_STACK_H_ */
