#ifndef Py_IMPORT_H
#define Py_IMPORT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

#ifndef Py_LIMITED_API
void _PyImportZip_Init(void);

PyMODINIT_FUNC PyInit_imp(void);
#endif /* !Py_LIMITED_API */
long PyImport_GetMagicNumber(void);
const char * PyImport_GetMagicTag(void);
PyObject * PyImport_ExecCodeModule(
    const char *name,           /* UTF-8 encoded string */
    PyObject *co
    );
PyObject * PyImport_ExecCodeModuleEx(
    const char *name,           /* UTF-8 encoded string */
    PyObject *co,
    const char *pathname        /* decoded from the filesystem encoding */
    );
PyObject * PyImport_ExecCodeModuleWithPathnames(
    const char *name,           /* UTF-8 encoded string */
    PyObject *co,
    const char *pathname,       /* decoded from the filesystem encoding */
    const char *cpathname       /* decoded from the filesystem encoding */
    );
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
PyObject * PyImport_ExecCodeModuleObject(
    PyObject *name,
    PyObject *co,
    PyObject *pathname,
    PyObject *cpathname
    );
#endif
PyObject * PyImport_GetModuleDict(void);
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
PyObject * PyImport_AddModuleObject(
    PyObject *name
    );
#endif
PyObject * PyImport_AddModule(
    const char *name            /* UTF-8 encoded string */
    );
PyObject * PyImport_ImportModule(
    const char *name            /* UTF-8 encoded string */
    );
PyObject * PyImport_ImportModuleNoBlock(
    const char *name            /* UTF-8 encoded string */
    );
PyObject * PyImport_ImportModuleLevel(
    const char *name,           /* UTF-8 encoded string */
    PyObject *globals,
    PyObject *locals,
    PyObject *fromlist,
    int level
    );
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03050000
PyObject * PyImport_ImportModuleLevelObject(
    PyObject *name,
    PyObject *globals,
    PyObject *locals,
    PyObject *fromlist,
    int level
    );
#endif

#define PyImport_ImportModuleEx(n, g, l, f) \
    PyImport_ImportModuleLevel(n, g, l, f, 0)

PyObject * PyImport_GetImporter(PyObject *path);
PyObject * PyImport_Import(PyObject *name);
PyObject * PyImport_ReloadModule(PyObject *m);
void PyImport_Cleanup(void);
void _PyImportLookupTables_Init(void);
void _PyImportLookupTables_Cleanup(void);
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03030000
int PyImport_ImportFrozenModuleObject(
    PyObject *name
    );
#endif
int PyImport_ImportFrozenModule(
    const char *name            /* UTF-8 encoded string */
    );

#ifndef Py_LIMITED_API
#ifdef WITH_THREAD
void _PyImport_AcquireLock(void);
int _PyImport_ReleaseLock(void);
#else
#define _PyImport_AcquireLock()
#define _PyImport_ReleaseLock() 1
#endif

void _PyImport_ReInitLock(void);

PyObject * _PyImport_FindBuiltin(
    const char *name            /* UTF-8 encoded string */
    );
PyObject * _PyImport_FindExtensionObject(PyObject *, PyObject *);
int _PyImport_FixupBuiltin(
    PyObject *mod,
    const char *name            /* UTF-8 encoded string */
    );
int _PyImport_FixupExtensionObject(PyObject*, PyObject *, PyObject *);

struct _inittab {
    const char *name;           /* ASCII encoded string */
    PyObject* (*initfunc)(void);
};
extern const struct _inittab _PyImport_Inittab[];
extern struct _inittab * PyImport_Inittab;
int PyImport_ExtendInittab(struct _inittab *newtab);
#endif /* Py_LIMITED_API */

extern PyTypeObject PyNullImporter_Type;

int PyImport_AppendInittab(
    const char *name,           /* ASCII encoded string */
    PyObject* (*initfunc)(void)
    );

#ifndef Py_LIMITED_API
struct _frozen {
    const char *name;                 /* ASCII encoded string */
    const unsigned char *code;
    int size;
};

/* Embedding apps may change this pointer to point to their favorite
   collection of frozen modules: */

extern const struct _frozen * PyImport_FrozenModules;
extern const struct _frozen _PyImport_FrozenModules[];
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_IMPORT_H */
