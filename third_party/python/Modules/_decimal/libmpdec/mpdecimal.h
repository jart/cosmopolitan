#ifndef MPDECIMAL_H
#define MPDECIMAL_H
#include "libc/fmt/conv.h"
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "third_party/python/pyconfig.h"
COSMOPOLITAN_C_START_

#define MPD_VERSION "2.4.2"
#define MPD_MAJOR_VERSION 2
#define MPD_MINOR_VERSION 4
#define MPD_MICRO_VERSION 2
#define MPD_VERSION_HEX ((MPD_MAJOR_VERSION << 24) | \
                         (MPD_MINOR_VERSION << 16) | \
                         (MPD_MICRO_VERSION <<  8))

const char *mpd_version(void);

#define MPD_UINT_MAX UINT64_MAX
#define MPD_BITS_PER_UINT 64
typedef uint64_t mpd_uint_t;  /* unsigned mod type */

#define MPD_SIZE_MAX SIZE_MAX
typedef size_t mpd_size_t; /* unsigned size type */

/* type for exp, digits, len, prec */
#define MPD_SSIZE_MAX INT64_MAX
#define MPD_SSIZE_MIN INT64_MIN
typedef int64_t mpd_ssize_t;
#define _mpd_strtossize strtoll

#define MPD_RADIX 10000000000000000000ULL  /* 10**19 */
#define MPD_RDIGITS 19
#define MPD_MAX_POW10 19
#define MPD_EXPDIGITS 19  /* MPD_EXPDIGITS <= MPD_RDIGITS+1 */

#define MPD_MAXTRANSFORM_2N 4294967296ULL      /* 2**32 */
#define MPD_MAX_PREC 999999999999999999LL
#define MPD_MAX_PREC_LOG2 64
#define MPD_ELIMIT  1000000000000000000LL
#define MPD_MAX_EMAX   999999999999999999LL    /* ELIMIT-1 */
#define MPD_MIN_EMIN  (-999999999999999999LL)  /* -EMAX */
#define MPD_MIN_ETINY (MPD_MIN_EMIN-(MPD_MAX_PREC-1))
#define MPD_EXP_INF 2000000000000000001LL
#define MPD_EXP_CLAMP (-4000000000000000001LL)
#define MPD_MAXIMPORT 105263157894736842L /* ceil((2*MPD_MAX_PREC)/MPD_RDIGITS) */

/* conversion specifiers */
#define PRI_mpd_uint_t PRIu64
#define PRI_mpd_ssize_t PRIi64

#if MPD_SIZE_MAX != MPD_UINT_MAX
  #error "unsupported platform: need mpd_size_t == mpd_uint_t"
#endif


/******************************************************************************/
/*                                Context                                     */
/******************************************************************************/

enum {
    MPD_ROUND_UP,          /* round away from 0               */
    MPD_ROUND_DOWN,        /* round toward 0 (truncate)       */
    MPD_ROUND_CEILING,     /* round toward +infinity          */
    MPD_ROUND_FLOOR,       /* round toward -infinity          */
    MPD_ROUND_HALF_UP,     /* 0.5 is rounded up               */
    MPD_ROUND_HALF_DOWN,   /* 0.5 is rounded down             */
    MPD_ROUND_HALF_EVEN,   /* 0.5 is rounded to even          */
    MPD_ROUND_05UP,        /* round zero or five away from 0  */
    MPD_ROUND_TRUNC,       /* truncate, but set infinity      */
    MPD_ROUND_GUARD
};

enum { MPD_CLAMP_DEFAULT, MPD_CLAMP_IEEE_754, MPD_CLAMP_GUARD };

extern const char *mpd_round_string[MPD_ROUND_GUARD];
extern const char *mpd_clamp_string[MPD_CLAMP_GUARD];


typedef struct mpd_context_t {
    mpd_ssize_t prec;   /* precision */
    mpd_ssize_t emax;   /* max positive exp */
    mpd_ssize_t emin;   /* min negative exp */
    uint32_t traps;     /* status events that should be trapped */
    uint32_t status;    /* status flags */
    uint32_t newtrap;   /* set by mpd_addstatus_raise() */
    int      round;     /* rounding mode */
    int      clamp;     /* clamp mode */
    int      allcr;     /* all functions correctly rounded */
} mpd_context_t;


