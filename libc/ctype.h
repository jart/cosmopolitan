#ifndef COSMOPOLITAN_CTYPE_H_
#define COSMOPOLITAN_CTYPE_H_
COSMOPOLITAN_C_START_

#define _tolower(u) (0040 | (u))
#define _toupper(u) (0137 & (u))

int isascii(int) libcesque;
int isspace(int) libcesque;
int isalpha(int) libcesque;
int isdigit(int) libcesque;
int isalnum(int) libcesque;
int isxdigit(int) libcesque;
int isprint(int) libcesque;
int islower(int) libcesque;
int isupper(int) libcesque;
int isblank(int) libcesque;
int iscntrl(int) libcesque;
int isgraph(int) libcesque;
int tolower(int) libcesque;
int ispunct(int) libcesque;
int toupper(int) libcesque;
int toascii(int) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_CTYPE_H_ */
