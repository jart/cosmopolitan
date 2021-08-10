#ifndef Py_PYGETOPT_H
#define Py_PYGETOPT_H
COSMOPOLITAN_C_START_
/* clang-format off */

#ifndef Py_LIMITED_API
PyAPI_DATA(int) _PyOS_opterr;
PyAPI_DATA(int) _PyOS_optind;
PyAPI_DATA(wchar_t *) _PyOS_optarg;

PyAPI_FUNC(void) _PyOS_ResetGetOpt(void);

PyAPI_FUNC(int) _PyOS_GetOpt(int argc, wchar_t **argv, wchar_t *optstring);
#endif /* !Py_LIMITED_API */

COSMOPOLITAN_C_END_
#endif /* !Py_PYGETOPT_H */
