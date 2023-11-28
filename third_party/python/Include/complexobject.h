#ifndef Py_COMPLEXOBJECT_H
#define Py_COMPLEXOBJECT_H
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/unicodeobject.h"
COSMOPOLITAN_C_START_

#ifndef Py_LIMITED_API
typedef struct {
    double real;
    double imag;
} Py_complex;

/* Operations on complex numbers from complexmodule.c */

Py_complex _Py_c_sum(Py_complex, Py_complex);
Py_complex _Py_c_diff(Py_complex, Py_complex);
Py_complex _Py_c_neg(Py_complex);
Py_complex _Py_c_prod(Py_complex, Py_complex);
Py_complex _Py_c_quot(Py_complex, Py_complex);
Py_complex _Py_c_pow(Py_complex, Py_complex);
double _Py_c_abs(Py_complex);
#endif

/* Complex object interface */

/*
PyComplexObject represents a complex number with double-precision
real and imaginary parts.
*/
#ifndef Py_LIMITED_API
typedef struct {
    PyObject_HEAD
    Py_complex cval;
} PyComplexObject;
#endif

extern PyTypeObject PyComplex_Type;

#define PyComplex_Check(op) PyObject_TypeCheck(op, &PyComplex_Type)
#define PyComplex_CheckExact(op) (Py_TYPE(op) == &PyComplex_Type)

#ifndef Py_LIMITED_API
PyObject * PyComplex_FromCComplex(Py_complex);
#endif
PyObject * PyComplex_FromDoubles(double real, double imag);

double PyComplex_RealAsDouble(PyObject *op);
double PyComplex_ImagAsDouble(PyObject *op);
#ifndef Py_LIMITED_API
Py_complex PyComplex_AsCComplex(PyObject *op);
#endif

/* Format the object based on the format_spec, as defined in PEP 3101
   (Advanced String Formatting). */
#ifndef Py_LIMITED_API
int _PyComplex_FormatAdvancedWriter(
    _PyUnicodeWriter *writer,
    PyObject *obj,
    PyObject *format_spec,
    Py_ssize_t start,
    Py_ssize_t end);
#endif

COSMOPOLITAN_C_END_
#endif /* !Py_COMPLEXOBJECT_H */
