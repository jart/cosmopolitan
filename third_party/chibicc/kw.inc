/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf kw.gperf  */
/* Computed positions: -k'1,4,11,14,$' */
// clang-format off

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 1 "kw.gperf"

#include "libc/str/str.h"
#include "third_party/chibicc/kw.h"
#line 10 "kw.gperf"
struct thatispacked KwSlot { char *name; unsigned char code; };

#define TOTAL_KEYWORDS 119
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 28
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 238
/* maximum key range = 238, duplicates = 0 */

static inline unsigned int
hash (register const char *str, register size_t len)
{
  static const unsigned char asso_values[] =
    {
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 115, 239, 239, 239, 239,  50, 239,
      110, 105, 100,   5, 239,   0,  95, 239, 239, 239,
       10, 239, 239, 239, 239, 239,   0, 239, 239, 239,
      239, 239,  45, 239, 239, 239,   0, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239,   5, 239,   0,  90,   5,
       55,  10,   0,  25,  75, 105,  15,  10,  20,  15,
      125,  60,  15,  10,  10,  10,   0,  70,   5,   5,
       10,   0,  45,  85,  10,  30,  15, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239, 239, 239, 239,
      239, 239, 239, 239, 239, 239, 239
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
      case 12:
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
      case 9:
      case 8:
      case 7:
      case 6:
      case 5:
      case 4:
        hval += asso_values[(unsigned char)str[3]+1];
      /*FALLTHROUGH*/
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static inline const struct thatispacked KwSlot *
LookupKw (register const char *str, register size_t len)
{
  static const struct thatispacked KwSlot wordlist[] =
    {
      {""},
#line 110 "kw.gperf"
      {"-",                              KW_MINUS},
#line 116 "kw.gperf"
      {"--",                             KW_DECREMENT},
      {""}, {""}, {""},
#line 63 "kw.gperf"
      {"typeof",                           KW_TYPEOF},
#line 62 "kw.gperf"
      {"typedef",                          KW_TYPEDEF},
      {""}, {""},
#line 29 "kw.gperf"
      {"const",                            KW_CONST},
#line 109 "kw.gperf"
      {"+",                              KW_PLUS},
#line 115 "kw.gperf"
      {"++",                             KW_INCREMENT},
#line 20 "kw.gperf"
      {"for",                              KW_FOR},
      {""},
#line 78 "kw.gperf"
      {"__restrict",                       KW_RESTRICT},
#line 22 "kw.gperf"
      {"sizeof",                           KW_SIZEOF},
#line 75 "kw.gperf"
      {"__asm__",                          KW___ASM__},
#line 41 "kw.gperf"
      {"asm",                              KW_ASM},
#line 15 "kw.gperf"
      {"case",                             KW_CASE},
#line 73 "kw.gperf"
      {"__VA_OPT__",                       KW___VA_OPT__},
#line 13 "kw.gperf"
      {"struct",                           KW_STRUCT},
#line 118 "kw.gperf"
      {"||",                             KW_LOGOR},
      {""},
#line 19 "kw.gperf"
      {"else",                             KW_ELSE},
#line 31 "kw.gperf"
      {"short",                            KW_SHORT},
#line 61 "kw.gperf"
      {"strstr",                           KW_STRSTR},
#line 79 "kw.gperf"
      {"__restrict__",                     KW_RESTRICT},
#line 67 "kw.gperf"
      {"_Alignof",                         KW__ALIGNOF},
#line 18 "kw.gperf"
      {"char",                             KW_CHAR},
#line 48 "kw.gperf"
      {"endif",                            KW_ENDIF},
#line 114 "kw.gperf"
      {"~",                              KW_TILDE},
#line 68 "kw.gperf"
      {"_Atomic",                          KW__ATOMIC},
#line 88 "kw.gperf"
      {"__builtin_ffs",                    KW___BUILTIN_FFS},
#line 55 "kw.gperf"
      {"line",                             KW_LINE},
#line 25 "kw.gperf"
      {"while",                            KW_WHILE},
      {""},
#line 60 "kw.gperf"
      {"strpbrk",                          KW_STRPBRK},
#line 66 "kw.gperf"
      {"_Alignas",                         KW__ALIGNAS},
#line 47 "kw.gperf"
      {"elif",                             KW_ELIF},
#line 49 "kw.gperf"
      {"error",                            KW_ERROR},
#line 74 "kw.gperf"
      {"__alignof__",                      KW___ALIGNOF__},
#line 82 "kw.gperf"
      {"__builtin_add_overflow",           KW___BUILTIN_ADD_OVERFLOW},
#line 44 "kw.gperf"
      {"register",                         KW_REGISTER},
      {""},
#line 69 "kw.gperf"
      {"_Bool",                            KW__BOOL},
#line 87 "kw.gperf"
      {"__builtin_expect",                 KW___BUILTIN_EXPECT},
#line 119 "kw.gperf"
      {"->",                             KW_ARROW},
#line 95 "kw.gperf"
      {"__builtin_popcount",               KW___BUILTIN_POPCOUNT},
      {""},
#line 91 "kw.gperf"
      {"__builtin_fpclassify",             KW___BUILTIN_FPCLASSIFY},
#line 99 "kw.gperf"
      {"__builtin_strchr",                 KW___BUILTIN_STRCHR},
#line 103 "kw.gperf"
      {"__builtin_sub_overflow",           KW___BUILTIN_SUB_OVERFLOW},
#line 72 "kw.gperf"
      {"_Thread_local",                    KW__THREAD_LOCAL},
#line 98 "kw.gperf"
      {"__builtin_reg_class",              KW___BUILTIN_REG_CLASS},
      {""},
#line 102 "kw.gperf"
      {"__builtin_strstr",                 KW___BUILTIN_STRSTR},
#line 92 "kw.gperf"
      {"__builtin_mul_overflow",           KW___BUILTIN_MUL_OVERFLOW},
#line 81 "kw.gperf"
      {"__typeof",                         KW_TYPEOF},
      {""},
#line 86 "kw.gperf"
      {"__builtin_constant_p",             KW___BUILTIN_CONSTANT_P},
#line 108 "kw.gperf"
      {"}",                              KW_RB},
#line 101 "kw.gperf"
      {"__builtin_strpbrk",                KW___BUILTIN_STRPBRK},
#line 104 "kw.gperf"
      {"__builtin_types_compatible_p",     KW___BUILTIN_TYPES_COMPATIBLE_P},
#line 89 "kw.gperf"
      {"__builtin_ffsl",                   KW___BUILTIN_FFSL},
#line 90 "kw.gperf"
      {"__builtin_ffsll",                  KW___BUILTIN_FFSLL},
      {""}, {""}, {""},
#line 96 "kw.gperf"
      {"__builtin_popcountl",              KW___BUILTIN_POPCOUNTL},
#line 97 "kw.gperf"
      {"__builtin_popcountll",             KW___BUILTIN_POPCOUNTLL},
#line 85 "kw.gperf"
      {"__builtin_compare_and_swap",       KW___BUILTIN_COMPARE_AND_SWAP},
      {""},
#line 77 "kw.gperf"
      {"__int128",                         KW___INT128},
#line 17 "kw.gperf"
      {"void",                             KW_VOID},
#line 64 "kw.gperf"
      {"undef",                            KW_UNDEF},
#line 28 "kw.gperf"
      {"double",                           KW_DOUBLE},
      {""},
#line 70 "kw.gperf"
      {"_Generic",                         KW__GENERIC},
#line 43 "kw.gperf"
      {"auto",                             KW_AUTO},
      {""},
#line 58 "kw.gperf"
      {"strchr",                           KW_STRCHR},
      {""},
#line 76 "kw.gperf"
      {"__inline",                         KW_INLINE},
      {""}, {""},
#line 39 "kw.gperf"
      {"define",                           KW_DEFINE},
      {""},
#line 57 "kw.gperf"
      {"restrict",                         KW_RESTRICT},
      {""}, {""},
#line 16 "kw.gperf"
      {"static",                           KW_STATIC},
      {""},
#line 35 "kw.gperf"
      {"continue",                         KW_CONTINUE},
#line 127 "kw.gperf"
      {"__sync_lock_test_and_set",         KW___SYNC_LOCK_TEST_AND_SET},
#line 30 "kw.gperf"
      {"float",                            KW_FLOAT},
#line 56 "kw.gperf"
      {"pragma",                           KW_PRAGMA},
      {""},
#line 94 "kw.gperf"
      {"__builtin_offsetof",               KW___BUILTIN_OFFSETOF},
#line 128 "kw.gperf"
      {"__sync_lock_release",              KW___SYNC_LOCK_RELEASE},
      {""},
#line 111 "kw.gperf"
      {"&",                              KW_AMP},
#line 117 "kw.gperf"
      {"&&",                             KW_LOGAND},
#line 45 "kw.gperf"
      {"__attribute__",                    KW___ATTRIBUTE__},
#line 51 "kw.gperf"
      {"goto",                             KW_GOTO},
      {""}, {""},
#line 12 "kw.gperf"
      {"if",                               KW_IF},
#line 54 "kw.gperf"
      {"int",                              KW_INT},
#line 122 "kw.gperf"
      {"__atomic_store",                   KW___ATOMIC_STORE},
#line 37 "kw.gperf"
      {"ifdef",                            KW_IFDEF},
#line 126 "kw.gperf"
      {"__atomic_test_and_set",            KW___ATOMIC_TEST_AND_SET},
#line 84 "kw.gperf"
      {"__atomic_exchange",                KW___ATOMIC_EXCHANGE},
#line 65 "kw.gperf"
      {"volatile",                         KW_VOLATILE},
      {""}, {""}, {""},
#line 21 "kw.gperf"
      {"do",                               KW_DO},
      {""},
#line 71 "kw.gperf"
      {"_Static_assert",                   KW__STATIC_ASSERT},
      {""},
#line 38 "kw.gperf"
      {"ifndef",                           KW_IFNDEF},
      {""}, {""},
#line 24 "kw.gperf"
      {"long",                             KW_LONG},
      {""}, {""}, {""}, {""},
#line 123 "kw.gperf"
      {"__atomic_clear",                   KW___ATOMIC_CLEAR},
      {""},
#line 32 "kw.gperf"
      {"signed",                           KW_SIGNED},
#line 40 "kw.gperf"
      {"defined",                          KW_DEFINED},
      {""}, {""}, {""},
#line 53 "kw.gperf"
      {"inline",                           KW_INLINE},
#line 36 "kw.gperf"
      {"include",                          KW_INCLUDE},
      {""}, {""}, {""},
#line 50 "kw.gperf"
      {"extern",                           KW_EXTERN},
#line 52 "kw.gperf"
      {"include_next",                     KW_INCLUDE_NEXT},
      {""}, {""}, {""},
#line 14 "kw.gperf"
      {"return",                           KW_RETURN},
      {""},
#line 23 "kw.gperf"
      {"unsigned",                         KW_UNSIGNED},
#line 46 "kw.gperf"
      {"_Noreturn",                        KW__NORETURN},
      {""},
#line 130 "kw.gperf"
      {"__builtin_ia32_pmovmskb128",       KW___BUILTIN_IA32_PMOVMSKB128},
#line 42 "kw.gperf"
      {"default",                          KW_DEFAULT},
      {""},
#line 34 "kw.gperf"
      {"enum",                             KW_ENUM},
      {""},
#line 59 "kw.gperf"
      {"strlen",                           KW_STRLEN},
#line 129 "kw.gperf"
      {"__builtin_ia32_movntdq",           KW___BUILTIN_IA32_MOVNTDQ},
      {""},
#line 83 "kw.gperf"
      {"__builtin_assume_aligned",         KW___BUILTIN_ASSUME_ALIGNED},
      {""},
#line 27 "kw.gperf"
      {"switch",                           KW_SWITCH},
      {""}, {""}, {""}, {""}, {""},
#line 93 "kw.gperf"
      {"__builtin_neg_overflow",           KW___BUILTIN_NEG_OVERFLOW},
      {""}, {""}, {""},
#line 107 "kw.gperf"
      {"{",                              KW_LB},
      {""},
#line 80 "kw.gperf"
      {"__thread",                         KW__THREAD_LOCAL},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 100 "kw.gperf"
      {"__builtin_strlen",                 KW___BUILTIN_STRLEN},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 120 "kw.gperf"
      {".",                              KW_DOT},
      {""}, {""}, {""},
#line 33 "kw.gperf"
      {"break",                            KW_BREAK},
      {""}, {""}, {""}, {""}, {""},
#line 112 "kw.gperf"
      {"*",                              KW_STAR},
      {""},
#line 121 "kw.gperf"
      {"__atomic_load",                    KW___ATOMIC_LOAD},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 106 "kw.gperf"
      {")",                              KW_RP},
      {""}, {""}, {""},
#line 26 "kw.gperf"
      {"union",                            KW_UNION},
      {""}, {""}, {""}, {""}, {""},
#line 105 "kw.gperf"
      {"(",                              KW_LP},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 113 "kw.gperf"
      {"!",                              KW_EXCLAIM},
      {""},
#line 125 "kw.gperf"
      {"__atomic_fetch_add",               KW___ATOMIC_FETCH_ADD},
      {""}, {""}, {""}, {""},
#line 124 "kw.gperf"
      {"__atomic_sub_fetch",               KW___ATOMIC_SUB_FETCH}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
