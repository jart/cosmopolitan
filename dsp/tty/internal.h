#ifndef COSMOPOLITAN_DSP_TTY_INTERNAL_H_
#define COSMOPOLITAN_DSP_TTY_INTERNAL_H_
#include "dsp/tty/ttyrgb.h"
#include "libc/bits/xmmintrin.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct TtyRgb rgb2tty24f_(__m128);
struct TtyRgb rgb2ttyf2i_(__m128);
struct TtyRgb rgb2ttyi2f_(int, int, int);
struct TtyRgb rgb2ansi_(int, int, int);
struct TtyRgb rgb2ansihash_(int, int, int);
struct TtyRgb rgb2xterm24_(int, int, int);
struct TtyRgb rgb2xterm256gray_(__m128);
struct TtyRgb tty2rgb_(struct TtyRgb);
struct TtyRgb tty2rgb24_(struct TtyRgb);
__m128 tty2rgbf_(struct TtyRgb);
__m128 tty2rgbf24_(struct TtyRgb);

char *setbg16_(char *, struct TtyRgb);
char *setfg16_(char *, struct TtyRgb);
char *setbgfg16_(char *, struct TtyRgb, struct TtyRgb);
char *setbg256_(char *, struct TtyRgb);
char *setfg256_(char *, struct TtyRgb);
char *setbgfg256_(char *, struct TtyRgb, struct TtyRgb);
char *setbg24_(char *, struct TtyRgb);
char *setfg24_(char *, struct TtyRgb);
char *setbgfg24_(char *, struct TtyRgb, struct TtyRgb);
struct TtyRgb rgb2ansi8_(int, int, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_DSP_TTY_INTERNAL_H_ */
