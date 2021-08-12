#ifndef Py_IMPORTDL_H
#define Py_IMPORTDL_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

extern const char *_PyImport_DynLoadFiletab[];

extern PyObject *_PyImport_LoadDynamicModuleWithSpec(PyObject *spec, FILE *);

/* Max length of module suffix searched for -- accommodates "module.slb" */
#define MAXSUFFIXSIZE 12

typedef void (*dl_funcptr)(void);

COSMOPOLITAN_C_END_
#endif /* !Py_IMPORTDL_H */
