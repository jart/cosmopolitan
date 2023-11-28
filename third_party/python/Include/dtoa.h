#ifndef Py_LIMITED_API
#ifndef PY_NO_SHORT_FLOAT_REPR
COSMOPOLITAN_C_START_

double _Py_dg_strtod(const char *str, char **ptr);
char * _Py_dg_dtoa(double d, int mode, int ndigits,
                        int *decpt, int *sign, char **rve);
void _Py_dg_freedtoa(char *s);
double _Py_dg_stdnan(int sign);
double _Py_dg_infinity(int sign);

COSMOPOLITAN_C_END_
#endif
#endif
