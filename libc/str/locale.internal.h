#ifndef COSMOPOLITAN_LIBC_STR_LOCALE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STR_LOCALE_INTERNAL_H_
#include "libc/limits.h"
#include "libc/str/locale.h"
#include "libc/thread/posixthread.internal.h"
COSMOPOLITAN_C_START_

#define LOCALE_NAME_MAX 23

struct __locale_map {
  const void *map;
  size_t map_size;
  char name[LOCALE_NAME_MAX + 1];
  const struct __locale_map *next;
};

struct __locale_struct {
  const struct __locale_map *cat[6];
};

extern pthread_mutex_t __locale_lock;

extern struct __locale_struct __global_locale;
extern const struct __locale_map __c_dot_utf8;
extern const struct __locale_struct __c_locale;
extern const struct __locale_struct __c_dot_utf8_locale;

const struct __locale_map *__get_locale(int, const char *);
const char *__mo_lookup(const void *, size_t, const char *);
const char *__lctrans(const char *, const struct __locale_map *);
const char *__lctrans_cur(const char *);
const char *__lctrans_impl(const char *, const struct __locale_map *);
int __loc_is_allocated(locale_t);
char *__gettextdomain(void);

#define LOC_MAP_FAILED ((const struct __locale_map *)-1)

#define LCTRANS(msg, lc, loc) __lctrans(msg, (loc)->cat[(lc)])
#define LCTRANS_CUR(msg)      __lctrans_cur(msg)

#define C_LOCALE    ((locale_t) & __c_locale)
#define UTF8_LOCALE ((locale_t) & __c_dot_utf8_locale)

#define CURRENT_LOCALE _pthread_self()->pt_locale

#define CURRENT_UTF8 (!!_pthread_self()->pt_locale->cat[LC_CTYPE])

#undef MB_CUR_MAX
#define MB_CUR_MAX (CURRENT_UTF8 ? 4 : 1)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STR_LOCALE_INTERNAL_H_ */
