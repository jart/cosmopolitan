#ifdef _COSMO_SOURCE
#ifndef COSMOPOLITAN_LIBC_RUNTIME_STACK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_STACK_H_
#include "libc/runtime/runtime.h"

/**
 * Returns preferred size and alignment of thread stack.
 */
#ifndef MODE_DBG
#define GetStackSize() 81920
#else
#define GetStackSize() 163840
#endif

/**
 * Returns preferred stack guard size.
 *
 * This is the max cpu page size of supported architectures.
 */
#define GetGuardSize() __pagesize

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
#define STATIC_EXEC_STACK() _STACK_SYMBOL("ape_stack_pf", "7")

#define _STACK_STRINGIFY(ADDR) #ADDR
#define _STACK_SYMBOL(NAME, VALUE)       \
  __asm__(".equ\t" NAME "," VALUE "\n\t" \
          ".globl\t" NAME)

#ifdef __SANITIZE_ADDRESS__
#define _STACK_EXTRA "*2"
#else
#define _STACK_EXTRA ""
#endif

#if defined(__GNUC__) && defined(__ELF__)
COSMOPOLITAN_C_START_

extern char ape_stack_prot[] __attribute__((__weak__));
extern char ape_stack_memsz[] __attribute__((__weak__));

uintptr_t GetStackBottom(void) pureconst;

#define GetStaticStackSize() ((uintptr_t)ape_stack_memsz)

/**
 * Extends stack memory by poking large allocations.
 *
 * This can be particularly useful depending on how your system
 * implements guard pages. For example, Windows can make stacks
 * that aren't fully committed, in which case there's only 4096
 * bytes of grows-down guard pages made by portable executable.
 * If you alloca() more memory than that, you should call this,
 * since it'll not only ensure stack overflows are detected, it
 * will also trigger the stack to grow down safely.
 */
forceinline void CheckLargeStackAllocation(void *p, ssize_t n) {
  for (; n > 0; n -= 4096)
    ((volatile char *)p)[n - 1] = 0;
}

void *NewCosmoStack(void) vallocesque;
int FreeCosmoStack(void *) libcesque;

/**
 * Tunes stack size of main thread on Windows.
 *
 * On UNIX systems use `RLIMIT_STACK` to tune the main thread size.
 */
#define STATIC_STACK_SIZE(BYTES) \
  _STACK_SYMBOL("ape_stack_memsz", _STACK_STRINGIFY(BYTES) _STACK_EXTRA)

/**
 * Tunes main thread stack address on Windows.
 */
#define STATIC_STACK_ADDR(ADDR) \
  _STACK_SYMBOL("ape_stack_vaddr", _STACK_STRINGIFY(ADDR))

#ifdef __x86_64__
/**
 * Returns preferred bottom address of main thread stack.
 *
 * On UNIX systems we favor the system provided stack, so this only
 * really applies to Windows. It's configurable at link time. It is
 * needed because polyfilling fork requires that we know, precicely
 * where the stack memory begins and ends.
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
#endif

#define GetStackPointer()           \
  ({                                \
    uintptr_t __sp;                 \
    __asm__(__mov_sp : "=r"(__sp)); \
    __sp;                           \
  })

#ifdef __x86_64__
#define __mov_sp "mov\t%%rsp,%0"
#elif defined(__aarch64__)
#define __mov_sp "mov\t%0,sp"
#endif

COSMOPOLITAN_C_END_
#endif /* GNU ELF */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_STACK_H_ */
#endif /* _COSMO_SOURCE */
