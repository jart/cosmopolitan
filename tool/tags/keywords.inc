/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf keywords.gperf  */
/* Computed positions: -k'1-3,5,8' */

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

#line 1 "keywords.gperf"

#include "libc/str/str.h"
#include "tool/tags/tags.h"
#include "o/tool/tags/tags.h.inc"
#line 11 "keywords.gperf"
struct KeywordSlot { char *name; int code; };

#define TOTAL_KEYWORDS 149
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 34
#define MIN_HASH_VALUE 15
#define MAX_HASH_VALUE 485
/* maximum key range = 471, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register size_t len)
{
  static const unsigned short asso_values[] =
    {
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486,   0,   5,   5, 486, 486,
      486,   5, 486,   0, 486, 486, 486, 486,   0, 486,
      486, 486, 486,   5,   0, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486,  45,   0,  40, 120, 160,
       80,  60,   0, 130,   0,  55,  15, 486, 175,  20,
       10,   0,   5,  70, 125,  10,  40,   0,  40, 145,
       20,  20,   0, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486, 486, 486, 486,
      486, 486, 486, 486, 486, 486, 486
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[7]+1];
      /*FALLTHROUGH*/
      case 7:
      case 6:
      case 5:
        hval += asso_values[(unsigned char)str[4]+1];
      /*FALLTHROUGH*/
      case 4:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

