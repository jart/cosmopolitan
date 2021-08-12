#ifndef MPALLOC_H
#define MPALLOC_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"
/* clang-format off */

/* Internal header file: all symbols have local scope in the DSO */
MPD_PRAGMA(MPD_HIDE_SYMBOLS_START)

int mpd_switch_to_dyn(mpd_t *result, mpd_ssize_t size, uint32_t *status);
int mpd_switch_to_dyn_zero(mpd_t *result, mpd_ssize_t size, uint32_t *status);
int mpd_realloc_dyn(mpd_t *result, mpd_ssize_t size, uint32_t *status);

MPD_PRAGMA(MPD_HIDE_SYMBOLS_END) /* restore previous scope rules */

#endif
