#ifndef COSMOPOLITAN_THIRD_PARTY_GDTOA_GDTOA_H_
#define COSMOPOLITAN_THIRD_PARTY_GDTOA_GDTOA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

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

char *dtoa(double d, int mode, int ndigits, int *decpt, int *sign, char **rve);
char *gdtoa(const FPI *fpi, int be, unsigned *bits, int *kindp, int mode,
            int ndigits, int *decpt, char **rve);
void freedtoa(char *);

float strtof(const char *, char **);
double strtod(const char *, char **);
int strtodg(const char *, char **, const FPI *, int *, unsigned *);
long double strtold(const char *, char **);

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

#if 1
int strtodI(const char *, char **, double *);
int strtopd(const char *, char **, double *);
int strtopdd(const char *, char **, double *);
int strtopf(const char *, char **, float *);
int strtopQ(const char *, char **, void *);
int strtopx(const char *, char **, void *);
int strtopxL(const char *, char **, void *);
#else
#define strtopd(s, se, x)  strtord(s, se, 1, x)
#define strtopdd(s, se, x) strtordd(s, se, 1, x)
#define strtopf(s, se, x)  strtorf(s, se, 1, x)
#define strtopQ(s, se, x)  strtorQ(s, se, 1, x)
#define strtopx(s, se, x)  strtorx(s, se, 1, x)
#define strtopxL(s, se, x) strtorxL(s, se, 1, x)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_GDTOA_GDTOA_H_ */
