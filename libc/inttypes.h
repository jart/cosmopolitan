#ifndef COSMOPOLITAN_LIBC_INTTYPES_H_
#define COSMOPOLITAN_LIBC_INTTYPES_H_

typedef __INT_LEAST8_TYPE__ int_least8_t;
typedef __UINT_LEAST8_TYPE__ uint_least8_t;
typedef __INT_LEAST16_TYPE__ int_least16_t;
typedef __UINT_LEAST16_TYPE__ uint_least16_t;
typedef __INT_LEAST32_TYPE__ int_least32_t;
typedef __UINT_LEAST32_TYPE__ uint_least32_t;
typedef __INT_LEAST64_TYPE__ int_least64_t;
typedef __UINT_LEAST64_TYPE__ uint_least64_t;

typedef __INT_FAST8_TYPE__ int_fast8_t;
typedef __UINT_FAST8_TYPE__ uint_fast8_t;
typedef __INT_FAST16_TYPE__ int_fast16_t;
typedef __UINT_FAST16_TYPE__ uint_fast16_t;
typedef __INT_FAST32_TYPE__ int_fast32_t;
typedef __UINT_FAST32_TYPE__ uint_fast32_t;
typedef __INT_FAST64_TYPE__ int_fast64_t;
typedef __UINT_FAST64_TYPE__ uint_fast64_t;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define __PRI8 "hh"

#if __SIZEOF_INT__ == 2
#define __PRI16 ""
#elif __SIZEOF_SHORT__ == 2
#define __PRI16 "h"
#elif __SIZEOF_LONG__ == 2
#define __PRI16 "l"
#endif

#if __SIZEOF_INT__ == 4
#define __PRI32 ""
#elif __SIZEOF_LONG__ == 4
#define __PRI32 "l"
#elif __SIZEOF_LONG_LONG__ == 4
#define __PRI32 "ll"
#endif

#if __SIZEOF_INT__ == 8
#define __PRI64 ""
#elif __SIZEOF_LONG__ == 8
#define __PRI64 "l"
#elif __SIZEOF_LONG_LONG__ == 8
#define __PRI64 "ll"
#endif

#if __SIZEOF_INT__ == 16
#define __PRI128 ""
#elif __SIZEOF_LONG__ == 16
#define __PRI128 "l"
#elif __SIZEOF_LONG_LONG__ == 16
#define __PRI128 "ll"
#elif __SIZEOF_INTMAX__ == 16
#define __PRI128 "j"
#else
#define __PRI128 "jj"
#endif

#if __SIZEOF_POINTER__ == __SIZEOF_INT__
#define __PRIPTR ""
#elif __SIZEOF_POINTER__ == __SIZEOF_LONG__
#define __PRIPTR "l"
#elif __SIZEOF_POINTER__ == __SIZEOF_LONG_LONG__
#define __PRIPTR "ll"
#endif

#if __INT_FAST16_WIDTH__ == 16
#define __PRIFAST16 "h"
#elif __INT_FAST16_WIDTH__ == 32
#define __PRIFAST16 ""
#else
#define __PRIFAST16 "l"
#endif

#if __INT_FAST32_WIDTH__ == 32
#define __PRIFAST32 ""
#else
#define __PRIFAST32 "l"
#endif

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » printf » decimal                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define PRId8   __PRI8 "d"
#define PRId16  __PRI16 "d"
#define PRId32  __PRI32 "d"
#define PRId64  __PRI64 "d"
#define PRId128 __PRI128 "d"

#define PRIdLEAST8   __PRI8 "d"
#define PRIdLEAST16  __PRI16 "d"
#define PRIdLEAST32  __PRI32 "d"
#define PRIdLEAST64  __PRI64 "d"
#define PRIdLEAST128 __PRI128 "d"

