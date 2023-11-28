#ifndef SIX_STEP_H
#define SIX_STEP_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"

/* Internal header file: all symbols have local scope in the DSO */

int six_step_fnt(mpd_uint_t *a, mpd_size_t n, int modnum);
int inv_six_step_fnt(mpd_uint_t *a, mpd_size_t n, int modnum);


#endif
