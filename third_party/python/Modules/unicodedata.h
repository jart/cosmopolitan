#ifndef COSMOPOLITAN_THIRD_PARTY_PYTHON_MODULES_UNICODEDATA_H_
#define COSMOPOLITAN_THIRD_PARTY_PYTHON_MODULES_UNICODEDATA_H_
#include "libc/assert.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/unicodeobject.h"

#define _Hanghoul_SBase  0xAC00
#define _Hanghoul_LBase  0x1100
#define _Hanghoul_VBase  0x1161
#define _Hanghoul_TBase  0x11A7
#define _Hanghoul_LCount 19
#define _Hanghoul_VCount 21
#define _Hanghoul_TCount 28
#define _Hanghoul_NCount (_Hanghoul_VCount * _Hanghoul_TCount)
#define _Hanghoul_SCount (_Hanghoul_LCount * _Hanghoul_NCount)

COSMOPOLITAN_C_START_

/*
 * [jart] if it adds 1.2megs to each binary then it should have an api
 *        breaking this up into separate files allows ld to do its job
 */

#define UCD_Check(o)            (Py_TYPE(o)==&UCD_Type)
#define get_old_record(self, v) ((((PreviousDBVersion*)self)->getrecord)(v))

typedef struct {
    const unsigned char category;         /* index into _PyUnicode_CategoryNames */
    const unsigned char combining;        /* combining class value 0 - 255 */
    const unsigned char bidirectional;    /* index into _PyUnicode_BidirectionalNames */
    const unsigned char mirrored;         /* true if mirrored in bidir mode */
    const unsigned char east_asian_width; /* index into _PyUnicode_EastAsianWidth */
    const unsigned char normalization_quick_check; /* see is_normalized() */
} _PyUnicode_Record;

typedef struct {
    /* sequence of fields should be the same as in merge_old_version */
    const unsigned char bidir_changed;
    const unsigned char category_changed;
    const unsigned char decimal_changed;
    const unsigned char mirrored_changed;
    const unsigned char east_asian_width_changed;
    const double numeric_changed;
} _PyUnicode_ChangeRecord;

typedef struct {
    PyObject_HEAD
    const char *name;
    const _PyUnicode_ChangeRecord *(*getrecord)(Py_UCS4);
    Py_UCS4 (*normalization)(Py_UCS4);
} PreviousDBVersion;

typedef struct {
    int start;
    short count;
    short index;
} _PyUnicode_Reindex;

typedef struct {
    /*
       These are either deltas to the character or offsets in
       _PyUnicode_ExtendedCase.
    */
    int upper;
    int lower;
    int title;
    /* Note if more flag space is needed, decimal and digit could be unified. */
    unsigned char decimal;
    unsigned char digit;
    unsigned short flags;
} _PyUnicode_TypeRecord;

/*
 * In Unicode 6.0.0, the sequences contain at most 4 BMP chars,
 * so we are using Py_UCS2 seq[4].  This needs to be updated if longer
 * sequences or sequences with non-BMP chars are added.
 * unicodedata_lookup should be adapted too.
 */
typedef struct{
  int seqlen;
  Py_UCS2 seq[4];
} _PyUnicode_NamedSequence;

extern PyTypeObject UCD_Type;

int _PyUnicode_IsUnifiedIdeograph(Py_UCS4);
const _PyUnicode_Record *_PyUnicode_GetRecord(Py_UCS4);
PyObject *_PyUnicode_NfcNfkc(PyObject *, PyObject *, int);
PyObject *_PyUnicode_NfdNfkd(PyObject *, PyObject *, int);
int _PyUnicode_IsNormalized(PyObject *, PyObject *, int, int);
int _PyUnicode_GetUcName(PyObject *, Py_UCS4, char *, int, int);
int _PyUnicode_FindNfcIndex(const _PyUnicode_Reindex *, Py_UCS4);
void _PyUnicode_FindSyllable(const char *, int *, int *, int, int);
int _PyUnicode_GetCode(PyObject *, const char *, int, Py_UCS4 *, int);
void _PyUnicode_GetDecompRecord(PyObject *, Py_UCS4, int *, int *, int *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_PYTHON_MODULES_UNICODEDATA_H_ */
