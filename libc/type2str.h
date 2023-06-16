#ifndef COSMOPOLITAN_LIBC_TYPE2STR_H_
#define COSMOPOLITAN_LIBC_TYPE2STR_H_
#if __STDC_VERSION__ + 0 >= 201112
/* clang-format off */

#define _TYPE2STR(X)                          \
  _Generic(X,                                 \
    _Bool:              "_Bool",              \
    signed char:        "signed char",        \
    unsigned char:      "unsigned char",      \
    char:               "char",               \
    short:              "short",              \
    unsigned short:     "unsigned short",     \
    int:                "int",                \
    unsigned:           "unsigned",           \
    long:               "long",               \
    unsigned long:      "unsigned long",      \
    long long:          "long long",          \
    unsigned long long: "unsigned long long", \
    __int128:           "__int128",           \
    unsigned __int128:  "unsigned __int128",  \
    float:              "float",              \
    double:             "double",             \
    long double:        "long double")

#define _PRINTF_GENERIC(X, D, U) \
  _Generic(X,                    \
    _Bool:              "hhh" U, \
    signed char:        "hh" D,  \
    unsigned char:      "hh" U,  \
    char:               "hh" D,  \
    short:              "h" D,   \
    unsigned short:     "h" U,   \
    int:                D,       \
    unsigned:           U,       \
    long:               "l" D,   \
    unsigned long:      "l" U,   \
    long long:          "ll" D,  \
    unsigned long long: "ll" U,  \
    float:              "f",     \
    double:             "f",     \
    long double:        "Lf")

/* clang-format on */
#endif /* C11 */
#endif /* COSMOPOLITAN_LIBC_TYPE2STR_H_ */
