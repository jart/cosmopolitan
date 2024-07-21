#ifndef COSMOPOLITAN_WCTYPE_H_
#define COSMOPOLITAN_WCTYPE_H_
COSMOPOLITAN_C_START_

#ifndef WEOF
#define WEOF -1u
#endif

typedef unsigned wctype_t;
typedef const int *wctrans_t;

int iswalnum(wint_t) libcesque;
int iswalpha(wint_t) libcesque;
int iswblank(wint_t) libcesque;
int iswcntrl(wint_t) libcesque;
int iswdigit(wint_t) libcesque;
int iswgraph(wint_t) libcesque;
int iswlower(wint_t) libcesque;
int iswspace(wint_t) libcesque;
int iswupper(wint_t) libcesque;
int iswxdigit(wint_t) libcesque;
int iswpunct(wint_t) libcesque;
int iswprint(wint_t) libcesque;
int iswseparator(wint_t) libcesque;
wint_t towlower(wint_t) libcesque;
wint_t towupper(wint_t) libcesque;

wctype_t wctype(const char *) strlenesque;
pureconst int iswctype(wint_t, wctype_t) libcesque;

wctrans_t wctrans(const char *) libcesque;
wint_t towctrans(wint_t, wctrans_t) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_WCTYPE_H_ */
