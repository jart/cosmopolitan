#ifndef COSMOPOLITAN_LIBC_STR_LOCALE_H_
#define COSMOPOLITAN_LIBC_STR_LOCALE_H_
#include "libc/fmt/conv.h"
#include "libc/time/struct/tm.h"

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
#define LOCALE_NAME_MAX  23

COSMOPOLITAN_C_START_

#define LC_GLOBAL_LOCALE ((locale_t)-1)

struct __locale_map {
  const void *map;
  size_t map_size;
  char name[LOCALE_NAME_MAX + 1];
  const struct __locale_map *next;
};

struct __locale_struct {
  const struct __locale_map *cat[6];
};

typedef struct __locale_struct *locale_t;

extern const struct __locale_map __c_dot_utf8;
extern const struct __locale_struct __c_locale;
extern const struct __locale_struct __c_dot_utf8_locale;

char *nl_langinfo_l(int, locale_t) libcesque;
char *setlocale(int, const char *) libcesque;
double strtod_l(const char *, char **, locale_t) libcesque;
double wcstod_l(const wchar_t *, wchar_t **, locale_t) libcesque;
float strtof_l(const char *, char **, locale_t) libcesque;
float wcstof_l(const wchar_t *, wchar_t **, locale_t) libcesque;
int isdigit_l(int, locale_t) libcesque;
int islower_l(int, locale_t) libcesque;
int isupper_l(int, locale_t) libcesque;
int iswalpha_l(wint_t, locale_t) libcesque;
int iswblank_l(wint_t, locale_t) libcesque;
int iswcntrl_l(wint_t, locale_t) libcesque;
int iswdigit_l(wint_t, locale_t) libcesque;
int iswlower_l(wint_t, locale_t) libcesque;
int iswprint_l(wint_t, locale_t) libcesque;
int iswpunct_l(wint_t, locale_t) libcesque;
int iswspace_l(wint_t, locale_t) libcesque;
int iswupper_l(wint_t, locale_t) libcesque;
int iswxdigit_l(wint_t, locale_t) libcesque;
int isxdigit_l(int, locale_t) libcesque;
int strcoll_l(const char *, const char *, locale_t) libcesque;
int tolower_l(int, locale_t) libcesque;
int toupper_l(int, locale_t) libcesque;
int wcscoll_l(const wchar_t *, const wchar_t *, locale_t) libcesque;
locale_t duplocale(locale_t) libcesque;
locale_t newlocale(int, const char *, locale_t) libcesque;
locale_t uselocale(locale_t) libcesque;
long double strtold_l(const char *, char **, locale_t) libcesque;
long double wcstold_l(const wchar_t *, wchar_t **, locale_t) libcesque;
long long strtoll_l(const char *, char **, int, locale_t) libcesque;
long long wcstoll_l(const wchar_t *, wchar_t **, int, locale_t) libcesque;
size_t strftime_l(char *, size_t, char const *, struct tm const *, locale_t);
size_t strxfrm_l(char *, const char *, size_t, locale_t) libcesque;
size_t wcsxfrm_l(wchar_t *, const wchar_t *, size_t, locale_t) libcesque;
unsigned long long strtoull_l(const char *, char **, int, locale_t) libcesque;
unsigned long long wcstoull_l(const wchar_t *, wchar_t **, int, locale_t);
void freelocale(locale_t) libcesque;
wint_t towlower_l(wint_t, locale_t) libcesque;
wint_t towupper_l(wint_t, locale_t) libcesque;
int strcasecmp_l(const char *, const char *, locale_t) libcesque;
int strncasecmp_l(const char *, const char *, size_t, locale_t) libcesque;
ssize_t strfmon_l(char *, size_t, locale_t, const char *, ...) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STR_LOCALE_H_ */
