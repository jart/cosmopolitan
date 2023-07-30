#ifndef COSMOPOLITAN_LIBC_INTRIN_ASANCODES_H_
#define COSMOPOLITAN_LIBC_INTRIN_ASANCODES_H_

#define kAsanScale           3
#define kAsanMagic           0x7fff8000
#define kAsanNullPage        -1  /* ∅ 0xff */
#define kAsanProtected       -2  /* P 0xfe */
#define kAsanHeapFree        -3  /* F 0xfd */
#define kAsanHeapRelocated   -4  /* R 0xfc */
#define kAsanAllocaOverrun   -5  /* 𝑂 0xfb */
#define kAsanHeapUnderrun    -6  /* U 0xfa */
#define kAsanHeapOverrun     -7  /* O 0xf9 */
#define kAsanStackUnscoped   -8  /* s 0xf8 */
#define kAsanStackOverflow   -9  /* ! 0xf7 */
#define kAsanGlobalOrder     -10 /* I 0xf6 */
#define kAsanStackFree       -11 /* r 0xf5 */
#define kAsanStackPartial    -12 /* p 0xf4 */
#define kAsanStackOverrun    -13 /* o 0xf3 */
#define kAsanStackMiddle     -14 /* m 0xf2 */
#define kAsanStackUnderrun   -15 /* u 0xf1 */
#define kAsanAllocaUnderrun  -16 /* 𝑈 0xf0 */
#define kAsanUnmapped        -17 /* M 0xef */
#define kAsanGlobalRedzone   -18 /* G 0xee */
#define kAsanGlobalGone      -19 /* 𝐺 0xed */
#define kAsanGlobalUnderrun  -20 /* μ 0xec */
#define kAsanGlobalOverrun   -21 /* Ω 0xeb */
#define kAsanMmapSizeOverrun -22 /* Z 0xea */

#endif /* COSMOPOLITAN_LIBC_INTRIN_ASANCODES_H_ */
