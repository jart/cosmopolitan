#ifndef COSMOPOLITAN_LIBC_INTRIN_NOPL_H_
#define COSMOPOLITAN_LIBC_INTRIN_NOPL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * @fileoverview Turns CALLs into NOPs that are fixupable at runtime.
 *
 * Things like lock/unlock function calls can take on average 100ms.
 * Libc needs to use internal locking pervasively in order to support
 * threads. So there's a lot of cost everywhere, even though most progs
 * don't use threads. In ANSI mode we dispatching (__threaded && lock())
 * to solve this problem, but if we write lock statements that way, it
 * adds a lot of bloat to the functions that call locking routines. So
 * what we do here is replace the CALL instruction with NOP, which keeps
 * the code just as fast as inlining, while making code size 10x tinier.
 */

#define _NOPL_PROLOGUE(SECTION)                    \
  ".section \".sort.rodata." SECTION ".1"          \
  "\",\"aG\",@progbits,\"" SECTION "\",comdat\n\t" \
  ".align\t4\n\t"                                  \
  ".type\t\"" SECTION "_start\",@object\n\t"       \
  ".globl\t\"" SECTION "_start\"\n\t"              \
  ".equ\t\"" SECTION "_start\",.\n\t"              \
  ".previous\n\t"

#define _NOPL_EPILOGUE(SECTION)                    \
  ".section \".sort.rodata." SECTION ".3"          \
  "\",\"aG\",@progbits,\"" SECTION "\",comdat\n\t" \
  ".align\t4\n\t"                                  \
  ".type\"" SECTION "_end\",@object\n\t"           \
  ".globl\t\"" SECTION "_end\"\n\t"                \
  ".equ\t\"" SECTION "_end\",.\n\t"                \
  ".previous\n\t"

#define _NOPL0(SECTION, FUNC)                                                  \
  ({                                                                           \
    asm volatile(_NOPL_PROLOGUE(SECTION) /*                                 */ \
                 _NOPL_EPILOGUE(SECTION) /*                                 */ \
                 ".section \".sort.rodata." SECTION ".2\",\"a\",@progbits\n\t" \
                 ".align\t4\n\t"                                               \
                 ".long\t353f-%a1\n\t"                                         \
                 ".previous\n353:\t"                                           \
                 "nopl\t%a0"                                                   \
                 : /* no inputs */                                             \
                 : "X"(FUNC), "X"(IMAGE_BASE_VIRTUAL)                          \
                 : "rax", "rdi", "rsi", "rdx", "rcx", "r8", "r9", "r10",       \
                   "r11", "memory", "cc");                                     \
    0;                                                                         \
  })

#define _NOPL1(SECTION, FUNC, ARG)                                             \
  ({                                                                           \
    register autotype(ARG) __arg asm("rdi") = ARG;                             \
    asm volatile(_NOPL_PROLOGUE(SECTION) /*                                 */ \
                 _NOPL_EPILOGUE(SECTION) /*                                 */ \
                 ".section \".sort.rodata." SECTION ".2\",\"a\",@progbits\n\t" \
                 ".align\t4\n\t"                                               \
                 ".long\t353f-%a2\n\t"                                         \
                 ".previous\n353:\t"                                           \
                 "nopl\t%a1"                                                   \
                 : "+D"(__arg)                                                 \
                 : "X"(FUNC), "X"(IMAGE_BASE_VIRTUAL)                          \
                 : "rax", "rsi", "rdx", "rcx", "r8", "r9", "r10", "r11",       \
                   "memory", "cc");                                            \
    0;                                                                         \
  })

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_NOPL_H_ */
