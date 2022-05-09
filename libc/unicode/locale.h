#ifndef COSMOPOLITAN_LIBC_UNICODE_LOCALE_H_
#define COSMOPOLITAN_LIBC_UNICODE_LOCALE_H_
#include "libc/fmt/conv.h"

#define LC_CTYPE         0
#define LC_NUMERIC       1
#define LC_CTYPE_MASK    1
#define LC_TIME          2
#define LC_NUMERIC_MASK  2
#define LC_COLLATE       3
#define LC_MONETARY      4
#define LC_TIME_MASK     4
#define LC_MESSAGES      5
#define LC_ALL           6
#define LC_COLLATE_MASK  8
#define LC_MONETARY_MASK 16
#define LC_MESSAGES_MASK 32
#define LC_ALL_MASK      0x1fbf

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct __locale_struct;
typedef struct __locale_struct *locale_t;

char *setlocale(int, const char *);
locale_t uselocale(locale_t);
locale_t newlocale(int, const char *, locale_t);
long long strtoll_l(const char *, char **, int, locale_t);
unsigned long long strtoull_l(const char *, char **, int, locale_t);
long long wcstoll_l(const wchar_t *, wchar_t **, int, locale_t);
unsigned long long wcstoull_l(const wchar_t *, wchar_t **, int, locale_t);
float strtof_l(const char *, char **, locale_t);
float wcstof_l(const wchar_t *, wchar_t **, locale_t);
double wcstod_l(const wchar_t *, wchar_t **, locale_t);
long double wcstold_l(const wchar_t *, wchar_t **, locale_t);
double strtod_l(const char *, char **, locale_t);
long double strtold_l(const char *, char **, locale_t);
int isxdigit_l(int, locale_t);
int isdigit_l(int, locale_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_UNICODE_LOCALE_H_ */
