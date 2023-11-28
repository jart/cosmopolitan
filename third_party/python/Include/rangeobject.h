#ifndef Py_RANGEOBJECT_H
#define Py_RANGEOBJECT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

/*
A range object represents an integer range.  This is an immutable object;
a range cannot change its value after creation.

Range objects behave like the corresponding tuple objects except that
they are represented by a start, stop, and step datamembers.
*/

extern PyTypeObject PyRange_Type;
extern PyTypeObject PyRangeIter_Type;
extern PyTypeObject PyLongRangeIter_Type;

#define PyRange_Check(op) (Py_TYPE(op) == &PyRange_Type)

COSMOPOLITAN_C_END_
#endif /* !Py_RANGEOBJECT_H */
