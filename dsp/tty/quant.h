#ifndef DSP_TTY_QUANT_H_
#define DSP_TTY_QUANT_H_
#include "dsp/tty/ttyrgb.h"
#include "libc/assert.h"
#include "libc/limits.h"
#include "libc/str/str.h"
COSMOPOLITAN_C_START_

#define TL 0
#define TR 1
#define BL 2
#define BR 3

typedef float ttyrgb_m128 __attribute__((__vector_size__(16), __may_alias__));

typedef ttyrgb_m128 (*tty2rgbf_f)(struct TtyRgb);
typedef char *(*setbg_f)(char *, struct TtyRgb);
typedef char *(*setbgfg_f)(char *, struct TtyRgb, struct TtyRgb);
typedef char *(*setfg_f)(char *, struct TtyRgb);
typedef struct TtyRgb (*rgb2tty_f)(int, int, int);
typedef struct TtyRgb (*rgb2ttyf_f)(ttyrgb_m128);
typedef struct TtyRgb (*tty2rgb_f)(struct TtyRgb);
typedef struct TtyRgb ttypalette_t[2][8];

enum TtyQuantizationAlgorithm {
  kTtyQuantAnsi,
  kTtyQuantTrue,
  kTtyQuantXterm256,
};

enum TtyBlocksSelection {
  kTtyBlocksUnicode,
  kTtyBlocksCp437,
};

enum TtyQuantizationChannels {
  kTtyQuantGrayscale = 1,
  kTtyQuantRgb = 3,
};

struct TtyQuant {
  enum TtyQuantizationAlgorithm alg;
  enum TtyBlocksSelection blocks;
  enum TtyQuantizationChannels chans;
  unsigned min;
  unsigned max;
  setbg_f setbg;
  setfg_f setfg;
  setbgfg_f setbgfg;
  rgb2tty_f rgb2tty;
  rgb2ttyf_f rgb2ttyf;
  tty2rgb_f tty2rgb;
  tty2rgbf_f tty2rgbf;
  const ttypalette_t *palette;
};

extern const ttypalette_t kCgaPalette;
extern const ttypalette_t kXtermPalette;
extern const ttypalette_t kTangoPalette;

extern const uint8_t kXtermCubeSteps[6];
extern double g_xterm256_gamma;
extern struct TtyRgb g_ansi2rgb_[256];
extern struct TtyQuant g_ttyquant_;
extern const uint8_t kXtermXlat[2][256];
extern const uint8_t kXtermCube[6];

void ttyquantsetup(enum TtyQuantizationAlgorithm, enum TtyQuantizationChannels,
                   enum TtyBlocksSelection);

extern char *ttyraster(char *, const struct TtyRgb *, size_t, size_t,
                       struct TtyRgb, struct TtyRgb);

#ifndef ttyquant
#define ttyquant()    (&g_ttyquant_)
#define TTYQUANT()    __veil("r", &g_ttyquant_)
#define rgb2tty(...)  (ttyquant()->rgb2tty(__VA_ARGS__))
#define tty2rgb(...)  (ttyquant()->tty2rgb(__VA_ARGS__))
#define rgb2ttyf(...) (ttyquant()->rgb2ttyf(__VA_ARGS__))
#define tty2rgbf(...) (ttyquant()->tty2rgbf(__VA_ARGS__))
#define setbg(...)    (ttyquant()->setbg(__VA_ARGS__))
#define setfg(...)    (ttyquant()->setfg(__VA_ARGS__))
#define setbgfg(...)  (ttyquant()->setbgfg(__VA_ARGS__))
#endif /* ttyquant */

forceinline bool ttyeq(struct TtyRgb x, struct TtyRgb y) {
  return x.r == y.r && x.g == y.g && x.b == y.b;
}

COSMOPOLITAN_C_END_
#endif /* DSP_TTY_QUANT_H_ */