const struct KeywordSlot *
LookupKeyword (register const char *str, register size_t len)
{
  static const struct KeywordSlot wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 35 "keywords.gperf"
      {"short",                              TK_SHORT},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 28 "keywords.gperf"
      {"if",                                 TK_IF},
#line 56 "keywords.gperf"
      {"_Thread_local",                      TK_THREAD_LOCAL},
      {""}, {""}, {""}, {""}, {""},
#line 54 "keywords.gperf"
      {"_Noreturn",                          TK_NORETURN},
      {""}, {""}, {""}, {""}, {""},
#line 42 "keywords.gperf"
      {"union",                              TK_UNION},
#line 60 "keywords.gperf"
      {"ifndef",                             TK_IFNDEF},
      {""},
#line 67 "keywords.gperf"
      {"asm",                                TK_ASM},
#line 23 "keywords.gperf"
      {"enum",                               TK_ENUM},
#line 50 "keywords.gperf"
      {"_Bool",                              TK_BOOL},
#line 37 "keywords.gperf"
      {"sizeof",                             TK_SIZEOF},
      {""}, {""}, {""}, {""}, {""},
#line 20 "keywords.gperf"
      {"do",                                 TK_DO},
      {""},
#line 13 "keywords.gperf"
      {"auto",                               TK_AUTO},
      {""}, {""}, {""},
#line 43 "keywords.gperf"
      {"unsigned",                           TK_UNSIGNED},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 119 "keywords.gperf"
      {"__hot__",                            TK_HOT},
#line 152 "keywords.gperf"
      {"__used__",                           TK_USED},
#line 44 "keywords.gperf"
      {"void",                               TK_VOID},
      {""},
#line 160 "keywords.gperf"
      {"__optnone__",                        TK_OPTNONE},
#line 49 "keywords.gperf"
      {"_Atomic",                            TK_ATOMIC},
      {""},
#line 55 "keywords.gperf"
      {"_Static_assert",                     TK_STATIC_ASSERT},
      {""},
#line 21 "keywords.gperf"
      {"double",                             TK_DOUBLE},
      {""},
#line 30 "keywords.gperf"
      {"int",                                TK_INT},
      {""},
#line 114 "keywords.gperf"
      {"__format__",                         TK_FORMAT},
#line 38 "keywords.gperf"
      {"static",                             TK_STATIC},
#line 148 "keywords.gperf"
      {"__sentinel__",                       TK_SENTINEL},
#line 143 "keywords.gperf"
      {"__pure__",                           TK_PURE},
#line 118 "keywords.gperf"
      {"__format_arg__",                     TK_FORMAT_ARG},
#line 130 "keywords.gperf"
      {"__no_sanitize__",                    TK_NO_SANITIZE},
#line 141 "keywords.gperf"
      {"__nothrow__",                        TK_NOTHROW},
#line 142 "keywords.gperf"
      {"__optimize__",                       TK_OPTIMIZE},
#line 149 "keywords.gperf"
      {"__simd__",                           TK_SIMD},
#line 129 "keywords.gperf"
      {"__no_reorder__",                     TK_NO_REORDER},
#line 94 "keywords.gperf"
      {"__strong__",                         TK_STRONG},
#line 88 "keywords.gperf"
      {"__pointer__",                        TK_POINTER},
#line 133 "keywords.gperf"
      {"__no_sanitize_thread__",             TK_NO_SANITIZE_THREAD},
#line 131 "keywords.gperf"
      {"__no_sanitize_address__",            TK_NO_SANITIZE_ADDRESS},
#line 138 "keywords.gperf"
      {"__noipa__",                          TK_NOIPA},
#line 134 "keywords.gperf"
      {"__no_sanitize_undefined__",          TK_NO_SANITIZE_UNDEFINED},
#line 92 "keywords.gperf"
      {"__strfmon__",                        TK_STRFMON},
#line 76 "keywords.gperf"
      {"__force_align_arg_pointer__",        TK_FORCE_ALIGN_ARG_POINTER},
#line 26 "keywords.gperf"
      {"for",                                TK_FOR},
      {""}, {""},
#line 139 "keywords.gperf"
      {"__nonnull__",                        TK_NONNULL},
#line 41 "keywords.gperf"
      {"typedef",                            TK_TYPEDEF},
      {""}, {""},
#line 158 "keywords.gperf"
      {"__ms_abi__",                         TK_MS_ABI},
#line 24 "keywords.gperf"
      {"extern",                             TK_EXTERN},
#line 93 "keywords.gperf"
      {"__strftime__",                       TK_STRFTIME},
#line 135 "keywords.gperf"
      {"__no_split_stack__",                 TK_NO_SPLIT_STACK},
#line 128 "keywords.gperf"
      {"__no_profile_instrument_function__", TK_NO_PROFILE_INSTRUMENT_FUNCTION},
      {""},
#line 81 "keywords.gperf"
      {"__auto_type",                        TK_AUTO_TYPE},
#line 75 "keywords.gperf"
      {"__noinline__",                       TK_LABEL},
#line 85 "keywords.gperf"
      {"__may_alias__",                      TK_MAY_ALIAS},
      {""}, {""},
#line 61 "keywords.gperf"
      {"define",                             TK_DEFINE},
#line 80 "keywords.gperf"
      {"__asm__",                            TK_ASM},
#line 51 "keywords.gperf"
      {"_Complex",                           TK_COMPLEX},
#line 123 "keywords.gperf"
      {"__no_caller_saved_registers__",      TK_NO_CALLER_SAVED_REGISTERS},
#line 95 "keywords.gperf"
      {"__target__",                         TK_TARGET},
      {""}, {""}, {""},
#line 99 "keywords.gperf"
      {"__alias__",                          TK_ALIAS},
      {""},
#line 100 "keywords.gperf"
      {"__aligned__",                        TK_ALIGNED},
#line 150 "keywords.gperf"
      {"__target_clones__",                  TK_TARGET_CLONES},
      {""},
#line 103 "keywords.gperf"
      {"__artificial__",                     TK_ARTIFICIAL},
      {""},
#line 79 "keywords.gperf"
      {"__alignof__",                        TK_ALIGNOF},
#line 86 "keywords.gperf"
      {"__noreturn__",                       TK_NORETURN},
#line 155 "keywords.gperf"
      {"__params_nonnull__",                 TK_PARAMS_NONNULL},
#line 102 "keywords.gperf"
      {"__alloc_size__",                     TK_ALLOC_SIZE},
#line 101 "keywords.gperf"
      {"__alloc_align__",                    TK_ALLOC_ALIGN},
#line 127 "keywords.gperf"
      {"__no_instrument_function__",         TK_NO_INSTRUMENT_FUNCTION},
      {""},
#line 121 "keywords.gperf"
      {"__interrupt__",                      TK_INTERRUPT},
#line 110 "keywords.gperf"
      {"__error__",                          TK_ERROR},
      {""}, {""},
#line 112 "keywords.gperf"
      {"__externally_visible__",             TK_EXTERNALLY_VISIBLE},
#line 72 "keywords.gperf"
      {"__inline",                           TK_INLINE},
#line 27 "keywords.gperf"
      {"goto",                               TK_GOTO},
#line 17 "keywords.gperf"
      {"const",                              TK_CONST},
#line 122 "keywords.gperf"
      {"__interrupt_handler__",              TK_INTERRUPT_HANDLER},
#line 97 "keywords.gperf"
      {"__volatile__",                       TK_VOLATILE},
#line 159 "keywords.gperf"
      {"__mode__",                           TK_MODE},
#line 140 "keywords.gperf"
      {"__noplt__",                          TK_NOPLT},
#line 25 "keywords.gperf"
      {"float",                              TK_FLOAT},
      {""}, {""}, {""}, {""}, {""},
#line 66 "keywords.gperf"
      {"pragma",                             TK_PRAGMA},
#line 19 "keywords.gperf"
      {"default",                            TK_DEFAULT},
#line 104 "keywords.gperf"
      {"__assume_aligned__",                 TK_ASSUME_ALIGNED},
#line 31 "keywords.gperf"
      {"long",                               TK_LONG},
#line 132 "keywords.gperf"
      {"__no_address_safety_analysis__",     TK_NO_ADDRESS_SAFETY_ANALYSIS},
#line 137 "keywords.gperf"
      {"__noclone__",                        TK_NOCLONE},
      {""},
#line 18 "keywords.gperf"
      {"continue",                           TK_CONTINUE},
#line 120 "keywords.gperf"
      {"__ifunc__",                          TK_IFUNC},
#line 53 "keywords.gperf"
      {"_Imaginary",                         TK_IMAGINARY},
#line 147 "keywords.gperf"
      {"__section__",                        TK_SECTION},
      {""},
#line 52 "keywords.gperf"
      {"_Generic",                           TK_GENERIC},
#line 153 "keywords.gperf"
      {"__visibility__",                     TK_VISIBILITY},
#line 151 "keywords.gperf"
      {"__unused__",                         TK_UNUSED},
#line 36 "keywords.gperf"
      {"signed",                             TK_SIGNED},
      {""}, {""},
#line 16 "keywords.gperf"
      {"char",                               TK_CHAR},
#line 46 "keywords.gperf"
      {"while",                              TK_WHILE},
      {""},
#line 77 "keywords.gperf"
      {"__always_inline__",                  TK_ALWAYS_INLINE},
      {""}, {""}, {""},
#line 161 "keywords.gperf"
      {"__nodebug__",                        TK_NODEBUG},
      {""},
#line 33 "keywords.gperf"
      {"restrict",                           TK_RESTRICT},
#line 15 "keywords.gperf"
      {"case",                               TK_CASE},
      {""}, {""}, {""},
#line 82 "keywords.gperf"
      {"__byte__",                           TK_BYTE},
      {""},
#line 125 "keywords.gperf"
      {"__malloc__",                         TK_MALLOC},
#line 113 "keywords.gperf"
      {"__flatten__",                        TK_FLATTEN},
      {""},
#line 45 "keywords.gperf"
      {"volatile",                           TK_VOLATILE},
      {""},
#line 62 "keywords.gperf"
      {"undef",                              TK_UNDEF},
      {""}, {""}, {""},
#line 91 "keywords.gperf"
      {"__scanf__",                          TK_SCANF},
      {""}, {""},
#line 146 "keywords.gperf"
      {"__returns_twice__",                  TK_RETURNS_TWICE},
      {""},
#line 145 "keywords.gperf"
      {"__returns_nonnull__",                TK_RETURNS_NONNULL},
#line 157 "keywords.gperf"
      {"__vector_size__",                    TK_VECTOR_SIZE},
      {""}, {""},
#line 136 "keywords.gperf"
      {"__no_stack_limit__",                 TK_NO_STACK_LIMIT},
      {""},
#line 126 "keywords.gperf"
      {"__no_icf__",                         TK_NO_ICF},
#line 34 "keywords.gperf"
      {"return",                             TK_RETURN},
      {""},
#line 144 "keywords.gperf"
      {"__patchable_function_entry__",       TK_PATCHABLE_FUNCTION_ENTRY},
#line 64 "keywords.gperf"
      {"line",                               TK_LINE},
#line 87 "keywords.gperf"
      {"__packed__",                         TK_PACKED},
#line 29 "keywords.gperf"
      {"inline",                             TK_INLINE},
      {""}, {""},
#line 22 "keywords.gperf"
      {"else",                               TK_ELSE},
#line 89 "keywords.gperf"
      {"__printf__",                         TK_PRINTF},
      {""}, {""},
#line 98 "keywords.gperf"
      {"__word__",                           TK_WORD},
      {""}, {""},
#line 111 "keywords.gperf"
      {"__warning__",                        TK_WARNING},
      {""}, {""},
#line 73 "keywords.gperf"
      {"__const__",                          TK_CONST},
      {""},
#line 39 "keywords.gperf"
      {"struct",                             TK_STRUCT},
      {""}, {""},
#line 107 "keywords.gperf"
      {"__destructor__",                     TK_DESTRUCTOR},
      {""}, {""}, {""},
#line 47 "keywords.gperf"
      {"_Alignas",                           TK_ALIGNAS},
      {""},
#line 59 "keywords.gperf"
      {"ifdef",                              TK_IFDEF},
#line 83 "keywords.gperf"
      {"__complex__",                        TK_COMPLEX},
#line 63 "keywords.gperf"
      {"include",                            TK_INCLUDE},
#line 84 "keywords.gperf"
      {"__imag__",                           TK_IMAG},
#line 78 "keywords.gperf"
      {"__gnu_inline__",                     TK_GNU_INLINE},
#line 106 "keywords.gperf"
      {"__constructor__",                    TK_CONSTRUCTOR},
      {""}, {""},
#line 105 "keywords.gperf"
      {"__cold__",                           TK_COLD},
#line 115 "keywords.gperf"
      {"__gnu_format__",                     TK_GNU_FORMAT},
      {""}, {""},
#line 69 "keywords.gperf"
      {"__restrict__",                       TK_RESTRICT},
      {""},
#line 116 "keywords.gperf"
      {"__gnu_printf__",                     TK_GNU_PRINTF},
#line 58 "keywords.gperf"
      {"endif",                              TK_ENDIF},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 57 "keywords.gperf"
      {"elif",                               TK_ELIF},
      {""},
#line 40 "keywords.gperf"
      {"switch",                             TK_SWITCH},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 68 "keywords.gperf"
      {"__attribute__",                      TK_ATTRIBUTE},
      {""}, {""}, {""},
#line 154 "keywords.gperf"
      {"__warn_unused_result__",             TK_WARN_UNUSED_RESULT},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 65 "keywords.gperf"
      {"error",                              TK_ERROR},
      {""}, {""},
#line 108 "keywords.gperf"
      {"__copy__",                           TK_COPY},
      {""}, {""}, {""}, {""}, {""},
#line 109 "keywords.gperf"
      {"__deprecated__",                     TK_DEPRECATED},
      {""}, {""}, {""},
#line 71 "keywords.gperf"
      {"__typeof",                           TK_TYPEOF},
      {""},
#line 70 "keywords.gperf"
      {"__typeof__",                         TK_TYPEOF},
#line 96 "keywords.gperf"
      {"__transparent_union__",              TK_TRANSPARENT_UNION},
      {""},
#line 90 "keywords.gperf"
      {"__real__",                           TK_REAL},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 117 "keywords.gperf"
      {"__gnu_scanf__",                      TK_GNU_SCANF},
      {""}, {""}, {""}, {""},
#line 48 "keywords.gperf"
      {"_Alignof",                           TK_ALIGNOF},
      {""}, {""}, {""}, {""},
#line 156 "keywords.gperf"
      {"__weak__",                           TK_WEAK},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 32 "keywords.gperf"
      {"register",                           TK_REGISTER},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
#line 124 "keywords.gperf"
      {"__leaf__",                           TK_LEAF},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 74 "keywords.gperf"
      {"__label__",                          TK_LABEL},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 14 "keywords.gperf"
      {"break",                              TK_BREAK}
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
