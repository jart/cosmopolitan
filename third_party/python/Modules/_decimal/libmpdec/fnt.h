#ifndef FNT_H
#define FNT_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"

/* Internal header file: all symbols have local scope in the DSO */

int std_fnt(mpd_uint_t[], mpd_size_t, int);
int std_inv_fnt(mpd_uint_t[], mpd_size_t, int);

#endif
