#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_KD_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_KD_H_
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

#define GIO_FONT      GIO_FONT
#define PIO_FONT      PIO_FONT
#define GIO_FONTX     GIO_FONTX
#define PIO_FONTX     PIO_FONTX
#define PIO_FONT8x8   PIO_FONT8x8
#define PIO_FONT8x14  PIO_FONT8x14
#define PIO_FONT8x16  PIO_FONT8x14
#define PIO_VFONT     PIO_VFONT
#define PIO_FONTRESET PIO_FONTRESET
#define KDFONTOP      KDFONTOP

#ifdef _BSD_SOURCE
#define PIO_VFONT_DEFAULT PIO_FONTRESET
#endif

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_KD_H_ */