#define PRIdFAST8   __PRI8 "d"
#define PRIdFAST16  __PRIFAST16 "d"
#define PRIdFAST32  __PRIFAST32 "d"
#define PRIdFAST64  __PRI64 "d"
#define PRIdFAST128 __PRI128 "d"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » printf » unsigned decimal        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define PRIu8   __PRI8 "u"
#define PRIu16  __PRI16 "u"
#define PRIu32  __PRI32 "u"
#define PRIu64  __PRI64 "u"
#define PRIu128 __PRI128 "u"

#define PRIuLEAST8   __PRI8 "u"
#define PRIuLEAST16  __PRI16 "u"
#define PRIuLEAST32  __PRI32 "u"
#define PRIuLEAST64  __PRI64 "u"
#define PRIuLEAST128 __PRI128 "u"

#define PRIuFAST8   __PRI8 "u"
#define PRIuFAST16  __PRIFAST16 "u"
#define PRIuFAST32  __PRIFAST32 "u"
#define PRIuFAST64  __PRI64 "u"
#define PRIuFAST128 __PRI128 "u"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » printf » wut                     ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define PRIi8   __PRI8 "i"
#define PRIi16  __PRI16 "i"
#define PRIi32  __PRI32 "i"
#define PRIi64  __PRI64 "i"
#define PRIi128 __PRI128 "i"

#define PRIiLEAST8   __PRI8 "i"
#define PRIiLEAST16  __PRI16 "i"
#define PRIiLEAST32  __PRI32 "i"
#define PRIiLEAST64  __PRI64 "i"
#define PRIiLEAST128 __PRI128 "i"

#define PRIiFAST8   __PRI8 "i"
#define PRIiFAST16  __PRIFAST16 "i"
#define PRIiFAST32  __PRIFAST32 "i"
#define PRIiFAST64  __PRI64 "i"
#define PRIiFAST128 __PRI128 "i"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » printf » octal                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define PRIo8   __PRI8 "o"
#define PRIo16  __PRI16 "o"
#define PRIo32  __PRI32 "o"
#define PRIo64  __PRI64 "o"
#define PRIo128 __PRI128 "o"

#define PRIoLEAST8   __PRI8 "o"
#define PRIoLEAST16  __PRI16 "o"
#define PRIoLEAST32  __PRI32 "o"
#define PRIoLEAST64  __PRI64 "o"
#define PRIoLEAST128 __PRI128 "o"

#define PRIoFAST8   __PRI8 "o"
#define PRIoFAST16  __PRIFAST16 "o"
#define PRIoFAST32  __PRIFAST32 "o"
#define PRIoFAST64  __PRI64 "o"
#define PRIoFAST128 __PRI128 "o"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » printf » hexadecimal             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define PRIx8   __PRI8 "x"
#define PRIx16  __PRI16 "x"
#define PRIx32  __PRI32 "x"
#define PRIx64  __PRI64 "x"
#define PRIx128 __PRI128 "x"

#define PRIxLEAST8   __PRI8 "x"
#define PRIxLEAST16  __PRI16 "x"
#define PRIxLEAST32  __PRI32 "x"
#define PRIxLEAST64  __PRI64 "x"
#define PRIxLEAST128 __PRI128 "x"

#define PRIxFAST8   __PRI8 "x"
#define PRIxFAST16  __PRIFAST16 "x"
#define PRIxFAST32  __PRIFAST32 "x"
#define PRIxFAST64  __PRI64 "x"
#define PRIxFAST128 __PRI128 "x"

#define PRIX8   __PRI8 "X"
#define PRIX16  __PRI16 "X"
#define PRIX32  __PRI32 "X"
#define PRIX64  __PRI64 "X"
#define PRIX128 __PRI128 "X"

#define PRIXLEAST8   __PRI8 "X"
#define PRIXLEAST16  __PRI16 "X"
#define PRIXLEAST32  __PRI32 "X"
#define PRIXLEAST64  __PRI64 "X"
#define PRIXLEAST128 __PRI128 "X"

