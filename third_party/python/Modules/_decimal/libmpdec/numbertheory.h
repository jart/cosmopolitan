#ifndef NUMBER_THEORY_H
#define NUMBER_THEORY_H
#include "third_party/python/Modules/_decimal/libmpdec/constants.h"
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"

struct fnt_params {
  int modnum;
  mpd_uint_t modulus;
  mpd_uint_t kernel;
  mpd_uint_t wtable[];
};

mpd_uint_t _mpd_getkernel(mpd_uint_t, int, int);
struct fnt_params *_mpd_init_fnt_params(mpd_size_t, int, int);
void _mpd_init_w3table(mpd_uint_t[3], int, int);

static inline void std_setmodulus(int modnum, mpd_uint_t *umod) {
  *umod = mpd_moduli[modnum];
}

#endif
