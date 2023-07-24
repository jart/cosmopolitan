#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/fenv.h"
#include "libc/str/str.h"
#include "third_party/gdtoa/gdtoa.h"

asm(".ident\t\"\\n\\n\
gdtoa (MIT License)\\n\
The author of this software is David M. Gay\\n\
Kudos go to Guy L. Steele, Jr. and Jon L. White\\n\
Copyright (C) 1997, 1998, 2000 by Lucent Technologies\"");
asm(".include \"libc/disclaimer.inc\"");

#define IEEE_Arith          1
#define IEEE_8087           1
#define Honor_FLT_ROUNDS    1
#define f_QNAN              0x7fc00000
#define d_QNAN0             0x7ff80000
#define d_QNAN1             0x0
#define Omit_Private_Memory 1
#define Check_FLT_ROUNDS    1

/* On a machine with IEEE extended-precision registers, it is
 * necessary to specify double-precision (53-bit) rounding precision
 * before invoking strtod or dtoa.  If the machine uses (the equivalent
 * of) Intel 80x87 arithmetic, the call
 *	_control87(PC_53, MCW_PC);
 * does this with many compilers.  Whether this or another call is
 * appropriate depends on the compiler; for this to work, it may be
 * necessary third_party/gdtoa/to #include "float.h" or another system-dependent
 *header file.
 */

/* strtod for IEEE-, VAX-, and IBM-arithmetic machines.
 *
 * This strtod returns a nearest machine number to the input decimal
 * string (or sets errno to ERANGE).  With IEEE arithmetic, ties are
 * broken by the IEEE round-even rule.  Otherwise ties are broken by
 * biased rounding (add half and chop).
 *
 * Inspired loosely by William D. Clinger's paper "How to Read Floating
 * Point Numbers Accurately" [Proc. ACM SIGPLAN '90, pp. 112-126].
 *
 * Modifications:
 *
 *	1. We only require IEEE, IBM, or VAX double-precision
 *		arithmetic (not IEEE double-extended).
 *	2. We get by with floating-point arithmetic in a case that
 *		Clinger missed -- when we're computing d * 10^n
 *		for a small integer d and the integer n is not too
 *		much larger than 22 (the maximum integer k for which
 *		we can represent 10^k exactly), we may be able to
 *		compute (d*10^k) * 10^(e-k) with just one roundoff.
 *	3. Rather than a bit-at-a-time adjustment of the binary
 *		result in the hard case, we use floating-point
 *		arithmetic to determine the adjustment to within
 *		one bit; only in really hard cases do we need to
 *		compute a second residual.
 *	4. Because of 3., we don't need a large table of powers of 10
 *		for ten-to-e (just some small tables, e.g. of 10^k
 *		for 0 <= k <= 22).
 */

