/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#define PY_SSIZE_T_CLEAN
#include "third_party/python/Modules/unicodedata.h"
#include "libc/nexgen32e/kompressor.h"
#include "third_party/python/Include/floatobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/objimpl.h"
#include "third_party/python/Include/pycapsule.h"
#include "third_party/python/Include/pyctype.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/structmember.h"
#include "third_party/python/Include/ucnhash.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/Modules/bextra.h"
#include "third_party/python/Modules/unicodedata_unidata.h"

PYTHON_PROVIDE("unicodedata");
PYTHON_PROVIDE("unicodedata.UCD");
PYTHON_PROVIDE("unicodedata.bidirectional");
PYTHON_PROVIDE("unicodedata.category");
PYTHON_PROVIDE("unicodedata.combining");
PYTHON_PROVIDE("unicodedata.decimal");
PYTHON_PROVIDE("unicodedata.decomposition");
PYTHON_PROVIDE("unicodedata.digit");
PYTHON_PROVIDE("unicodedata.east_asian_width");
PYTHON_PROVIDE("unicodedata.lookup");
PYTHON_PROVIDE("unicodedata.mirrored");
PYTHON_PROVIDE("unicodedata.name");
PYTHON_PROVIDE("unicodedata.normalize");
PYTHON_PROVIDE("unicodedata.numeric");
PYTHON_PROVIDE("unicodedata.ucd_3_2_0");
PYTHON_PROVIDE("unicodedata.ucnhash_CAPI");
PYTHON_PROVIDE("unicodedata.unidata_version");

/* ------------------------------------------------------------------------

   unicodedata -- Provides access to the Unicode database.

   Data was extracted from the UnicodeData.txt file.
   The current version number is reported in the unidata_version constant.

   Written by Marc-Andre Lemburg (mal@lemburg.com).
   Modified for Python 2.0 by Fredrik Lundh (fredrik@pythonware.com)
   Modified by Martin v. Löwis (martin@v.loewis.de)

   Copyright (c) Corporation for National Research Initiatives.

   ------------------------------------------------------------------------ */

/*[clinic input]
module unicodedata
class unicodedata.UCD 'PreviousDBVersion *' '&UCD_Type'
[clinic start generated code]*/
/*[clinic end generated code: output=da39a3ee5e6b4b0d input=6dac153082d150bc]*/

#include "third_party/python/Modules/clinic/unicodedata.inc"

/* ------------- Previous-version API ------------------------------------- */

static PyMemberDef DB_members[] = {
    {"unidata_version", T_STRING, offsetof(PreviousDBVersion, name), READONLY},
    {0}
};

static PyObject *
new_previous_version(const char *name, 
                     const _PyUnicode_ChangeRecord* (*getrecord)(Py_UCS4),
                     Py_UCS4 (*normalization)(Py_UCS4))
{
    PreviousDBVersion *self;
    self = PyObject_New(PreviousDBVersion, &UCD_Type);
    if (self == NULL)
        return NULL;
    self->name = name;
    self->getrecord = getrecord;
    self->normalization = normalization;
    return (PyObject*)self;
}

/* --- Module API --------------------------------------------------------- */

/*[clinic input]
unicodedata.UCD.decimal

    self: self
    chr: int(accept={str})
    default: object=NULL
    /

Converts a Unicode character into its equivalent decimal value.

Returns the decimal value assigned to the character chr as integer.
If no such value is defined, default is returned, or, if not given,
ValueError is raised.
[clinic start generated code]*/

static PyObject *
unicodedata_UCD_decimal_impl(PyObject *self, int chr,
                             PyObject *default_value)
/*[clinic end generated code: output=be23376e1a185231 input=933f8107993f23d0]*/
{
    int have_old = 0;
    long rc;
    Py_UCS4 c = (Py_UCS4)chr;

    if (self && UCD_Check(self)) {
        const _PyUnicode_ChangeRecord *old = get_old_record(self, c);
        if (old->category_changed == 0) {
            /* unassigned */
            have_old = 1;
            rc = -1;
        }
        else if (old->decimal_changed != 0xFF) {
            have_old = 1;
            rc = old->decimal_changed;
        }
    }

    if (!have_old)
        rc = Py_UNICODE_TODECIMAL(c);
    if (rc < 0) {
        if (default_value == NULL) {
            PyErr_SetString(PyExc_ValueError,
                            "not a decimal");
            return NULL;
        }
        else {
            Py_INCREF(default_value);
            return default_value;
        }
    }
    return PyLong_FromLong(rc);
}

