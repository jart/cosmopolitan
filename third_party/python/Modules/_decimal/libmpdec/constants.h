#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"

#define MULMOD(a, b) x64_mulmod(a, b, umod)
#define MULMOD2C(a0, a1, w) x64_mulmod2c(a0, a1, w, umod)
#define MULMOD2(a0, b0, a1, b1) x64_mulmod2(a0, b0, a1, b1, umod)
#define POWMOD(base, exp) x64_powmod(base, exp, umod)
#define SETMODULUS(modnum) std_setmodulus(modnum, &umod)
#define SIZE3_NTT(x0, x1, x2, w3table) std_size3_ntt(x0, x1, x2, w3table, umod)

/* PentiumPro (or later) gcc inline asm */
extern const float MPD_TWO63;
extern const uint32_t mpd_invmoduli[3][3];

enum {P1, P2, P3};

extern const mpd_uint_t mpd_moduli[];
extern const mpd_uint_t mpd_roots[];
extern const mpd_size_t mpd_bits[];
extern const mpd_uint_t mpd_pow10[];

extern const mpd_uint_t INV_P1_MOD_P2;
extern const mpd_uint_t INV_P1P2_MOD_P3;
extern const mpd_uint_t LH_P1P2;
extern const mpd_uint_t UH_P1P2;

#endif /* CONSTANTS_H */
