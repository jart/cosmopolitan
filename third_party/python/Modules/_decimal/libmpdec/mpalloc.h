#ifndef MPALLOC_H
#define MPALLOC_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"

int mpd_switch_to_dyn(mpd_t *, mpd_ssize_t, uint32_t *);
int mpd_switch_to_dyn_zero(mpd_t *, mpd_ssize_t, uint32_t *);
int mpd_realloc_dyn(mpd_t *, mpd_ssize_t, uint32_t *);

#endif