/*[clinic input]
unicodedata.UCD.digit

    self: self
    chr: int(accept={str})
    default: object=NULL
    /

Converts a Unicode character into its equivalent digit value.

Returns the digit value assigned to the character chr as integer.
If no such value is defined, default is returned, or, if not given,
ValueError is raised.
[clinic start generated code]*/

static PyObject *
unicodedata_UCD_digit_impl(PyObject *self, int chr, PyObject *default_value)
/*[clinic end generated code: output=96e18c950171fd2f input=e27d6e4565cd29f2]*/
{
    long rc;
    Py_UCS4 c = (Py_UCS4)chr;
    rc = Py_UNICODE_TODIGIT(c);
    if (rc < 0) {
        if (default_value == NULL) {
            PyErr_SetString(PyExc_ValueError, "not a digit");
            return NULL;
        }
        else {
            Py_INCREF(default_value);
            return default_value;
        }
    }
    return PyLong_FromLong(rc);
}

/*[clinic input]
unicodedata.UCD.numeric

    self: self
    chr: int(accept={str})
    default: object=NULL
    /

Converts a Unicode character into its equivalent numeric value.

Returns the numeric value assigned to the character chr as float.
If no such value is defined, default is returned, or, if not given,
ValueError is raised.
[clinic start generated code]*/

static PyObject *
unicodedata_UCD_numeric_impl(PyObject *self, int chr,
                             PyObject *default_value)
/*[clinic end generated code: output=53ce281fe85b10c4 input=fdf5871a5542893c]*/
{
    int have_old = 0;
    double rc;
    Py_UCS4 c = (Py_UCS4)chr;
    if (self && UCD_Check(self)) {
        const _PyUnicode_ChangeRecord *old = get_old_record(self, c);
        if (old->category_changed == 0) {
            /* unassigned */
            have_old = 1;
            rc = -1.0;
        }
        else if (old->decimal_changed != 0xFF) {
            have_old = 1;
            rc = old->decimal_changed;
        }
    }
    if (!have_old)
        rc = Py_UNICODE_TONUMERIC(c);
    if (rc == -1.0) {
        if (default_value == NULL) {
            PyErr_SetString(PyExc_ValueError, "not a numeric character");
            return NULL;
        }
        else {
            Py_INCREF(default_value);
            return default_value;
        }
    }
    return PyFloat_FromDouble(rc);
}

/*[clinic input]
unicodedata.UCD.category

    self: self
    chr: int(accept={str})
    /

Returns the general category assigned to the character chr as string.
[clinic start generated code]*/

static PyObject *
unicodedata_UCD_category_impl(PyObject *self, int chr)
/*[clinic end generated code: output=8571539ee2e6783a input=27d6f3d85050bc06]*/
{
    int index;
    Py_UCS4 c = (Py_UCS4)chr;
    index = (int) _PyUnicode_GetRecord(c)->category;
    if (self && UCD_Check(self)) {
        const _PyUnicode_ChangeRecord *old = get_old_record(self, c);
        if (old->category_changed != 0xFF)
            index = old->category_changed;
    }
    return PyUnicode_FromString(_PyUnicode_CategoryNames[index]);
}

/*[clinic input]
unicodedata.UCD.bidirectional

    self: self
    chr: int(accept={str})
    /

Returns the bidirectional class assigned to the character chr as string.

If no such value is defined, an empty string is returned.
[clinic start generated code]*/

static PyObject *
unicodedata_UCD_bidirectional_impl(PyObject *self, int chr)
/*[clinic end generated code: output=d36310ce2039bb92 input=b3d8f42cebfcf475]*/
{
    int index;
    Py_UCS4 c = (Py_UCS4)chr;
    index = (int) _PyUnicode_GetRecord(c)->bidirectional;
    if (self && UCD_Check(self)) {
        const _PyUnicode_ChangeRecord *old = get_old_record(self, c);
        if (old->category_changed == 0)
            index = 0; /* unassigned */
        else if (old->bidir_changed != 0xFF)
            index = old->bidir_changed;
    }
    return PyUnicode_FromString(_PyUnicode_BidirectionalNames[index]);
}

/*[clinic input]
unicodedata.UCD.combining -> int

    self: self
    chr: int(accept={str})
    /

Returns the canonical combining class assigned to the character chr as integer.

Returns 0 if no combining class is defined.
[clinic start generated code]*/