/*
 * #define IEEE_8087 for IEEE-arithmetic machines where the least
 *	significant byte has the lowest address.
 * #define IEEE_MC68k for IEEE-arithmetic machines where the most
 *	significant byte has the lowest address.
 * #define Long int on machines with 32-bit ints and 64-bit longs.
 * #define Sudden_Underflow for IEEE-format machines without gradual
 *	underflow (i.e., that flush to zero on underflow).
 * #define IBM for IBM mainframe-style floating-point arithmetic.
 * #define VAX for VAX-style floating-point arithmetic (D_floating).
 * #define No_leftright to omit left-right logic in fast floating-point
 *	computation of dtoa and gdtoa.  This will cause modes 4 and 5 to be
 *	treated the same as modes 2 and 3 for some inputs.
 * #define Check_FLT_ROUNDS if FLT_ROUNDS can assume the values 2 or 3.
 * #define RND_PRODQUOT to use rnd_prod and rnd_quot (assembly routines
 *	that use extended-precision instructions to compute rounded
 *	products and quotients) with IBM.
 * #define ROUND_BIASED for IEEE-format with biased rounding and arithmetic
 *	that rounds toward +Infinity.
 * #define ROUND_BIASED_without_Round_Up for IEEE-format with biased
 *	rounding when the underlying floating-point arithmetic uses
 *	unbiased rounding.  This prevent using ordinary floating-point
 *	arithmetic when the result could be computed with one rounding error.
 * #define Inaccurate_Divide for IEEE-format with correctly rounded
 *	products but inaccurate quotients, e.g., for Intel i860.
 * #define NO_LONG_LONG on machines that do not have a "long long"
 *	integer type (of >= 64 bits).  On such machines, you can
 *	#define Just_16 to store 16 bits per 32-bit Long when doing
 *	high-precision integer arithmetic.  Whether this speeds things
 *	up or slows things down depends on the machine and the number
 *	being converted.  If long long is available and the name is
 *	something other than "long long", #define Llong to be the name,
 *	and if "unsigned Llong" does not work as an unsigned version of
 *	Llong, #define #ULLong to be the corresponding unsigned type.
 * #define Bad_float_h if your system lacks a float.h or if it does not
 *	define some or all of DBL_DIG, DBL_MAX_10_EXP, DBL_MAX_EXP,
 *	FLT_RADIX, FLT_ROUNDS, and DBL_MAX.
 * #define MALLOC your_malloc, where your_malloc(n) acts like malloc(n)
 *	if memory is available and otherwise does something you deem
 *	appropriate.  If MALLOC is undefined, malloc will be invoked
 *	directly -- and assumed always to succeed.  Similarly, if you
 *	want something other than the system's free() to be called to
 *	recycle memory acquired from MALLOC, #define FREE to be the
 *	name of the alternate routine.  (FREE or free is only called in
 *	pathological cases, e.g., in a gdtoa call after a gdtoa return in
 *	mode 3 with thousands of digits requested.)
 * #define Omit_Private_Memory to omit logic (added Jan. 1998) for making
 *	memory allocations from a private pool of memory when possible.
 *	When used, the private pool is PRIVATE_MEM bytes long:  2304 bytes,
 *	unless #defined to be a different length.  This default length
 *	suffices to get rid of MALLOC calls except for unusual cases,
 *	such as decimal-to-binary conversion of a very long string of
 *	digits.  When converting IEEE double precision values, the
 *	longest string gdtoa can return is about 751 bytes long.  For
 *	conversions by strtod of strings of 800 digits and all gdtoa
 *	conversions of IEEE doubles in single-threaded executions with
 *	8-byte pointers, PRIVATE_MEM >= 7400 appears to suffice; with
 *	4-byte pointers, PRIVATE_MEM >= 7112 appears adequate.
 * #define NO_INFNAN_CHECK if you do not wish to have INFNAN_CHECK
 *	#defined automatically on IEEE systems.  On such systems,
 *	when INFNAN_CHECK is #defined, strtod checks
 *	for Infinity and NaN (case insensitively).
 *	When INFNAN_CHECK is #defined and No_Hex_NaN is not #defined,
 *	strtodg also accepts (case insensitively) strings of the form
 *	NaN(x), where x is a string of hexadecimal digits (optionally
 *	preceded by 0x or 0X) and spaces; if there is only one string
 *	of hexadecimal digits, it is taken for the fraction bits of the
 *	resulting NaN; if there are two or more strings of hexadecimal
 *	digits, each string is assigned to the next available sequence
 *	of 32-bit words of fractions bits (starting with the most
 *	significant), right-aligned in each sequence.
 *	Unless GDTOA_NON_PEDANTIC_NANCHECK is #defined, input "NaN(...)"
 *	is consumed even when ... has the wrong form (in which case the
 *	"(...)" is consumed but ignored).
 * #define MULTIPLE_THREADS if the system offers preemptively scheduled
 *	multiple threads.  In this case, you must provide (or suitably
 *	#define) two locks, acquired by ACQUIRE_DTOA_LOCK(n) and freed
 *	by FREE_DTOA_LOCK(n) for n = 0 or 1.  (The second lock, accessed
 *	in pow5mult, ensures lazy evaluation of only one copy of high
 *	powers of 5; omitting this lock would introduce a small
 *	probability of wasting memory, but would otherwise be harmless.)
 *	You must also invoke freedtoa(s) to free the value s returned by
 *	dtoa.  You may do so whether or not MULTIPLE_THREADS is #defined.
 *	When MULTIPLE_THREADS is #defined, source file misc.c provides
 *		void set_max_gdtoa_threads(unsigned int n);
 *	and expects
 *		unsigned int dtoa_get_threadno(void);
 *	to be available (possibly provided by
 *		#define dtoa_get_threadno omp_get_thread_num
 *	if OpenMP is in use or by
 *		#define dtoa_get_threadno pthread_self
 *	if Pthreads is in use), to return the current thread number.
 *	If set_max_dtoa_threads(n) was called and the current thread
 *	number is k with k < n, then calls on ACQUIRE_DTOA_LOCK(...) and
 *	FREE_DTOA_LOCK(...) are avoided; instead each thread with thread
 *	number < n has a separate copy of relevant data structures.
 *	After set_max_dtoa_threads(n), a call set_max_dtoa_threads(m)
 *	with m <= n has has no effect, but a call with m > n is honored.
 *	Such a call invokes REALLOC (assumed to be "realloc" if REALLOC
 *	is not #defined) to extend the size of the relevant array.
 * #define IMPRECISE_INEXACT if you do not care about the setting of
 *	the STRTOG_Inexact bits in the special case of doing IEEE double
 *	precision conversions (which could also be done by the strtod in
 *	dtoa.c).
 * #define NO_HEX_FP to disable recognition of C9x's hexadecimal
 *	floating-point constants.
 * #define -DNO_ERRNO to suppress setting errno (in strtod.c and
 *	strtodg.c).
 * #define USE_LOCALE to use the current locale's decimal_point value.
 */

