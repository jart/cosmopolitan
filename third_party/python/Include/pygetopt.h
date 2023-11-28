#ifndef Py_PYGETOPT_H
#define Py_PYGETOPT_H
COSMOPOLITAN_C_START_

#ifndef Py_LIMITED_API
extern int _PyOS_opterr;
extern int _PyOS_optind;
extern wchar_t * _PyOS_optarg;

void _PyOS_ResetGetOpt(void);

int _PyOS_GetOpt(int argc, wchar_t **argv, wchar_t *optstring);
#endif /* !Py_LIMITED_API */

COSMOPOLITAN_C_END_
#endif /* !Py_PYGETOPT_H */