static int
unicodedata_UCD_combining_impl(PyObject *self, int chr)
/*[clinic end generated code: output=cad056d0cb6a5920 input=9f2d6b2a95d0a22a]*/
{
    int index;
    Py_UCS4 c = (Py_UCS4)chr;
    index = (int) _PyUnicode_GetRecord(c)->combining;
    if (self && UCD_Check(self)) {
        const _PyUnicode_ChangeRecord *old = get_old_record(self, c);
        if (old->category_changed == 0)
            index = 0; /* unassigned */
    }
    return index;
}

/*[clinic input]
unicodedata.UCD.mirrored -> int

    self: self
    chr: int(accept={str})
    /

Returns the mirrored property assigned to the character chr as integer.

Returns 1 if the character has been identified as a "mirrored"
character in bidirectional text, 0 otherwise.
[clinic start generated code]*/

static int
unicodedata_UCD_mirrored_impl(PyObject *self, int chr)
/*[clinic end generated code: output=2532dbf8121b50e6 input=5dd400d351ae6f3b]*/
{
    int index;
    Py_UCS4 c = (Py_UCS4)chr;
    index = (int) _PyUnicode_GetRecord(c)->mirrored;
    if (self && UCD_Check(self)) {
        const _PyUnicode_ChangeRecord *old = get_old_record(self, c);
        if (old->category_changed == 0)
            index = 0; /* unassigned */
        else if (old->mirrored_changed != 0xFF)
            index = old->mirrored_changed;
    }
    return index;
}

/*[clinic input]
unicodedata.UCD.east_asian_width

    self: self
    chr: int(accept={str})
    /

Returns the east asian width assigned to the character chr as string.
[clinic start generated code]*/

static PyObject *
unicodedata_UCD_east_asian_width_impl(PyObject *self, int chr)
/*[clinic end generated code: output=484e8537d9ee8197 input=c4854798aab026e0]*/
{
    int index;
    Py_UCS4 c = (Py_UCS4)chr;
    index = (int) _PyUnicode_GetRecord(c)->east_asian_width;
    if (self && UCD_Check(self)) {
        const _PyUnicode_ChangeRecord *old = get_old_record(self, c);
        if (old->category_changed == 0)
            index = 0; /* unassigned */
        else if (old->east_asian_width_changed != 0xFF)
            index = old->east_asian_width_changed;
    }
    return PyUnicode_FromString(_PyUnicode_EastAsianWidthNames[index]);
}

/*[clinic input]
unicodedata.UCD.decomposition

    self: self
    chr: int(accept={str})
    /

Returns the character decomposition mapping assigned to the character chr as string.

An empty string is returned in case no such mapping is defined.
[clinic start generated code]*/

static PyObject *
unicodedata_UCD_decomposition_impl(PyObject *self, int chr)
/*[clinic end generated code: output=7d699f3ec7565d27 input=e4c12459ad68507b]*/
{
    char decomp[256];
    int code, index, count;
    size_t i;
    unsigned int prefix_index;
    Py_UCS4 c = (Py_UCS4)chr;

    code = (int)c;

    if (self && UCD_Check(self)) {
        const _PyUnicode_ChangeRecord *old = get_old_record(self, c);
        if (old->category_changed == 0)
            return PyUnicode_FromString(""); /* unassigned */
    }

    if (code < 0 || code >= 0x110000) {
        index = 0;
    } else {
        index = _PyUnicode_DecompIndex1[(code>>_PyUnicode_DecompShift)];
        index = _PyUnicode_DecompIndex2[(index<<_PyUnicode_DecompShift)+
                                        (code&((1<<_PyUnicode_DecompShift)-1))];
    }

    /* high byte is number of hex bytes (usually one or two), low byte
       is prefix code (from*/
    count = BitFieldExtract(_PyUnicode_Decomp, index, _PyUnicode_DecompBits) >> 8;

    /* XXX: could allocate the PyString up front instead
       (strlen(prefix) + 5 * count + 1 bytes) */

    /* Based on how index is calculated above and _PyUnicode_Decomp is
       generated from Tools/unicode/makeunicodedata.py, it should not be
       possible to overflow _PyUnicode_DecompPrefix. */
    prefix_index = BitFieldExtract(_PyUnicode_Decomp, index, _PyUnicode_DecompBits) & 255;
    assert(prefix_index < Py_ARRAY_LENGTH(_PyUnicode_DecompPrefix));

    /* copy prefix */
    i = strlen(_PyUnicode_DecompPrefix[prefix_index]);
    memcpy(decomp, _PyUnicode_DecompPrefix[prefix_index], i);

    while (count-- > 0) {
        if (i)
            decomp[i++] = ' ';
        assert(i < sizeof(decomp));
        PyOS_snprintf(decomp + i, sizeof(decomp) - i, "%04X",
                      BitFieldExtract(_PyUnicode_Decomp, ++index,
                                      _PyUnicode_DecompBits));
        i += strlen(decomp + i);
    }
    return PyUnicode_FromStringAndSize(decomp, i);
}