/* Status flags */
#define MPD_Clamped             0x00000001U
#define MPD_Conversion_syntax   0x00000002U
#define MPD_Division_by_zero    0x00000004U
#define MPD_Division_impossible 0x00000008U
#define MPD_Division_undefined  0x00000010U
#define MPD_Fpu_error           0x00000020U
#define MPD_Inexact             0x00000040U
#define MPD_Invalid_context     0x00000080U
#define MPD_Invalid_operation   0x00000100U
#define MPD_Malloc_error        0x00000200U
#define MPD_Not_implemented     0x00000400U
#define MPD_Overflow            0x00000800U
#define MPD_Rounded             0x00001000U
#define MPD_Subnormal           0x00002000U
#define MPD_Underflow           0x00004000U
#define MPD_Max_status         (0x00008000U-1U)

/* Conditions that result in an IEEE 754 exception */
#define MPD_IEEE_Invalid_operation (MPD_Conversion_syntax |   \
                                    MPD_Division_impossible | \
                                    MPD_Division_undefined |  \
                                    MPD_Fpu_error |           \
                                    MPD_Invalid_context |     \
                                    MPD_Invalid_operation |   \
                                    MPD_Malloc_error)         \

/* Errors that require the result of an operation to be set to NaN */
#define MPD_Errors (MPD_IEEE_Invalid_operation | \
                    MPD_Division_by_zero)

/* Default traps */
#define MPD_Traps (MPD_IEEE_Invalid_operation | \
                   MPD_Division_by_zero |       \
                   MPD_Overflow |               \
                   MPD_Underflow)

/* Official name */
#define MPD_Insufficient_storage MPD_Malloc_error

/* IEEE 754 interchange format contexts */
#define MPD_IEEE_CONTEXT_MAX_BITS 512 /* 16*(log2(MPD_MAX_EMAX / 3)-3) */
#define MPD_DECIMAL32 32
#define MPD_DECIMAL64 64
#define MPD_DECIMAL128 128

#define MPD_MINALLOC_MIN 2
#define MPD_MINALLOC_MAX 64

extern mpd_ssize_t MPD_MINALLOC;
extern void (* mpd_traphandler)(mpd_context_t *);
void mpd_dflt_traphandler(mpd_context_t *);
void mpd_setminalloc(mpd_ssize_t);
void mpd_init(mpd_context_t *, mpd_ssize_t);
void mpd_maxcontext(mpd_context_t *);
void mpd_defaultcontext(mpd_context_t *);
void mpd_basiccontext(mpd_context_t *);
int mpd_ieee_context(mpd_context_t *, int);
mpd_ssize_t mpd_getprec(const mpd_context_t *);
mpd_ssize_t mpd_getemax(const mpd_context_t *);
mpd_ssize_t mpd_getemin(const mpd_context_t *);
int mpd_getround(const mpd_context_t *);
uint32_t mpd_gettraps(const mpd_context_t *);
uint32_t mpd_getstatus(const mpd_context_t *);
int mpd_getclamp(const mpd_context_t *);
int mpd_getcr(const mpd_context_t *);
int mpd_qsetprec(mpd_context_t *, mpd_ssize_t);
int mpd_qsetemax(mpd_context_t *, mpd_ssize_t);
int mpd_qsetemin(mpd_context_t *, mpd_ssize_t);
int mpd_qsetround(mpd_context_t *, int);
int mpd_qsettraps(mpd_context_t *, uint32_t);
int mpd_qsetstatus(mpd_context_t *, uint32_t);
int mpd_qsetclamp(mpd_context_t *, int);
int mpd_qsetcr(mpd_context_t *, int);
void mpd_addstatus_raise(mpd_context_t *, uint32_t);


/******************************************************************************/
/*                           Decimal Arithmetic                               */
/******************************************************************************/

