#ifndef Py_SETOBJECT_H
#define Py_SETOBJECT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

#ifndef Py_LIMITED_API

/* There are three kinds of entries in the table:

1. Unused:  key == NULL and hash == 0
2. Dummy:   key == dummy and hash == -1
3. Active:  key != NULL and key != dummy and hash != -1

The hash field of Unused slots is always zero.

The hash field of Dummy slots are set to -1
meaning that dummy entries can be detected by
either entry->key==dummy or by entry->hash==-1.
*/

#define PySet_MINSIZE 8

typedef struct {
    PyObject *key;
    Py_hash_t hash;             /* Cached hash code of the key */
} setentry;

/* The SetObject data structure is shared by set and frozenset objects.

Invariant for sets:
 - hash is -1

Invariants for frozensets:
 - data is immutable.
 - hash is the hash of the frozenset or -1 if not computed yet.

*/

typedef struct {
    PyObject_HEAD

    Py_ssize_t fill;            /* Number active and dummy entries*/
    Py_ssize_t used;            /* Number active entries */

    /* The table contains mask + 1 slots, and that's a power of 2.
     * We store the mask instead of the size because the mask is more
     * frequently needed.
     */
    Py_ssize_t mask;

    /* The table points to a fixed-size smalltable for small tables
     * or to additional malloc'ed memory for bigger tables.
     * The table pointer is never NULL which saves us from repeated
     * runtime null-tests.
     */
    setentry *table;
    Py_hash_t hash;             /* Only used by frozenset objects */
    Py_ssize_t finger;          /* Search finger for pop() */

    setentry smalltable[PySet_MINSIZE];
    PyObject *weakreflist;      /* List of weak references */
} PySetObject;

#define PySet_GET_SIZE(so) (((PySetObject *)(so))->used)

extern PyObject * _PySet_Dummy;

int _PySet_NextEntry(PyObject *set, Py_ssize_t *pos, PyObject **key, Py_hash_t *hash);
int _PySet_Update(PyObject *set, PyObject *iterable);
int PySet_ClearFreeList(void);

#endif /* Section excluded by Py_LIMITED_API */

extern PyTypeObject PySet_Type;
extern PyTypeObject PyFrozenSet_Type;
extern PyTypeObject PySetIter_Type;

PyObject * PySet_New(PyObject *);
PyObject * PyFrozenSet_New(PyObject *);

int PySet_Add(PyObject *set, PyObject *key);
int PySet_Clear(PyObject *set);
int PySet_Contains(PyObject *anyset, PyObject *key);
int PySet_Discard(PyObject *set, PyObject *key);
PyObject * PySet_Pop(PyObject *set);
Py_ssize_t PySet_Size(PyObject *anyset);

#define PyFrozenSet_CheckExact(ob) (Py_TYPE(ob) == &PyFrozenSet_Type)
#define PyAnySet_CheckExact(ob) \
    (Py_TYPE(ob) == &PySet_Type || Py_TYPE(ob) == &PyFrozenSet_Type)
#define PyAnySet_Check(ob) \
    (Py_TYPE(ob) == &PySet_Type || Py_TYPE(ob) == &PyFrozenSet_Type || \
      PyType_IsSubtype(Py_TYPE(ob), &PySet_Type) || \
      PyType_IsSubtype(Py_TYPE(ob), &PyFrozenSet_Type))
#define PySet_Check(ob) \
    (Py_TYPE(ob) == &PySet_Type || \
    PyType_IsSubtype(Py_TYPE(ob), &PySet_Type))
#define   PyFrozenSet_Check(ob) \
    (Py_TYPE(ob) == &PyFrozenSet_Type || \
      PyType_IsSubtype(Py_TYPE(ob), &PyFrozenSet_Type))

COSMOPOLITAN_C_END_
#endif /* !Py_SETOBJECT_H */