#ifndef Long
#define Long int
#endif
#ifndef ULong
typedef unsigned Long ULong;
#endif
#ifndef UShort
typedef unsigned short UShort;
#endif

#ifndef const
#define const const
#endif /* const */

#ifdef DEBUG
#define Bug(x)                  \
  {                             \
    fprintf(stderr, "%s\n", x); \
    exit(1);                    \
  }
#endif

#undef IEEE_Arith
#undef Avoid_Underflow
#ifdef IEEE_MC68k
#define IEEE_Arith
#endif
#ifdef IEEE_8087
#define IEEE_Arith
#endif

#ifdef Bad_float_h

#else  /* ifndef Bad_float_h */
#endif /* Bad_float_h */

#ifdef IEEE_Arith
#define Scale_Bit         0x10
#define n___gdtoa_bigtens 5
#endif

typedef union {
  double d;
  ULong L[2];
} U;

#ifdef IEEE_8087
#define word0(x) (x)->L[1]
#define word1(x) (x)->L[0]
#else
#define word0(x) (x)->L[0]
#define word1(x) (x)->L[1]
#endif
#define dval(x) (x)->d

/* The following definition of Storeinc is appropriate for MIPS processors.
 * An alternative that might be better on some machines is
 * #define Storeinc(a,b,c) (*a++ = b << 16 | c & 0xffff)
 */
#if defined(IEEE_8087) + defined(VAX)
#define Storeinc(a, b, c)                        \
  (((unsigned short *)a)[1] = (unsigned short)b, \
   ((unsigned short *)a)[0] = (unsigned short)c, a++)
#else
#define Storeinc(a, b, c)                        \
  (((unsigned short *)a)[0] = (unsigned short)b, \
   ((unsigned short *)a)[1] = (unsigned short)c, a++)
#endif

