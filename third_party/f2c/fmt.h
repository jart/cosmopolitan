#ifndef COSMOPOLITAN_THIRD_PARTY_F2C_FMT_H_
#define COSMOPOLITAN_THIRD_PARTY_F2C_FMT_H_
#include "third_party/f2c/f2c.h"
#include "third_party/f2c/fio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define RET1   1
#define REVERT 2
#define GOTO   3
#define X      4
#define SLASH  5
#define STACK  6
#define I      7
#define ED     8
#define NED    9
#define IM     10
#define APOS   11
#define H      12
#define TL     13
#define TR     14
#define T      15
#define COLON  16
#define S      17
#define SP     18
#define SS     19
#define P      20
#define BN     21
#define BZ     22
#define F      23
#define E      24
#define EE     25
#define D      26
#define G      27
#define GE     28
#define L      29
#define A      30
#define AW     31
#define O      32
#define NONL   33
#define OM     34
#define Z      35
#define ZM     36

struct syl {
  int op;
  int p1;
  union {
    int i[2];
    char *s;
  } p2;
};

typedef union {
  real pf;
  doublereal pd;
} ufloat;

typedef union {
  short is;
  signed char ic;
  integer il;
#ifdef Allow_TYQUAD
  longint ili;
#endif
} Uint;

void fmt_bg(void);
int pars_f(const char *);
int rd_ed(struct syl *, char *, ftnlen);
int rd_ned(struct syl *);
int signbit_f2c(double *);
int w_ed(struct syl *, char *, ftnlen);
int w_ned(struct syl *);
int wrt_E(ufloat *, int, int, int, ftnlen);
int wrt_F(ufloat *, int, int, ftnlen);
int wrt_L(Uint *, int, ftnlen);

extern const char *f__fmtbuf;
extern int (*f__doed)(struct syl *, char *, ftnlen), (*f__doned)(struct syl *);
extern int (*f__dorevert)(void);
extern int f__pc, f__parenlvl, f__revloc;
extern flag f__cblank, f__cplus, f__workdone, f__nonl;
extern int f__scale;

#define GET(x) \
  if ((x = (*f__getn)()) < 0) return (x)
#define VAL(x) (x != '\n' ? x : ' ')
#define PUT(x) (*f__putn)(x)

#undef TYQUAD
#ifndef Allow_TYQUAD
#undef longint
#define longint long
#else
#define TYQUAD 14
#endif

char *f__icvt(longint, int *, int *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_F2C_FMT_H_ */
