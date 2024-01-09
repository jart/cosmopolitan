#ifndef COSMOPOLITAN_LIBC_STR_NLTYPES_H_
#define COSMOPOLITAN_LIBC_STR_NLTYPES_H_

#define NL_SETD       1
#define NL_CAT_LOCALE 1

COSMOPOLITAN_C_START_

typedef int nl_item;
typedef void *nl_catd;

nl_catd catopen(const char *, int) libcesque;
char *catgets(nl_catd, int, int, const char *) libcesque;
int catclose(nl_catd) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STR_NLTYPES_H_ */