/* #define P DBL_MANT_DIG */
/* Ten_pmax = floor(P*log(2)/log(5)) */
/* Bletch = (highest power of 2 < DBL_MAX_10_EXP) / 16 */
/* Quick_max = floor((P-1)*log(FLT_RADIX)/log(10) - 1) */
/* Int_max = floor(P*log(FLT_RADIX)/log(10) - 1) */

#define Exp_shift   20
#define Exp_shift1  20
#define Exp_msk1    0x100000
#define Exp_msk11   0x100000
#define Exp_mask    0x7ff00000
#define P           53
#define Bias        1023
#define Emin        (-1022)
#define Exp_1       0x3ff00000
#define Exp_11      0x3ff00000
#define Ebits       11
#define Frac_mask   0xfffff
#define Frac_mask1  0xfffff
#define Ten_pmax    22
#define Bletch      0x10
#define Bndry_mask  0xfffff
#define Bndry_mask1 0xfffff
#define LSB         1
#define Sign_bit    0x80000000
#define Log2P       1
#define Tiny0       0
#define Tiny1       1
#define Quick_max   14
#define Int_max     14

#ifndef IEEE_Arith
#define ROUND_BIASED
#else
#ifdef ROUND_BIASED_without_Round_Up
#undef ROUND_BIASED
#define ROUND_BIASED
#endif
#endif

#ifdef RND_PRODQUOT
#define rounded_product(a, b)  a = rnd_prod(a, b)
#define rounded_quotient(a, b) a = rnd_quot(a, b)
extern double rnd_prod(double, double), rnd_quot(double, double);
#else
#define rounded_product(a, b)  a *= b
#define rounded_quotient(a, b) a /= b
#endif

#define Big0 (Frac_mask1 | Exp_msk1 * (DBL_MAX_EXP + Bias - 1))
#define Big1 0xffffffff

#undef Pack_16
#ifndef Pack_32
#define Pack_32
#endif

#ifdef NO_LONG_LONG
#undef ULLong
#ifdef Just_16
#undef Pack_32
#define Pack_16
/* When Pack_32 is not defined, we store 16 bits per 32-bit Long.
 * This makes some inner loops simpler and sometimes saves work
 * during __gdtoa_multiplications, but it often seems to make things slightly
 * slower.  Hence the default is now to store 32 bits per Long.
 */
#endif
#else /* long long available */
#ifndef Llong
#define Llong long long
#endif
#ifndef ULLong
#define ULLong unsigned Llong
#endif
#endif /* NO_LONG_LONG */

#ifdef Pack_32
#define ULbits 32
#define kshift 5
#define kmask  31
#define ALL_ON 0xffffffff
#else
#define ULbits 16
#define kshift 4
#define kmask  15
#define ALL_ON 0xffff
#endif

#define Kmax 9

struct Bigint {
  struct Bigint *next;
  int k, maxwds, sign, wds;
  ULong x[1];
};

typedef struct Bigint Bigint;

typedef struct ThInfo {
  Bigint *Freelist[Kmax + 1];
  Bigint *P5s;
} ThInfo;

#define Bcopy(x, y) \
  memcpy(&x->sign, &y->sign, y->wds * sizeof(ULong) + 2 * sizeof(int))

extern const double __gdtoa_tens[];
extern const double __gdtoa_bigtens[];
extern const double __gdtoa_tinytens[];
extern const unsigned char __gdtoa_hexdig[];
extern const char *const __gdtoa_InfName[6];
extern const char *const __gdtoa_NanName[3];
extern const ULong __gdtoa_NanDflt_Q[4];

Bigint *__gdtoa_Balloc(int, ThInfo **);
void __gdtoa_Bfree(Bigint *, ThInfo **);
Bigint *__gdtoa_d2b(double, int *, int *, ThInfo **);
Bigint *__gdtoa_diff(Bigint *, Bigint *, ThInfo **);
int __gdtoa_gethex(const char **, const FPI *, int *, Bigint **, int,
                   ThInfo **);
