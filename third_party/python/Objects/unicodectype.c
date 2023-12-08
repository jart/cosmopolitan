/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Modules/unicodedata.h"
#include "third_party/python/Modules/unicodedata_unidata.h"

/*
 * Unicode character type helpers.
 *
 * Written by Marc-Andre Lemburg (mal@lemburg.com).
 * Modified for Python 2.0 by Fredrik Lundh (fredrik@pythonware.com)
 *
 * Copyright (c) Corporation for National Research Initiatives.
 */

#define ALPHA_MASK          0x01
#define DECIMAL_MASK        0x02
#define DIGIT_MASK          0x04
#define LOWER_MASK          0x08
#define LINEBREAK_MASK      0x10
#define SPACE_MASK          0x20
#define TITLE_MASK          0x40
#define UPPER_MASK          0x80
#define XID_START_MASK      0x100
#define XID_CONTINUE_MASK   0x200
#define PRINTABLE_MASK      0x400
#define NUMERIC_MASK        0x800
#define CASE_IGNORABLE_MASK 0x1000
#define CASED_MASK          0x2000
#define EXTENDED_CASE_MASK  0x4000

static const _PyUnicode_TypeRecord *
_PyUnicode_GetTypeRecord(Py_UCS4 c)
{
    int i, k;
    if (c >= 0x110000) {
        i = 0;
    } else {
        k = _PyUnicode_TypeRecordsShift;
        i = _PyUnicode_TypeRecordsIndex1[(c >> k)];
        i = _PyUnicode_TypeRecordsIndex2[(i << k) + (c & ((1 << k) - 1))];
    }
    return &_PyUnicode_TypeRecords[i];
}

/* Returns the titlecase Unicode characters corresponding to ch or just
   ch if no titlecase mapping is known. */
Py_UCS4 _PyUnicode_ToTitlecase(Py_UCS4 ch)
{
    const _PyUnicode_TypeRecord *ctype = _PyUnicode_GetTypeRecord(ch);
    if (ctype->flags & EXTENDED_CASE_MASK)
        return _PyUnicode_ExtendedCase[ctype->title & 0xFFFF];
    return ch + ctype->title;
}

/* Returns 1 for Unicode characters having the category 'Lt', 0
   otherwise. */
int _PyUnicode_IsTitlecase(Py_UCS4 ch)
{
    return !!(_PyUnicode_GetTypeRecord(ch)->flags & TITLE_MASK);
}

/* Returns 1 for Unicode characters having the XID_Start property, 0
   otherwise. */
int _PyUnicode_IsXidStart(Py_UCS4 ch)
{
    return !!(_PyUnicode_GetTypeRecord(ch)->flags & XID_START_MASK);
}

/* Returns 1 for Unicode characters having the XID_Continue property,
   0 otherwise. */
int _PyUnicode_IsXidContinue(Py_UCS4 ch)
{
    return !!(_PyUnicode_GetTypeRecord(ch)->flags & XID_CONTINUE_MASK);
}

/* Returns the integer decimal (0-9) for Unicode characters having
   this property, -1 otherwise. */
int _PyUnicode_ToDecimalDigit(Py_UCS4 ch)
{
    const _PyUnicode_TypeRecord *ctype = _PyUnicode_GetTypeRecord(ch);
    return (ctype->flags & DECIMAL_MASK) ? ctype->decimal : -1;
}

int _PyUnicode_IsDecimalDigit(Py_UCS4 ch)
{
    if (_PyUnicode_ToDecimalDigit(ch) < 0)
        return 0;
    return 1;
}

/* Returns the integer digit (0-9) for Unicode characters having
   this property, -1 otherwise. */
int _PyUnicode_ToDigit(Py_UCS4 ch)
{
    const _PyUnicode_TypeRecord *ctype = _PyUnicode_GetTypeRecord(ch);
    return (ctype->flags & DIGIT_MASK) ? ctype->digit : -1;
}

int _PyUnicode_IsDigit(Py_UCS4 ch)
{
    if (_PyUnicode_ToDigit(ch) < 0)
        return 0;
    return 1;
}

/* Returns the numeric value as double for Unicode characters having
   this property, -1.0 otherwise. */
int _PyUnicode_IsNumeric(Py_UCS4 ch)
{
    return !!(_PyUnicode_GetTypeRecord(ch)->flags & NUMERIC_MASK);
}

/* Returns 1 for Unicode characters to be hex-escaped when repr()ed,
   0 otherwise.
   All characters except those characters defined in the Unicode character
   database as following categories are considered printable.
      * Cc (Other, Control)
      * Cf (Other, Format)
      * Cs (Other, Surrogate)
      * Co (Other, Private Use)
      * Cn (Other, Not Assigned)
      * Zl Separator, Line ('\u2028', LINE SEPARATOR)
      * Zp Separator, Paragraph ('\u2029', PARAGRAPH SEPARATOR)
      * Zs (Separator, Space) other than ASCII space('\x20').
*/
int _PyUnicode_IsPrintable(Py_UCS4 ch)
{
    return !!(_PyUnicode_GetTypeRecord(ch)->flags & PRINTABLE_MASK);
}

