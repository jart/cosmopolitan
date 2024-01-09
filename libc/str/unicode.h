#ifndef COSMOPOLITAN_LIBC_STR_UNICODE_H_
#define COSMOPOLITAN_LIBC_STR_UNICODE_H_
COSMOPOLITAN_C_START_

struct lconv {
  char *decimal_point;
  char *thousands_sep;
  char *grouping;
  char *int_curr_symbol;
  char *currency_symbol;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;
  char int_frac_digits;
  char frac_digits;
  char p_cs_precedes;
  char p_sep_by_space;
  char n_cs_precedes;
  char n_sep_by_space;
  char p_sign_posn;
  char n_sign_posn;
  char int_p_cs_precedes;
  char int_n_cs_precedes;
  char int_p_sep_by_space;
  char int_n_sep_by_space;
  char int_p_sign_posn;
  char int_n_sign_posn;
};

pureconst int wcwidth(wchar_t) libcesque;
int wcswidth(const wchar_t *, size_t) strlenesque;
struct lconv *localeconv(void) libcesque;

#ifdef _COSMO_SOURCE
int wcsnwidth(const wchar_t *, size_t, size_t) strlenesque;
#endif /* _COSMO_SOURCE */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STR_UNICODE_H_ */