#define PRIXFAST8   __PRI8 "X"
#define PRIXFAST16  __PRIFAST16 "X"
#define PRIXFAST32  __PRIFAST32 "X"
#define PRIXFAST64  __PRI64 "X"
#define PRIXFAST128 __PRI128 "X"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » printf » binary                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define PRIb8   __PRI8 "b"
#define PRIb16  __PRI16 "b"
#define PRIb32  __PRI32 "b"
#define PRIb64  __PRI64 "b"
#define PRIb128 __PRI128 "b"

#define PRIbLEAST8   __PRI8 "b"
#define PRIbLEAST16  __PRI16 "b"
#define PRIbLEAST32  __PRI32 "b"
#define PRIbLEAST64  __PRI64 "b"
#define PRIbLEAST128 __PRI128 "b"

#define PRIbFAST8   __PRI8 "b"
#define PRIbFAST16  __PRIFAST16 "b"
#define PRIbFAST32  __PRIFAST32 "b"
#define PRIbFAST64  __PRI64 "b"
#define PRIbFAST128 __PRI128 "b"

#define PRIB8   __PRI8 "B"
#define PRIB16  __PRI16 "B"
#define PRIB32  __PRI32 "B"
#define PRIB64  __PRI64 "B"
#define PRIB128 __PRI128 "B"

#define PRIBLEAST8   __PRI8 "B"
#define PRIBLEAST16  __PRI16 "B"
#define PRIBLEAST32  __PRI32 "B"
#define PRIBLEAST64  __PRI64 "B"
#define PRIBLEAST128 __PRI128 "B"

#define PRIBFAST8   __PRI8 "B"
#define PRIBFAST16  __PRIFAST16 "B"
#define PRIBFAST32  __PRIFAST32 "B"
#define PRIBFAST64  __PRI64 "B"
#define PRIBFAST128 __PRI128 "B"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » printf » miscellaneous           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define PRIdMAX "jd"
#define PRIiMAX "ji"
#define PRIoMAX "jo"
#define PRIuMAX "ju"
#define PRIxMAX "jx"
#define PRIXMAX "jX"

#define PRIdPTR __PRIPTR "d"
#define PRIiPTR __PRIPTR "i"
#define PRIoPTR __PRIPTR "o"
#define PRIuPTR __PRIPTR "u"
#define PRIxPTR __PRIPTR "x"
#define PRIXPTR __PRIPTR "X"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » scanf » decimal                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define SCNd8   __PRI8 "d"
#define SCNd16  __PRI16 "d"
#define SCNd32  __PRI32 "d"
#define SCNd64  __PRI64 "d"
#define SCNd128 __PRI128 "d"

#define SCNdLEAST8   __PRI8 "d"
#define SCNdLEAST16  __PRI16 "d"
#define SCNdLEAST32  __PRI32 "d"
#define SCNdLEAST64  __PRI64 "d"
#define SCNdLEAST128 __PRI128 "d"

#define SCNdFAST8   __PRI8 "d"
#define SCNdFAST16  __PRIFAST16 "d"
#define SCNdFAST32  __PRIFAST32 "d"
#define SCNdFAST64  __PRI64 "d"
#define SCNdFAST128 __PRI128 "d"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » scanf » flexidecimal             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define SCNi8   __PRI8 "i"
#define SCNi16  __PRI16 "i"
#define SCNi32  __PRI32 "i"
#define SCNi64  __PRI64 "i"
#define SCNi128 __PRI128 "i"

#define SCNiLEAST8   __PRI8 "i"
#define SCNiLEAST16  __PRI16 "i"
#define SCNiLEAST32  __PRI32 "i"
#define SCNiLEAST64  __PRI64 "i"
#define SCNiLEAST128 __PRI128 "i"