/* mpd_t flags */
#define MPD_POS                 ((uint8_t)0)
#define MPD_NEG                 ((uint8_t)1)
#define MPD_INF                 ((uint8_t)2)
#define MPD_NAN                 ((uint8_t)4)
#define MPD_SNAN                ((uint8_t)8)
#define MPD_SPECIAL (MPD_INF|MPD_NAN|MPD_SNAN)
#define MPD_STATIC              ((uint8_t)16)
#define MPD_STATIC_DATA         ((uint8_t)32)
#define MPD_SHARED_DATA         ((uint8_t)64)
#define MPD_CONST_DATA          ((uint8_t)128)
#define MPD_DATAFLAGS (MPD_STATIC_DATA|MPD_SHARED_DATA|MPD_CONST_DATA)

typedef struct mpd_t {
    uint8_t flags;
    mpd_ssize_t exp;
    mpd_ssize_t digits;
    mpd_ssize_t len;
    mpd_ssize_t alloc;
    mpd_uint_t *data;
} mpd_t;

typedef unsigned char uchar;


/******************************************************************************/
/*                       Quiet, thread-safe functions                         */
/******************************************************************************/

/* format specification */
typedef struct mpd_spec_t {
    mpd_ssize_t min_width; /* minimum field width */
    mpd_ssize_t prec;      /* fraction digits or significant digits */
    char type;             /* conversion specifier */
    char align;            /* alignment */
    char sign;             /* sign printing/alignment */
    char fill[5];          /* fill character */
    const char *dot;       /* decimal point */
    const char *sep;       /* thousands separator */
    const char *grouping;  /* grouping of digits */
} mpd_spec_t;

/* output to a string */
char *mpd_to_sci(const mpd_t *, int);
char *mpd_to_eng(const mpd_t *, int);
mpd_ssize_t mpd_to_sci_size(char **, const mpd_t *, int);
mpd_ssize_t mpd_to_eng_size(char **, const mpd_t *, int);
int mpd_validate_lconv(mpd_spec_t *);
int mpd_parse_fmt_str(mpd_spec_t *, const char *, int);
char *mpd_qformat_spec(const mpd_t *, const mpd_spec_t *, const mpd_context_t *, uint32_t *);
char *mpd_qformat(const mpd_t *, const char *, const mpd_context_t *, uint32_t *);

#define MPD_NUM_FLAGS 15
#define MPD_MAX_FLAG_STRING 208
#define MPD_MAX_FLAG_LIST (MPD_MAX_FLAG_STRING+18)
#define MPD_MAX_SIGNAL_LIST 121

int mpd_snprint_flags(char *, int, uint32_t);
int mpd_lsnprint_flags(char *, int, uint32_t, const char *[]);
int mpd_lsnprint_signals(char *, int, uint32_t, const char *[]);

/* output to a file */
void mpd_fprint(FILE *, const mpd_t *);
void mpd_print(const mpd_t *);

/* assignment from a string */
void mpd_qset_string(mpd_t *, const char *s, const mpd_context_t *, uint32_t *);

/* set to NaN with error flags */
void mpd_seterror(mpd_t *, uint32_t, uint32_t *);
/* set a special with sign and type */
void mpd_setspecial(mpd_t *, uint8_t, uint8_t);
/* set coefficient to zero or all nines */
void mpd_zerocoeff(mpd_t *);
void mpd_qmaxcoeff(mpd_t *, const mpd_context_t *, uint32_t *);

/* quietly assign a C integer type to an mpd_t */
void mpd_qset_ssize(mpd_t *, mpd_ssize_t, const mpd_context_t *, uint32_t *);
void mpd_qset_i32(mpd_t *, int32_t, const mpd_context_t *, uint32_t *);
void mpd_qset_uint(mpd_t *, mpd_uint_t, const mpd_context_t *, uint32_t *);
void mpd_qset_u32(mpd_t *, uint32_t, const mpd_context_t *, uint32_t *);
void mpd_qset_i64(mpd_t *, int64_t, const mpd_context_t *, uint32_t *);
void mpd_qset_u64(mpd_t *, uint64_t, const mpd_context_t *, uint32_t *);

