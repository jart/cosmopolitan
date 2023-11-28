#ifndef COSMOPOLITAN_DSP_TTY_INTERNAL_H_
#define COSMOPOLITAN_DSP_TTY_INTERNAL_H_
#include "dsp/tty/quant.h"
#include "dsp/tty/ttyrgb.h"
#include "third_party/intel/xmmintrin.internal.h"
COSMOPOLITAN_C_START_

struct TtyRgb rgb2tty24f_(ttyrgb_m128);
struct TtyRgb rgb2ttyf2i_(ttyrgb_m128);
struct TtyRgb rgb2ttyi2f_(int, int, int);
struct TtyRgb rgb2ansi_(int, int, int);
struct TtyRgb rgb2ansihash_(int, int, int);
struct TtyRgb rgb2xterm24_(int, int, int);
struct TtyRgb rgb2xterm256gray_(ttyrgb_m128);
struct TtyRgb tty2rgb_(struct TtyRgb);
struct TtyRgb tty2rgb24_(struct TtyRgb);
ttyrgb_m128 tty2rgbf_(struct TtyRgb);
ttyrgb_m128 tty2rgbf24_(struct TtyRgb);

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
#endif /* COSMOPOLITAN_DSP_TTY_INTERNAL_H_ */