/*[clinic input]
unicodedata.UCD.normalize

    self: self
    form: str
    unistr as input: object(subclass_of='&PyUnicode_Type')
    /

Return the normal form 'form' for the Unicode string unistr.

Valid values for form are 'NFC', 'NFKC', 'NFD', and 'NFKD'.
[clinic start generated code]*/

static PyObject *
unicodedata_UCD_normalize_impl(PyObject *self, const char *form,
                               PyObject *input)
/*[clinic end generated code: output=62d1f8870027efdc input=cd092e631cf11883]*/
{
    if (PyUnicode_READY(input) == -1)
        return NULL;
    if (PyUnicode_GET_LENGTH(input) == 0) {
        /* Special case empty input strings, since resizing
           them  later would cause internal errors. */
        Py_INCREF(input);
        return input;
    }
    if (strcmp(form, "NFC") == 0) {
        if (_PyUnicode_IsNormalized(self, input, 1, 0)) {
            Py_INCREF(input);
            return input;
        }
        return _PyUnicode_NfcNfkc(self, input, 0);
    }
    if (strcmp(form, "NFKC") == 0) {
        if (_PyUnicode_IsNormalized(self, input, 1, 1)) {
            Py_INCREF(input);
            return input;
        }
        return _PyUnicode_NfcNfkc(self, input, 1);
    }
    if (strcmp(form, "NFD") == 0) {
        if (_PyUnicode_IsNormalized(self, input, 0, 0)) {
            Py_INCREF(input);
            return input;
        }
        return _PyUnicode_NfdNfkd(self, input, 0);
    }
    if (strcmp(form, "NFKD") == 0) {
        if (_PyUnicode_IsNormalized(self, input, 0, 1)) {
            Py_INCREF(input);
            return input;
        }
        return _PyUnicode_NfdNfkd(self, input, 1);
    }
    PyErr_SetString(PyExc_ValueError, "invalid normalization form");
    return NULL;
}

/* -------------------------------------------------------------------- */
/* database code (cut and pasted from the unidb package) */

/* macros used to determine if the given code point is in the PUA range that
 * we are using to store aliases and named sequences */
#define IS_ALIAS(cp) ((cp >= _PyUnicode_AliasesStart) && \
                      (cp <  _PyUnicode_AliasesEnd))
#define IS_NAMED_SEQ(cp) ((cp >= _PyUnicode_NamedSequencesStart) && \
                          (cp <  _PyUnicode_NamedSequencesEnd))

static const _PyUnicode_Name_CAPI hashAPI =
{
    sizeof(_PyUnicode_Name_CAPI),
    _PyUnicode_GetUcName,
    _PyUnicode_GetCode,
};

/* -------------------------------------------------------------------- */
/* Python bindings */

/*[clinic input]
unicodedata.UCD.name

    self: self
    chr: int(accept={str})
    default: object=NULL
    /

Returns the name assigned to the character chr as a string.

If no name is defined, default is returned, or, if not given,
ValueError is raised.
[clinic start generated code]*/

static PyObject *
unicodedata_UCD_name_impl(PyObject *self, int chr, PyObject *default_value)
/*[clinic end generated code: output=6bbb37a326407707 input=3e0367f534de56d9]*/
{
    char name[UNIDATA_NAME_MAXLEN+1];
    Py_UCS4 c = (Py_UCS4)chr;
    if (!_PyUnicode_GetUcName(self, c, name, UNIDATA_NAME_MAXLEN, 0)) {
        if (default_value == NULL) {
            PyErr_SetString(PyExc_ValueError, "no such name");
            return NULL;
        }
        else {
            Py_INCREF(default_value);
            return default_value;
        }
    }
    return PyUnicode_FromString(name);
}

