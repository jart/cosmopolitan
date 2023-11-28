#ifndef Py_ODICTOBJECT_H
#define Py_ODICTOBJECT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

/* OrderedDict */
/* This API is optional and mostly redundant. */

#ifndef Py_LIMITED_API

typedef struct _odictobject PyODictObject;

extern PyTypeObject PyODict_Type;
extern PyTypeObject PyODictIter_Type;
extern PyTypeObject PyODictKeys_Type;
extern PyTypeObject PyODictItems_Type;
extern PyTypeObject PyODictValues_Type;

#define PyODict_Check(op) PyObject_TypeCheck(op, &PyODict_Type)
#define PyODict_CheckExact(op) (Py_TYPE(op) == &PyODict_Type)
#define PyODict_SIZE(op) ((PyDictObject *)op)->ma_used

PyObject * PyODict_New(void);
int PyODict_SetItem(PyObject *od, PyObject *key, PyObject *item);
int PyODict_DelItem(PyObject *od, PyObject *key);

/* wrappers around PyDict* functions */
#define PyODict_GetItem(od, key) PyDict_GetItem((PyObject *)od, key)
#define PyODict_GetItemWithError(od, key) \
    PyDict_GetItemWithError((PyObject *)od, key)
#define PyODict_Contains(od, key) PyDict_Contains((PyObject *)od, key)
#define PyODict_Size(od) PyDict_Size((PyObject *)od)
#define PyODict_GetItemString(od, key) \
    PyDict_GetItemString((PyObject *)od, key)

#endif

COSMOPOLITAN_C_END_
#endif /* !Py_ODICTOBJECT_H */
