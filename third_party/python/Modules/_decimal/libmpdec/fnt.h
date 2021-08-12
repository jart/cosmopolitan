#ifndef FNT_H
#define FNT_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"
/* clang-format off */

/* Internal header file: all symbols have local scope in the DSO */
MPD_PRAGMA(MPD_HIDE_SYMBOLS_START)

int std_fnt(mpd_uint_t a[], mpd_size_t n, int modnum);
int std_inv_fnt(mpd_uint_t a[], mpd_size_t n, int modnum);

MPD_PRAGMA(MPD_HIDE_SYMBOLS_END) /* restore previous scope rules */

#endif
