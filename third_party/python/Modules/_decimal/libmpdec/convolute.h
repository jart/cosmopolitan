#ifndef CONVOLUTE_H
#define CONVOLUTE_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"

#define SIX_STEP_THRESHOLD 4096

int fnt_convolute(mpd_uint_t *, mpd_uint_t *, mpd_size_t, int);
int fnt_autoconvolute(mpd_uint_t *, mpd_size_t, int);

#endif