/*[clinic input]
unicodedata.UCD.lookup

    self: self
    name: str(accept={str, robuffer}, zeroes=True)
    /

Look up character by name.

If a character with the given name is found, return the
corresponding character.  If not found, KeyError is raised.
[clinic start generated code]*/

static PyObject *
unicodedata_UCD_lookup_impl(PyObject *self, const char *name,
                            Py_ssize_clean_t name_length)
/*[clinic end generated code: output=765cb8186788e6be input=a557be0f8607a0d6]*/
{
    Py_UCS4 code;
    unsigned int index;
    if (name_length > UNIDATA_NAME_MAXLEN) {
        PyErr_SetString(PyExc_KeyError, "name too long");
        return NULL;
    }
    if (!_PyUnicode_GetCode(self, name, (int)name_length, &code, 1)) {
        PyErr_Format(PyExc_KeyError, "undefined character name '%s'", name);
        return NULL;
    }
    /* check if code is in the PUA range that we use for named sequences
       and convert it */
    if (IS_NAMED_SEQ(code)) {
        index = code - _PyUnicode_NamedSequencesStart;
        return PyUnicode_FromKindAndData(PyUnicode_2BYTE_KIND,
                                         _PyUnicode_NamedSequences[index].seq,
                                         _PyUnicode_NamedSequences[index].seqlen);
    }
    return PyUnicode_FromOrdinal(code);
}

/* XXX Add doc strings. */

static PyMethodDef unicodedata_functions[] = {
    UNICODEDATA_UCD_DECIMAL_METHODDEF
    UNICODEDATA_UCD_DIGIT_METHODDEF
    UNICODEDATA_UCD_NUMERIC_METHODDEF
    UNICODEDATA_UCD_CATEGORY_METHODDEF
    UNICODEDATA_UCD_BIDIRECTIONAL_METHODDEF
    UNICODEDATA_UCD_COMBINING_METHODDEF
    UNICODEDATA_UCD_MIRRORED_METHODDEF
    UNICODEDATA_UCD_EAST_ASIAN_WIDTH_METHODDEF
    UNICODEDATA_UCD_DECOMPOSITION_METHODDEF
    UNICODEDATA_UCD_NAME_METHODDEF
    UNICODEDATA_UCD_LOOKUP_METHODDEF
    UNICODEDATA_UCD_NORMALIZE_METHODDEF
    {NULL, NULL}                /* sentinel */
};

PyDoc_STRVAR(unicodedata_docstring,
"This module provides access to the Unicode Character Database which\n\
defines character properties for all Unicode characters. The data in\n\
this database is based on the UnicodeData.txt file version\n\
" UNIDATA_VERSION " which is publically available from ftp://ftp.unicode.org/.\n\
\n\
The module uses the same names and symbols as defined by the\n\
UnicodeData File Format " UNIDATA_VERSION ".");

static struct PyModuleDef unicodedatamodule = {
    PyModuleDef_HEAD_INIT,
    "unicodedata",
    unicodedata_docstring,
    -1,
    unicodedata_functions
};

PyMODINIT_FUNC
PyInit_unicodedata(void)
{
    PyObject *m, *v;
    UCD_Type.tp_dealloc = (destructor)PyObject_Del;
    UCD_Type.tp_getattro = PyObject_GenericGetAttr;
    UCD_Type.tp_flags = Py_TPFLAGS_DEFAULT;
    UCD_Type.tp_methods = unicodedata_functions;
    UCD_Type.tp_members = DB_members;
    Py_TYPE(&UCD_Type) = &PyType_Type;
    m = PyModule_Create(&unicodedatamodule);
    if (!m)
        return NULL;
    PyModule_AddStringConstant(m, "unidata_version", UNIDATA_VERSION);
    Py_INCREF(&UCD_Type);
    PyModule_AddObject(m, "UCD", (PyObject*)&UCD_Type);
    /* Previous versions */
    v = new_previous_version("3.2.0",
                             _PyUnicode_GetChange_3_2_0,
                             _PyUnicode_Normalization_3_2_0);
    if (v != NULL)
        PyModule_AddObject(m, "ucd_3_2_0", v);
    /* Export C API */
    v = PyCapsule_New((void *)&hashAPI, PyUnicodeData_CAPSULE_NAME, NULL);
    if (v != NULL)
        PyModule_AddObject(m, "ucnhash_CAPI", v);
    return m;
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab_unicodedata = {
    "unicodedata",
    PyInit_unicodedata,
};