#define SCNiFAST8   __PRI8 "i"
#define SCNiFAST16  __PRIFAST16 "i"
#define SCNiFAST32  __PRIFAST32 "i"
#define SCNiFAST64  __PRI64 "i"
#define SCNiFAST128 __PRI128 "i"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » scanf » unsigned decimal         ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define SCNu8   __PRI8 "u"
#define SCNu16  __PRI16 "u"
#define SCNu32  __PRI32 "u"
#define SCNu64  __PRI64 "u"
#define SCNu128 __PRI128 "u"

#define SCNuLEAST8   __PRI8 "u"
#define SCNuLEAST16  __PRI16 "u"
#define SCNuLEAST32  __PRI32 "u"
#define SCNuLEAST64  __PRI64 "u"
#define SCNuLEAST128 __PRI128 "u"

#define SCNuFAST8   __PRI8 "u"
#define SCNuFAST16  __PRIFAST16 "u"
#define SCNuFAST32  __PRIFAST32 "u"
#define SCNuFAST64  __PRI64 "u"
#define SCNuFAST128 __PRI128 "u"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » scanf » octal                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define SCNo8   __PRI8 "o"
#define SCNo16  __PRI16 "o"
#define SCNo32  __PRI32 "o"
#define SCNo64  __PRI64 "o"
#define SCNo128 __PRI128 "o"

#define SCNoLEAST8   __PRI8 "o"
#define SCNoLEAST16  __PRI16 "o"
#define SCNoLEAST32  __PRI32 "o"
#define SCNoLEAST64  __PRI64 "o"
#define SCNoLEAST128 __PRI128 "o"

#define SCNoFAST8   __PRI8 "o"
#define SCNoFAST16  __PRIFAST16 "o"
#define SCNoFAST32  __PRIFAST32 "o"
#define SCNoFAST64  __PRI64 "o"
#define SCNoFAST128 __PRI128 "o"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » scanf » hexadecimal              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define SCNx8   __PRI8 "x"
#define SCNx16  __PRI16 "x"
#define SCNx32  __PRI32 "x"
#define SCNx64  __PRI64 "x"
#define SCNx128 __PRI128 "x"

#define SCNxLEAST8   __PRI8 "x"
#define SCNxLEAST16  __PRI16 "x"
#define SCNxLEAST32  __PRI32 "x"
#define SCNxLEAST64  __PRI64 "x"
#define SCNxLEAST128 __PRI128 "x"

#define SCNxFAST8   __PRI8 "x"
#define SCNxFAST16  __PRIFAST16 "x"
#define SCNxFAST32  __PRIFAST32 "x"
#define SCNxFAST64  __PRI64 "x"
#define SCNxFAST128 __PRI128 "x"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » scanf » binary                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define SCNb8   __PRI8 "b"
#define SCNb16  __PRI16 "b"
#define SCNb32  __PRI32 "b"
#define SCNb64  __PRI64 "b"
#define SCNb128 __PRI128 "b"

#define SCNbLEAST8   __PRI8 "b"
#define SCNbLEAST16  __PRI16 "b"
#define SCNbLEAST32  __PRI32 "b"
#define SCNbLEAST64  __PRI64 "b"
#define SCNbLEAST128 __PRI128 "b"

#define SCNbFAST8   __PRI8 "b"
#define SCNbFAST16  __PRIFAST16 "b"
#define SCNbFAST32  __PRIFAST32 "b"
#define SCNbFAST64  __PRI64 "b"
#define SCNbFAST128 __PRI128 "b"

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § dismal format notation » scanf » miscellaneous            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define SCNdMAX "jd"
#define SCNiMAX "ji"
#define SCNoMAX "jo"
#define SCNuMAX "ju"
#define SCNxMAX "jx"

#define SCNdPTR __PRIPTR "d"
#define SCNiPTR __PRIPTR "i"
#define SCNoPTR __PRIPTR "o"
#define SCNuPTR __PRIPTR "u"
#define SCNxPTR __PRIPTR "x"

#endif /* COSMOPOLITAN_LIBC_INTTYPES_H_ */