/* Returns 1 for Unicode characters having the category 'Ll', 0
   otherwise. */
int _PyUnicode_IsLowercase(Py_UCS4 ch)
{
    return !!(_PyUnicode_GetTypeRecord(ch)->flags & LOWER_MASK);
}

/* Returns 1 for Unicode characters having the category 'Lu', 0
   otherwise. */
int _PyUnicode_IsUppercase(Py_UCS4 ch)
{
    return !!(_PyUnicode_GetTypeRecord(ch)->flags & UPPER_MASK);
}

/* Returns the uppercase Unicode characters corresponding to ch or just
   ch if no uppercase mapping is known. */
Py_UCS4 _PyUnicode_ToUppercase(Py_UCS4 ch)
{
    const _PyUnicode_TypeRecord *ctype = _PyUnicode_GetTypeRecord(ch);
    if (ctype->flags & EXTENDED_CASE_MASK)
        return _PyUnicode_ExtendedCase[ctype->upper & 0xFFFF];
    return ch + ctype->upper;
}

/* Returns the lowercase Unicode characters corresponding to ch or just
   ch if no lowercase mapping is known. */
Py_UCS4 _PyUnicode_ToLowercase(Py_UCS4 ch)
{
    const _PyUnicode_TypeRecord *ctype = _PyUnicode_GetTypeRecord(ch);
    if (ctype->flags & EXTENDED_CASE_MASK)
        return _PyUnicode_ExtendedCase[ctype->lower & 0xFFFF];
    return ch + ctype->lower;
}

int _PyUnicode_ToLowerFull(Py_UCS4 ch, Py_UCS4 *res)
{
    const _PyUnicode_TypeRecord *ctype = _PyUnicode_GetTypeRecord(ch);
    if (ctype->flags & EXTENDED_CASE_MASK) {
        int index = ctype->lower & 0xFFFF;
        int n = ctype->lower >> 24;
        int i;
        for (i = 0; i < n; i++)
            res[i] = _PyUnicode_ExtendedCase[index + i];
        return n;
    }
    res[0] = ch + ctype->lower;
    return 1;
}

int _PyUnicode_ToTitleFull(Py_UCS4 ch, Py_UCS4 *res)
{
    const _PyUnicode_TypeRecord *ctype = _PyUnicode_GetTypeRecord(ch);
    if (ctype->flags & EXTENDED_CASE_MASK) {
        int index = ctype->title & 0xFFFF;
        int n = ctype->title >> 24;
        int i;
        for (i = 0; i < n; i++)
            res[i] = _PyUnicode_ExtendedCase[index + i];
        return n;
    }
    res[0] = ch + ctype->title;
    return 1;
}

int _PyUnicode_ToUpperFull(Py_UCS4 ch, Py_UCS4 *res)
{
    const _PyUnicode_TypeRecord *ctype = _PyUnicode_GetTypeRecord(ch);
    if (ctype->flags & EXTENDED_CASE_MASK) {
        int index = ctype->upper & 0xFFFF;
        int n = ctype->upper >> 24;
        int i;
        for (i = 0; i < n; i++)
            res[i] = _PyUnicode_ExtendedCase[index + i];
        return n;
    }
    res[0] = ch + ctype->upper;
    return 1;
}

int _PyUnicode_ToFoldedFull(Py_UCS4 ch, Py_UCS4 *res)
{
    const _PyUnicode_TypeRecord *ctype = _PyUnicode_GetTypeRecord(ch);
    if (ctype->flags & EXTENDED_CASE_MASK && (ctype->lower >> 20) & 7) {
        int index = (ctype->lower & 0xFFFF) + (ctype->lower >> 24);
        int n = (ctype->lower >> 20) & 7;
        int i;
        for (i = 0; i < n; i++)
            res[i] = _PyUnicode_ExtendedCase[index + i];
        return n;
    }
    return _PyUnicode_ToLowerFull(ch, res);
}

int _PyUnicode_IsCased(Py_UCS4 ch)
{
    return !!(_PyUnicode_GetTypeRecord(ch)->flags & CASED_MASK);
}

int _PyUnicode_IsCaseIgnorable(Py_UCS4 ch)
{
    return !!(_PyUnicode_GetTypeRecord(ch)->flags & CASE_IGNORABLE_MASK);
}

/* Returns 1 for Unicode characters having the category 'Ll', 'Lu', 'Lt',
   'Lo' or 'Lm',  0 otherwise. */
int _PyUnicode_IsAlpha(Py_UCS4 ch)
{
    return !!(_PyUnicode_GetTypeRecord(ch)->flags & ALPHA_MASK);
}
