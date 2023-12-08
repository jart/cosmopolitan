/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/yoink.h"

PYTHON_PROVIDE("_crypt");
PYTHON_PROVIDE("_crypt.crypt");

/* cryptmodule.c - by Steve Majewski
 */

/*[clinic input]
module crypt
[clinic start generated code]*/
/*[clinic end generated code: output=da39a3ee5e6b4b0d input=c6252cf4f2f2ae81]*/

#include "third_party/python/Modules/clinic/_cryptmodule.inc"

/*[clinic input]
crypt.crypt

    word: str
    salt: str
    /

Hash a *word* with the given *salt* and return the hashed password.

*word* will usually be a user's password.  *salt* (either a random 2 or 16
character string, possibly prefixed with $digit$ to indicate the method)
will be used to perturb the encryption algorithm and produce distinct
results for a given *word*.

[clinic start generated code]*/

static PyObject *
crypt_crypt_impl(PyObject *module, const char *word, const char *salt)
/*[clinic end generated code: output=0512284a03d2803c input=0e8edec9c364352b]*/
{
    /* On some platforms (AtheOS) crypt returns NULL for an invalid
       salt. Return None in that case. XXX Maybe raise an exception?  */
    return Py_BuildValue("s", crypt(word, salt));
}


static PyMethodDef crypt_methods[] = {
    CRYPT_CRYPT_METHODDEF
    {NULL,              NULL}           /* sentinel */
};


static struct PyModuleDef cryptmodule = {
    PyModuleDef_HEAD_INIT,
    "_crypt",
    NULL,
    -1,
    crypt_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit__crypt(void)
{
    return PyModule_Create(&cryptmodule);
}
