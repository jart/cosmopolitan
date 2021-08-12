#ifndef CRT_H
#define CRT_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"
/* clang-format off */

/* Internal header file: all symbols have local scope in the DSO */
MPD_PRAGMA(MPD_HIDE_SYMBOLS_START)

void crt3(mpd_uint_t *x1, mpd_uint_t *x2, mpd_uint_t *x3, mpd_size_t nmemb);

MPD_PRAGMA(MPD_HIDE_SYMBOLS_END) /* restore previous scope rules */

#endif
