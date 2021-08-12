#ifndef CONVOLUTE_H
#define CONVOLUTE_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"
/* clang-format off */

/* Internal header file: all symbols have local scope in the DSO */
MPD_PRAGMA(MPD_HIDE_SYMBOLS_START)

#define SIX_STEP_THRESHOLD 4096

int fnt_convolute(mpd_uint_t *c1, mpd_uint_t *c2, mpd_size_t n, int modnum);
int fnt_autoconvolute(mpd_uint_t *c1, mpd_size_t n, int modnum);

MPD_PRAGMA(MPD_HIDE_SYMBOLS_END) /* restore previous scope rules */

#endif
