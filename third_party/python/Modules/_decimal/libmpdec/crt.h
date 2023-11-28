#ifndef CRT_H
#define CRT_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"

/* Internal header file: all symbols have local scope in the DSO */

void crt3(mpd_uint_t *x1, mpd_uint_t *x2, mpd_uint_t *x3, mpd_size_t nmemb);


#endif