/* quietly assign a C integer type to an mpd_t with a static coefficient */
void mpd_qsset_ssize(mpd_t *, mpd_ssize_t, const mpd_context_t *, uint32_t *);
void mpd_qsset_i32(mpd_t *, int32_t, const mpd_context_t *, uint32_t *);
void mpd_qsset_uint(mpd_t *, mpd_uint_t, const mpd_context_t *, uint32_t *);
void mpd_qsset_u32(mpd_t *, uint32_t, const mpd_context_t *, uint32_t *);
mpd_ssize_t mpd_qget_ssize(const mpd_t *, uint32_t *);
mpd_uint_t mpd_qget_uint(const mpd_t *, uint32_t *);
mpd_uint_t mpd_qabs_uint(const mpd_t *, uint32_t *);
int32_t mpd_qget_i32(const mpd_t *, uint32_t *);
uint32_t mpd_qget_u32(const mpd_t *, uint32_t *);
int64_t mpd_qget_i64(const mpd_t *, uint32_t *);
uint64_t mpd_qget_u64(const mpd_t *, uint32_t *);
int mpd_qcheck_nan(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
int mpd_qcheck_nans(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qfinalize(mpd_t *, const mpd_context_t *, uint32_t *);
const char *mpd_class(const mpd_t *, const mpd_context_t *);
mpd_t *mpd_qncopy(const mpd_t *);
int mpd_qcopy(mpd_t *, const mpd_t *,  uint32_t *);
int mpd_qcopy_abs(mpd_t *, const mpd_t *, uint32_t *);
int mpd_qcopy_negate(mpd_t *, const mpd_t *, uint32_t *);
int mpd_qcopy_sign(mpd_t *, const mpd_t *, const mpd_t *, uint32_t *);
void mpd_qand(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qinvert(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qlogb(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qor(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qscaleb(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qxor(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
int mpd_same_quantum(const mpd_t *, const mpd_t *);
void mpd_qrotate(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
int mpd_qshiftl(mpd_t *, const mpd_t *, mpd_ssize_t, uint32_t *);
mpd_uint_t mpd_qshiftr(mpd_t *, const mpd_t *, mpd_ssize_t, uint32_t *);
mpd_uint_t mpd_qshiftr_inplace(mpd_t *, mpd_ssize_t);
void mpd_qshift(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qshiftn(mpd_t *, const mpd_t *, mpd_ssize_t, const mpd_context_t *, uint32_t *);
int mpd_qcmp(const mpd_t *, const mpd_t *, uint32_t *);
int mpd_qcompare(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
int mpd_qcompare_signal(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
int mpd_cmp_total(const mpd_t *, const mpd_t *);
int mpd_cmp_total_mag(const mpd_t *, const mpd_t *);
int mpd_compare_total(mpd_t *, const mpd_t *, const mpd_t *);
int mpd_compare_total_mag(mpd_t *, const mpd_t *, const mpd_t *);
void mpd_qround_to_intx(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qround_to_int(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qtrunc(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qfloor(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qceil(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qabs(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qmax(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qmax_mag(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qmin(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qmin_mag(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qminus(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qplus(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qnext_minus(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qnext_plus(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qnext_toward(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qquantize(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qrescale(mpd_t *, const mpd_t *, mpd_ssize_t, const mpd_context_t *, uint32_t *);
void mpd_qrescale_fmt(mpd_t *, const mpd_t *, mpd_ssize_t, const mpd_context_t *, uint32_t *);
void mpd_qreduce(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qadd(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qadd_ssize(mpd_t *, const mpd_t *, mpd_ssize_t, const mpd_context_t *, uint32_t *);
void mpd_qadd_i32(mpd_t *, const mpd_t *, int32_t, const mpd_context_t *, uint32_t *);
void mpd_qadd_uint(mpd_t *, const mpd_t *, mpd_uint_t, const mpd_context_t *, uint32_t *);
void mpd_qadd_u32(mpd_t *, const mpd_t *, uint32_t, const mpd_context_t *, uint32_t *);
void mpd_qsub(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qsub_ssize(mpd_t *, const mpd_t *, mpd_ssize_t, const mpd_context_t *, uint32_t *);
void mpd_qsub_i32(mpd_t *, const mpd_t *, int32_t, const mpd_context_t *, uint32_t *);
void mpd_qsub_uint(mpd_t *, const mpd_t *, mpd_uint_t, const mpd_context_t *, uint32_t *);
void mpd_qsub_u32(mpd_t *, const mpd_t *, uint32_t, const mpd_context_t *, uint32_t *);
void mpd_qmul(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qmul_ssize(mpd_t *, const mpd_t *, mpd_ssize_t, const mpd_context_t *, uint32_t *);
void mpd_qmul_i32(mpd_t *, const mpd_t *, int32_t, const mpd_context_t *, uint32_t *);
void mpd_qmul_uint(mpd_t *, const mpd_t *, mpd_uint_t, const mpd_context_t *, uint32_t *);
void mpd_qmul_u32(mpd_t *, const mpd_t *, uint32_t, const mpd_context_t *, uint32_t *);
void mpd_qfma(mpd_t *, const mpd_t *, const mpd_t *, const mpd_t *c, const mpd_context_t *, uint32_t *);
void mpd_qdiv(mpd_t *q, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qdiv_ssize(mpd_t *, const mpd_t *, mpd_ssize_t, const mpd_context_t *, uint32_t *);
void mpd_qdiv_i32(mpd_t *, const mpd_t *, int32_t, const mpd_context_t *, uint32_t *);
void mpd_qdiv_uint(mpd_t *, const mpd_t *, mpd_uint_t, const mpd_context_t *, uint32_t *);
void mpd_qdiv_u32(mpd_t *, const mpd_t *, uint32_t, const mpd_context_t *, uint32_t *);
void mpd_qdivint(mpd_t *q, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qrem(mpd_t *r, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qrem_near(mpd_t *r, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qdivmod(mpd_t *q, mpd_t *r, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qpow(mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qpowmod(mpd_t *, const mpd_t *, const mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qexp(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qln10(mpd_t *, mpd_ssize_t, uint32_t *);
void mpd_qln(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qlog10(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qsqrt(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qinvroot(mpd_t *, const mpd_t *, const mpd_context_t *, uint32_t *);
void mpd_qadd_i64(mpd_t *, const mpd_t *, int64_t, const mpd_context_t *, uint32_t *);
void mpd_qadd_u64(mpd_t *, const mpd_t *, uint64_t, const mpd_context_t *, uint32_t *);
void mpd_qsub_i64(mpd_t *, const mpd_t *, int64_t, const mpd_context_t *, uint32_t *);
void mpd_qsub_u64(mpd_t *, const mpd_t *, uint64_t, const mpd_context_t *, uint32_t *);
void mpd_qmul_i64(mpd_t *, const mpd_t *, int64_t, const mpd_context_t *, uint32_t *);
void mpd_qmul_u64(mpd_t *, const mpd_t *, uint64_t, const mpd_context_t *, uint32_t *);
void mpd_qdiv_i64(mpd_t *, const mpd_t *, int64_t, const mpd_context_t *, uint32_t *);
void mpd_qdiv_u64(mpd_t *, const mpd_t *, uint64_t, const mpd_context_t *, uint32_t *);

size_t mpd_sizeinbase(const mpd_t *, uint32_t);
void mpd_qimport_u16(mpd_t *, const uint16_t *, size_t, uint8_t, uint32_t, const mpd_context_t *, uint32_t *);
void mpd_qimport_u32(mpd_t *, const uint32_t *, size_t, uint8_t, uint32_t, const mpd_context_t *, uint32_t *);
size_t mpd_qexport_u16(uint16_t **, size_t, uint32_t, const mpd_t *, uint32_t *);
size_t mpd_qexport_u32(uint32_t **, size_t, uint32_t, const mpd_t *, uint32_t *);


/******************************************************************************/
/*                           Signalling functions                             */
/******************************************************************************/

char *mpd_format(const mpd_t *, const char *, mpd_context_t *);
void mpd_import_u16(mpd_t *, const uint16_t *, size_t, uint8_t, uint32_t, mpd_context_t *);
void mpd_import_u32(mpd_t *, const uint32_t *, size_t, uint8_t, uint32_t, mpd_context_t *);
size_t mpd_export_u16(uint16_t **, size_t, uint32_t, const mpd_t *, mpd_context_t *);
size_t mpd_export_u32(uint32_t **, size_t, uint32_t, const mpd_t *, mpd_context_t *);
void mpd_finalize(mpd_t *, mpd_context_t *);
int mpd_check_nan(mpd_t *, const mpd_t *, mpd_context_t *);
int mpd_check_nans(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_set_string(mpd_t *, const char *s, mpd_context_t *);
void mpd_maxcoeff(mpd_t *, mpd_context_t *);
void mpd_sset_ssize(mpd_t *, mpd_ssize_t, mpd_context_t *);
void mpd_sset_i32(mpd_t *, int32_t, mpd_context_t *);
void mpd_sset_uint(mpd_t *, mpd_uint_t, mpd_context_t *);
void mpd_sset_u32(mpd_t *, uint32_t, mpd_context_t *);
void mpd_set_ssize(mpd_t *, mpd_ssize_t, mpd_context_t *);
void mpd_set_i32(mpd_t *, int32_t, mpd_context_t *);
void mpd_set_uint(mpd_t *, mpd_uint_t, mpd_context_t *);
void mpd_set_u32(mpd_t *, uint32_t, mpd_context_t *);
void mpd_set_i64(mpd_t *, int64_t, mpd_context_t *);
void mpd_set_u64(mpd_t *, uint64_t, mpd_context_t *);
mpd_ssize_t mpd_get_ssize(const mpd_t *, mpd_context_t *);
mpd_uint_t mpd_get_uint(const mpd_t *, mpd_context_t *);
mpd_uint_t mpd_abs_uint(const mpd_t *, mpd_context_t *);
int32_t mpd_get_i32(const mpd_t *, mpd_context_t *);
uint32_t mpd_get_u32(const mpd_t *, mpd_context_t *);
int64_t mpd_get_i64(const mpd_t *, mpd_context_t *);
uint64_t mpd_get_u64(const mpd_t *, mpd_context_t *);
void mpd_and(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_copy(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_canonical(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_copy_abs(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_copy_negate(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_copy_sign(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_invert(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_logb(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_or(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_rotate(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_scaleb(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_shiftl(mpd_t *, const mpd_t *, mpd_ssize_t, mpd_context_t *);
mpd_uint_t mpd_shiftr(mpd_t *, const mpd_t *, mpd_ssize_t, mpd_context_t *);
void mpd_shiftn(mpd_t *, const mpd_t *, mpd_ssize_t, mpd_context_t *);
void mpd_shift(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_xor(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_abs(mpd_t *, const mpd_t *, mpd_context_t *);
int mpd_cmp(const mpd_t *, const mpd_t *, mpd_context_t *);
int mpd_compare(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
int mpd_compare_signal(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_add(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_add_ssize(mpd_t *, const mpd_t *, mpd_ssize_t, mpd_context_t *);
void mpd_add_i32(mpd_t *, const mpd_t *, int32_t, mpd_context_t *);
void mpd_add_uint(mpd_t *, const mpd_t *, mpd_uint_t, mpd_context_t *);
void mpd_add_u32(mpd_t *, const mpd_t *, uint32_t, mpd_context_t *);
void mpd_sub(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_sub_ssize(mpd_t *, const mpd_t *, mpd_ssize_t, mpd_context_t *);
void mpd_sub_i32(mpd_t *, const mpd_t *, int32_t, mpd_context_t *);
void mpd_sub_uint(mpd_t *, const mpd_t *, mpd_uint_t, mpd_context_t *);
void mpd_sub_u32(mpd_t *, const mpd_t *, uint32_t, mpd_context_t *);
void mpd_div(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_div_ssize(mpd_t *, const mpd_t *, mpd_ssize_t, mpd_context_t *);
void mpd_div_i32(mpd_t *, const mpd_t *, int32_t, mpd_context_t *);
void mpd_div_uint(mpd_t *, const mpd_t *, mpd_uint_t, mpd_context_t *);
void mpd_div_u32(mpd_t *, const mpd_t *, uint32_t, mpd_context_t *);
void mpd_divmod(mpd_t *, mpd_t *r, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_divint(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_exp(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_fma(mpd_t *, const mpd_t *, const mpd_t *, const mpd_t *c, mpd_context_t *);
void mpd_ln(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_log10(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_max(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_max_mag(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_min(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_min_mag(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_minus(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_mul(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_mul_ssize(mpd_t *, const mpd_t *, mpd_ssize_t, mpd_context_t *);
void mpd_mul_i32(mpd_t *, const mpd_t *, int32_t, mpd_context_t *);
void mpd_mul_uint(mpd_t *, const mpd_t *, mpd_uint_t, mpd_context_t *);
void mpd_mul_u32(mpd_t *, const mpd_t *, uint32_t, mpd_context_t *);
void mpd_next_minus(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_next_plus(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_next_toward(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_plus(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_pow(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_powmod(mpd_t *, const mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_quantize(mpd_t *, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_rescale(mpd_t *, const mpd_t *, mpd_ssize_t, mpd_context_t *);
void mpd_reduce(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_rem(mpd_t *r, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_rem_near(mpd_t *r, const mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_round_to_intx(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_round_to_int(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_trunc(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_floor(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_ceil(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_sqrt(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_invroot(mpd_t *, const mpd_t *, mpd_context_t *);
void mpd_add_i64(mpd_t *, const mpd_t *, int64_t, mpd_context_t *);
void mpd_add_u64(mpd_t *, const mpd_t *, uint64_t, mpd_context_t *);
void mpd_sub_i64(mpd_t *, const mpd_t *, int64_t, mpd_context_t *);
void mpd_sub_u64(mpd_t *, const mpd_t *, uint64_t, mpd_context_t *);
void mpd_div_i64(mpd_t *, const mpd_t *, int64_t, mpd_context_t *);
void mpd_div_u64(mpd_t *, const mpd_t *, uint64_t, mpd_context_t *);
void mpd_mul_i64(mpd_t *, const mpd_t *, int64_t, mpd_context_t *);
void mpd_mul_u64(mpd_t *, const mpd_t *, uint64_t, mpd_context_t *);


/******************************************************************************/
/*                          Configuration specific                            */
/******************************************************************************/

void mpd_qsset_i64(mpd_t *, int64_t, const mpd_context_t *, uint32_t *);
void mpd_qsset_u64(mpd_t *, uint64_t, const mpd_context_t *, uint32_t *);
void mpd_sset_i64(mpd_t *, int64_t, mpd_context_t *);
void mpd_sset_u64(mpd_t *, uint64_t, mpd_context_t *);


/******************************************************************************/
/*                       Get attributes of a decimal                          */
/******************************************************************************/

mpd_ssize_t mpd_adjexp(const mpd_t *);
mpd_ssize_t mpd_etiny(const mpd_context_t *);
mpd_ssize_t mpd_etop(const mpd_context_t *);
mpd_uint_t mpd_msword(const mpd_t *);
int mpd_word_digits(mpd_uint_t);
/* most significant digit of a word */
mpd_uint_t mpd_msd(mpd_uint_t);
/* least significant digit of a word */
mpd_uint_t mpd_lsd(mpd_uint_t);
/* coefficient size needed to store 'digits' */
mpd_ssize_t mpd_digits_to_size(mpd_ssize_t);
/* number of digits in the exponent, undefined for MPD_SSIZE_MIN */
int mpd_exp_digits(mpd_ssize_t);
int mpd_iscanonical(const mpd_t *);
int mpd_isfinite(const mpd_t *);
int mpd_isinfinite(const mpd_t *);
int mpd_isinteger(const mpd_t *);
int mpd_isnan(const mpd_t *);
int mpd_isnegative(const mpd_t *);
int mpd_ispositive(const mpd_t *);
int mpd_isqnan(const mpd_t *);
int mpd_issigned(const mpd_t *);
int mpd_issnan(const mpd_t *);
int mpd_isspecial(const mpd_t *);
int mpd_iszero(const mpd_t *);
/* undefined for special numbers */
int mpd_iszerocoeff(const mpd_t *);
int mpd_isnormal(const mpd_t *, const mpd_context_t *);
int mpd_issubnormal(const mpd_t *, const mpd_context_t *);
/* odd word */
int mpd_isoddword(mpd_uint_t);
/* odd coefficient */
int mpd_isoddcoeff(const mpd_t *);
/* odd decimal, only defined for integers */
int mpd_isodd(const mpd_t *);
/* even decimal, only defined for integers */
int mpd_iseven(const mpd_t *);
/* 0 if dec is positive, 1 if dec is negative */
uint8_t mpd_sign(const mpd_t *);
/* 1 if dec is positive, -1 if dec is negative */
int mpd_arith_sign(const mpd_t *);
long mpd_radix(void);
int mpd_isdynamic(const mpd_t *);
int mpd_isstatic(const mpd_t *);
int mpd_isdynamic_data(const mpd_t *);
int mpd_isstatic_data(const mpd_t *);
int mpd_isshared_data(const mpd_t *);
int mpd_isconst_data(const mpd_t *);
mpd_ssize_t mpd_trail_zeros(const mpd_t *);


/******************************************************************************/
/*                       Set attributes of a decimal                          */
/******************************************************************************/

void mpd_setdigits(mpd_t *);
void mpd_set_sign(mpd_t *, uint8_t);
void mpd_signcpy(mpd_t *, const mpd_t *);
void mpd_set_infinity(mpd_t *);
void mpd_set_qnan(mpd_t *);
void mpd_set_snan(mpd_t *);
void mpd_set_negative(mpd_t *);
void mpd_set_positive(mpd_t *);
void mpd_set_dynamic(mpd_t *);
void mpd_set_static(mpd_t *);
void mpd_set_dynamic_data(mpd_t *);
void mpd_set_static_data(mpd_t *);
void mpd_set_shared_data(mpd_t *);
void mpd_set_const_data(mpd_t *);
void mpd_clear_flags(mpd_t *);
void mpd_set_flags(mpd_t *, uint8_t);
void mpd_copy_flags(mpd_t *, const mpd_t *);


/******************************************************************************/
/*                              Error Macros                                  */
/******************************************************************************/

#define mpd_err_fatal(...) \
    do {fprintf(stderr, "%s:%d: error: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);  fputc('\n', stderr);    \
        abort();                                               \
    } while (0)
#define mpd_err_warn(...) \
    do {fprintf(stderr, "%s:%d: warning: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); fputc('\n', stderr);       \
    } while (0)


/******************************************************************************/
/*                            Memory handling                                 */
/******************************************************************************/

extern void *(*mpd_mallocfunc)(size_t);
extern void *(*mpd_callocfunc)(size_t, size_t);
extern void *(*mpd_reallocfunc)(void *, size_t);
extern void (*mpd_free)(void *);

void *mpd_callocfunc_em(size_t, size_t);

void *mpd_alloc(mpd_size_t, mpd_size_t);
void *mpd_calloc(mpd_size_t, mpd_size_t);
void *mpd_realloc(void *, mpd_size_t, mpd_size_t, uint8_t *);
void *mpd_sh_alloc(mpd_size_t, mpd_size_t, mpd_size_t);

mpd_t *mpd_qnew(void);
mpd_t *mpd_new(mpd_context_t *);
mpd_t *mpd_qnew_size(mpd_ssize_t);
void mpd_del(mpd_t *);

void mpd_uint_zero(mpd_uint_t *, mpd_size_t);
int mpd_qresize(mpd_t *, mpd_ssize_t, uint32_t *);
int mpd_qresize_zero(mpd_t *, mpd_ssize_t, uint32_t *);
void mpd_minalloc(mpd_t *);

int mpd_resize(mpd_t *, mpd_ssize_t, mpd_context_t *);
int mpd_resize_zero(mpd_t *, mpd_ssize_t, mpd_context_t *);

COSMOPOLITAN_C_END_
#endif /* MPDECIMAL_H */
