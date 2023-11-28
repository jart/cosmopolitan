#ifndef Py_CAPSULE_H
#define Py_CAPSULE_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

/* Capsule objects let you wrap a C "void *" pointer in a Python
   object.  They're a way of passing data through the Python interpreter
   without creating your own custom type.

   Capsules are used for communication between extension modules.
   They provide a way for an extension module to export a C interface
   to other extension modules, so that extension modules can use the
   Python import mechanism to link to one another.

   For more information, please see "c-api/capsule.html" in the
   documentation.
*/

extern PyTypeObject PyCapsule_Type;

typedef void (*PyCapsule_Destructor)(PyObject *);

#define PyCapsule_CheckExact(op) (Py_TYPE(op) == &PyCapsule_Type)


PyObject * PyCapsule_New(
    void *pointer,
    const char *name,
    PyCapsule_Destructor destructor);

void * PyCapsule_GetPointer(PyObject *capsule, const char *name);

PyCapsule_Destructor PyCapsule_GetDestructor(PyObject *capsule);

const char * PyCapsule_GetName(PyObject *capsule);

void * PyCapsule_GetContext(PyObject *capsule);

int PyCapsule_IsValid(PyObject *capsule, const char *name);

int PyCapsule_SetPointer(PyObject *capsule, void *pointer);

int PyCapsule_SetDestructor(PyObject *capsule, PyCapsule_Destructor destructor);

int PyCapsule_SetName(PyObject *capsule, const char *name);

int PyCapsule_SetContext(PyObject *capsule, void *context);

void * PyCapsule_Import(
    const char *name,           /* UTF-8 encoded string */
    int no_block);

COSMOPOLITAN_C_END_
#endif /* !Py_CAPSULE_H */
