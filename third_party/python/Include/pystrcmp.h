#ifndef Py_STRCMP_H
#define Py_STRCMP_H
COSMOPOLITAN_C_START_
/* clang-format off */

PyAPI_FUNC(int) PyOS_mystrnicmp(const char *, const char *, Py_ssize_t);
PyAPI_FUNC(int) PyOS_mystricmp(const char *, const char *);

#ifdef MS_WINDOWS
#define PyOS_strnicmp strnicmp
#define PyOS_stricmp stricmp
#else
#define PyOS_strnicmp PyOS_mystrnicmp
#define PyOS_stricmp PyOS_mystricmp
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_STRCMP_H */
