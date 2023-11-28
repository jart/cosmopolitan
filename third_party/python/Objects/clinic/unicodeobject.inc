/*[clinic input]
preserve
[clinic start generated code]*/

PyDoc_STRVAR(unicode_maketrans__doc__,
"maketrans(x, y=None, z=None, /)\n"
"--\n"
"\n"
"Return a translation table usable for str.translate().\n"
"\n"
"If there is only one argument, it must be a dictionary mapping Unicode\n"
"ordinals (integers) or characters to Unicode ordinals, strings or None.\n"
"Character keys will be then converted to ordinals.\n"
"If there are two arguments, they must be strings of equal length, and\n"
"in the resulting dictionary, each character in x will be mapped to the\n"
"character at the same position in y. If there is a third argument, it\n"
"must be a string, whose characters will be mapped to None in the result.");

#define UNICODE_MAKETRANS_METHODDEF    \
    {"maketrans", (PyCFunction)unicode_maketrans, METH_FASTCALL|METH_STATIC, unicode_maketrans__doc__},

static PyObject *
unicode_maketrans_impl(PyObject *x, PyObject *y, PyObject *z);

static PyObject *
unicode_maketrans(void *null, PyObject **args, Py_ssize_t nargs)
{
    PyObject *return_value = NULL;
    PyObject *x;
    PyObject *y = NULL;
    PyObject *z = NULL;

    if (!_PyArg_ParseStack(args, nargs, "O|UU:maketrans",
        &x, &y, &z)) {
        goto exit;
    }
    return_value = unicode_maketrans_impl(x, y, z);

exit:
    return return_value;
}
/*[clinic end generated code: output=d1e48260a99031c2 input=a9049054013a1b77]*/
