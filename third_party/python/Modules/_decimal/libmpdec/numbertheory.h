#ifndef NUMBER_THEORY_H
#define NUMBER_THEORY_H
#include "third_party/python/Modules/_decimal/libmpdec/constants.h"
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"
/* clang-format off */

/* Internal header file: all symbols have local scope in the DSO */
MPD_PRAGMA(MPD_HIDE_SYMBOLS_START)

/* transform parameters */
struct fnt_params {
    int modnum;
    mpd_uint_t modulus;
    mpd_uint_t kernel;
    mpd_uint_t wtable[];
};

mpd_uint_t _mpd_getkernel(mpd_uint_t n, int sign, int modnum);
struct fnt_params *_mpd_init_fnt_params(mpd_size_t n, int sign, int modnum);
void _mpd_init_w3table(mpd_uint_t w3table[3], int sign, int modnum);

#ifdef PPRO
static inline void
ppro_setmodulus(int modnum, mpd_uint_t *umod, double *dmod, uint32_t dinvmod[3])
{
    *dmod = *umod =  mpd_moduli[modnum];
    dinvmod[0] = mpd_invmoduli[modnum][0];
    dinvmod[1] = mpd_invmoduli[modnum][1];
    dinvmod[2] = mpd_invmoduli[modnum][2];
}
#else
static inline void
std_setmodulus(int modnum, mpd_uint_t *umod)
{
    *umod =  mpd_moduli[modnum];
}
#endif

MPD_PRAGMA(MPD_HIDE_SYMBOLS_END) /* restore previous scope rules */

#endif
