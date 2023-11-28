#ifndef Py_SYSMODULE_H
#define Py_SYSMODULE_H
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pyport.h"
COSMOPOLITAN_C_START_

PyObject * PySys_GetObject(const char *);
int PySys_SetObject(const char *, PyObject *);
#ifndef Py_LIMITED_API
PyObject * _PySys_GetObjectId(_Py_Identifier *key);
int _PySys_SetObjectId(_Py_Identifier *key, PyObject *);
#endif

void PySys_SetArgv(int, wchar_t **);
void PySys_SetArgvEx(int, wchar_t **, int);
void PySys_SetPath(const wchar_t *);
void PySys_FormatStdout(const char *, ...);
void PySys_FormatStderr(const char *, ...);
void PySys_ResetWarnOptions(void);
void PySys_AddWarnOption(const wchar_t *);
void PySys_AddWarnOptionUnicode(PyObject *);
int PySys_HasWarnOptions(void);
void PySys_AddXOption(const wchar_t *);
PyObject * PySys_GetXOptions(void);

#ifndef Py_LIMITED_API
size_t _PySys_GetSizeOf(PyObject *);
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_SYSMODULE_H */
