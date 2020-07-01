#ifndef COSMOPOLITAN_THIRD_PARTY_DTOA_DTOA_H_
#define COSMOPOLITAN_THIRD_PARTY_DTOA_DTOA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *g_fmt(char *buf /*[32]*/, double x);
char *dtoa(double d, int mode, int ndigits, int *decpt, int *sign,
           char **rve) nodiscard;
void freedtoa(char *s);
char *dtoa_r(double dd, int mode, int ndigits, int *decpt, int *sign,
             char **rve, char *buf, size_t blen);

double strtod(const char *, char **);
double plan9_strtod(const char *, char **);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_DTOA_DTOA_H_ */
