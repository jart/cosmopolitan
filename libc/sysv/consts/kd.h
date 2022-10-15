#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_KD_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_KD_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const uint64_t GIO_FONT;
extern const uint64_t PIO_FONT;
extern const uint64_t GIO_FONTX;
extern const uint64_t PIO_FONTX;
extern const uint64_t PIO_FONT8x8;
extern const uint64_t PIO_FONT8x14;
extern const uint64_t PIO_FONT8x16;
extern const uint64_t PIO_VFONT;
extern const uint64_t PIO_FONTRESET;
extern const uint64_t KDFONTOP;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define GIO_FONT      SYMBOLIC(GIO_FONT)
#define PIO_FONT      SYMBOLIC(PIO_FONT)
#define GIO_FONTX     SYMBOLIC(GIO_FONTX)
#define PIO_FONTX     SYMBOLIC(PIO_FONTX)
#define PIO_FONT8x8   SYMBOLIC(PIO_FONT8x8)
#define PIO_FONT8x14  SYMBOLIC(PIO_FONT8x14)
#define PIO_FONT8x16  SYMBOLIC(PIO_FONT8x14)
#define PIO_VFONT     SYMBOLIC(PIO_VFONT)
#define PIO_FONTRESET SYMBOLIC(PIO_FONTRESET)
#define KDFONTOP      SYMBOLIC(KDFONTOP)

#ifdef _BSD_SOURCE
#define PIO_VFONT_DEFAULT PIO_FONTRESET
#endif

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_KD_H_ */