Bigint *__gdtoa_i2b(int, ThInfo **);
Bigint *__gdtoa_increment(Bigint *, ThInfo **);
Bigint *__gdtoa_lshift(Bigint *, int, ThInfo **);
Bigint *__gdtoa_mult(Bigint *, Bigint *, ThInfo **);
Bigint *__gdtoa_multadd(Bigint *, int, int, ThInfo **);
char *__gdtoa_nrv_alloc(char *, char **, int, ThInfo **);
char *__gdtoa_rv_alloc(int, ThInfo **);
Bigint *__gdtoa_pow5mult(Bigint *, int, ThInfo **);
Bigint *__gdtoa_s2b(const char *, int, int, ULong, int, ThInfo **);
Bigint *__gdtoa_set_ones(Bigint *, int, ThInfo **);
Bigint *__gdtoa_sum(Bigint *, Bigint *, ThInfo **);

ULong __gdtoa_any_on(Bigint *, int);
char *__gdtoa_add_nanbits(char *, size_t, ULong *, int);
char *__gdtoa_g__fmt(char *, char *, char *, int, ULong, size_t);
double __gdtoa_b2d(Bigint *, int *);
double __gdtoa_ratio(Bigint *, Bigint *);
double __gdtoa_ulp(U *);
int __gdtoa_cmp(Bigint *, Bigint *);
int __gdtoa_hexnan(const char **, const FPI *, ULong *);
int __gdtoa_match(const char **, char *);
int __gdtoa_quorem(Bigint *, Bigint *);
int __gdtoa_strtoIg(const char *, char **, const FPI *, Long *, Bigint **,
                    int *);
int __gdtoa_trailz(Bigint *);
void __gdtoa_ULtoQ(ULong *, ULong *, Long, int);
void __gdtoa_ULtod(ULong *, ULong *, Long, int);
void __gdtoa_ULtodd(ULong *, ULong *, Long, int);
void __gdtoa_ULtof(ULong *, ULong *, Long, int);
void __gdtoa_ULtox(UShort *, ULong *, Long, int);
void __gdtoa_ULtoxL(ULong *, ULong *, Long, int);
void __gdtoa_copybits(ULong *, int, Bigint *);
void __gdtoa_decrement(Bigint *);
void __gdtoa_hexdig_init(void);
void __gdtoa_rshift(Bigint *, int);

forceinline int lo0bits(ULong *y) {
  int k;
  if (*y) {
    k = __builtin_ctz(*y);
    *y >>= k;
    return k;
  } else {
    return 32;
  }
}

forceinline int hi0bits(ULong x) {
  return x ? __builtin_clz(x) : 32;
}

/*
 * NAN_WORD0 and NAN_WORD1 are only referenced in strtod.c.  Prior to
 * 20050115, they used to be hard-wired here (to 0x7ff80000 and 0,
 * respectively), but now are determined by compiling and running
 * qnan.c to generate gd_qnan.h, which specifies d_QNAN0 and d_QNAN1.
 * Formerly gdtoaimp.h recommended supplying suitable -DNAN_WORD0=...
 * and -DNAN_WORD1=...  values if necessary.  This should still work.
 * (On HP Series 700/800 machines, -DNAN_WORD0=0x7ff40000 works.)
 */
#ifdef IEEE_Arith
#ifndef NO_INFNAN_CHECK
#undef INFNAN_CHECK
#define INFNAN_CHECK
#endif
#ifndef NAN_WORD0
#define NAN_WORD0 d_QNAN1
#endif
#ifndef NAN_WORD1
#define NAN_WORD1 d_QNAN0
#endif
#else
#undef INFNAN_CHECK
#endif

#undef SI
#ifdef Sudden_Underflow
#define SI 1
#else
#define SI 0
#endif
