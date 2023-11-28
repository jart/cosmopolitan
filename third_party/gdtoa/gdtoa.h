#ifndef COSMOPOLITAN_THIRD_PARTY_GDTOA_GDTOA_H_
#define COSMOPOLITAN_THIRD_PARTY_GDTOA_GDTOA_H_
COSMOPOLITAN_C_START_

#define dtoa      __dtoa
#define gdtoa     __gdtoa
#define strtodg   __strtodg
#define freedtoa  __freedtoa
#define g_ddfmt   __g_ddfmt
#define g_ddfmt_p __g_ddfmt_p
#define g_dfmt    __g_dfmt
#define g_dfmt_p  __g_dfmt_p
#define g_ffmt    __g_ffmt
#define g_ffmt_p  __g_ffmt_p
#define g_Qfmt    __g_Qfmt
#define g_Qfmt_p  __g_Qfmt_p
#define g_xfmt    __g_xfmt
#define g_xfmt_p  __g_xfmt_p
#define g_xLfmt   __g_xLfmt
#define g_xLfmt_p __g_xLfmt_p
#define strtoId   __strtoId
#define strtoIdd  __strtoIdd
#define strtoIf   __strtoIf
#define strtoIQ   __strtoIQ
#define strtoIx   __strtoIx
#define strtoIxL  __strtoIxL
#define strtord   __strtord
#define strtordd  __strtordd
#define strtorf   __strtorf
#define strtorQ   __strtorQ
#define strtorx   __strtorx
#define strtorxL  __strtorxL
#define strtodI   __strtodI
#define strtopd   __strtopd
#define strtopdd  __strtopdd
#define strtopf   __strtopf
#define strtopQ   __strtopQ
#define strtopx   __strtopx
#define strtopxL  __strtopxL

/**
 * Configures g_*fmt()
 *
 * @param ic
 * 	0 ==> Infinity or NaN
 * 	1 ==> infinity or nan
 * 	2 ==> INFINITY or NAN
 * 	3 ==> Inf or NaN
 * 	4 ==> inf or nan
 * 	5 ==> INF or NAN
 * @param ic determines if NaNs are rendered as NaN(...)
 * 	0 ==> no
 * 	1 ==> yes
 * 	2 ==> no for default NaN values; yes otherwise
 * @param ns determines sign of NaN values reported
 * 	0 ==> distinguish NaN and -NaN
 * 	1 ==> report both as NaN
 */
#define NIK(ic, nb, ns) (ic + 6 * (nb + 3 * ns))

enum {
  /* return values from strtodg */
  STRTOG_Zero = 0,
  STRTOG_Normal = 1,
  STRTOG_Denormal = 2,
  STRTOG_Infinite = 3,
  STRTOG_NaN = 4,
  STRTOG_NaNbits = 5,
  STRTOG_NoNumber = 6,
  STRTOG_Retmask = 7,
  /* The following may be or-ed into one of the above values. */
  STRTOG_Neg = 0x08,    /* does not affect STRTOG_Inexlo or STRTOG_Inexhi */
  STRTOG_Inexlo = 0x10, /* returned result rounded toward zero */
  STRTOG_Inexhi = 0x20, /* returned result rounded away from zero */
  STRTOG_Inexact = 0x30,
  STRTOG_Underflow = 0x40,
  STRTOG_Overflow = 0x80
};

typedef struct FPI {
  int nbits;
  int emin;
  int emax;
  int rounding;
  int sudden_underflow;
  int int_max;
} FPI;

enum {
  /* FPI.rounding values: same as FLT_ROUNDS */
  FPI_Round_zero = 0,
  FPI_Round_near = 1,
  FPI_Round_up = 2,
  FPI_Round_down = 3
};

char *dtoa(double, int, int, int *, int *, char **);
char *gdtoa(const FPI *, int, unsigned *, int *, int, int, int *, char **);
int strtodg(const char *, char **, const FPI *, int *, unsigned *);
void freedtoa(char *);

char *g_ddfmt(char *, double *, int, size_t);
char *g_ddfmt_p(char *, double *, int, size_t, int);
char *g_dfmt(char *, double *, int, size_t);
char *g_dfmt_p(char *, double *, int, size_t, int);
char *g_ffmt(char *, float *, int, size_t);
char *g_ffmt_p(char *, float *, int, size_t, int);
char *g_Qfmt(char *, void *, int, size_t);
char *g_Qfmt_p(char *, void *, int, size_t, int);
char *g_xfmt(char *, void *, int, size_t);
char *g_xfmt_p(char *, void *, int, size_t, int);
char *g_xLfmt(char *, void *, int, size_t);
char *g_xLfmt_p(char *, void *, int, size_t, int);

int strtoId(const char *, char **, double *, double *);
int strtoIdd(const char *, char **, double *, double *);
int strtoIf(const char *, char **, float *, float *);
int strtoIQ(const char *, char **, void *, void *);
int strtoIx(const char *, char **, void *, void *);
int strtoIxL(const char *, char **, void *, void *);
int strtord(const char *, char **, int, double *);
int strtordd(const char *, char **, int, double *);
int strtorf(const char *, char **, int, float *);
int strtorQ(const char *, char **, int, void *);
int strtorx(const char *, char **, int, void *);
int strtorxL(const char *, char **, int, void *);

int strtodI(const char *, char **, double *);
int strtopd(const char *, char **, double *);
int strtopdd(const char *, char **, double *);
int strtopf(const char *, char **, float *);
int strtopQ(const char *, char **, void *);
int strtopx(const char *, char **, void *);
int strtopxL(const char *, char **, void *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GDTOA_GDTOA_H_ */
