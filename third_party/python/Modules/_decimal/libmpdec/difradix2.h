#ifndef DIF_RADIX2_H
#define DIF_RADIX2_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"
#include "third_party/python/Modules/_decimal/libmpdec/numbertheory.h"
/* clang-format off */

/* Internal header file: all symbols have local scope in the DSO */
MPD_PRAGMA(MPD_HIDE_SYMBOLS_START)

void fnt_dif2(mpd_uint_t a[], mpd_size_t n, struct fnt_params *tparams);

MPD_PRAGMA(MPD_HIDE_SYMBOLS_END) /* restore previous scope rules */

#endif
