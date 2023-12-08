/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#define PY_SSIZE_T_CLEAN
#include "libc/assert.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/pyctype.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/pystrhex.h"
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/zlib/zlib.h"

PYTHON_PROVIDE("binascii");
PYTHON_PROVIDE("binascii.Error");
PYTHON_PROVIDE("binascii.Incomplete");
PYTHON_PROVIDE("binascii.a2b_base64");
PYTHON_PROVIDE("binascii.a2b_hex");
PYTHON_PROVIDE("binascii.a2b_hqx");
PYTHON_PROVIDE("binascii.a2b_qp");
PYTHON_PROVIDE("binascii.a2b_uu");
PYTHON_PROVIDE("binascii.b2a_base64");
PYTHON_PROVIDE("binascii.b2a_hex");
PYTHON_PROVIDE("binascii.b2a_hqx");
PYTHON_PROVIDE("binascii.b2a_qp");
PYTHON_PROVIDE("binascii.b2a_uu");
PYTHON_PROVIDE("binascii.crc32");
PYTHON_PROVIDE("binascii.crc_hqx");
PYTHON_PROVIDE("binascii.hexlify");
PYTHON_PROVIDE("binascii.rlecode_hqx");
PYTHON_PROVIDE("binascii.rledecode_hqx");
PYTHON_PROVIDE("binascii.unhexlify");

/*
** Routines to represent binary data in ASCII and vice-versa
**
** This module currently supports the following encodings:
** uuencode:
**      each line encodes 45 bytes (except possibly the last)
**      First char encodes (binary) length, rest data
**      each char encodes 6 bits, as follows:
**      binary: 01234567 abcdefgh ijklmnop
**      ascii:  012345 67abcd efghij klmnop
**      ASCII encoding method is "excess-space": 000000 is encoded as ' ', etc.
**      short binary data is zero-extended (so the bits are always in the
**      right place), this does *not* reflect in the length.
** base64:
**      Line breaks are insignificant, but lines are at most 76 chars
**      each char encodes 6 bits, in similar order as uucode/hqx. Encoding
**      is done via a table.
**      Short binary data is filled (in ASCII) with '='.
** hqx:
**      File starts with introductory text, real data starts and ends
**      with colons.
**      Data consists of three similar parts: info, datafork, resourcefork.
**      Each part is protected (at the end) with a 16-bit crc
**      The binary data is run-length encoded, and then ascii-fied:
**      binary: 01234567 abcdefgh ijklmnop
**      ascii:  012345 67abcd efghij klmnop
**      ASCII encoding is table-driven, see the code.
**      Short binary data results in the runt ascii-byte being output with
**      the bits in the right place.
**
** While I was reading dozens of programs that encode or decode the formats
** here (documentation? hihi:-) I have formulated Jansen's Observation:
**
**      Programs that encode binary data in ASCII are written in
**      such a style that they are as unreadable as possible. Devices used
**      include unnecessary global variables, burying important tables
**      in unrelated sourcefiles, putting functions in include files,
**      using seemingly-descriptive variable names for different purposes,
**      calls to empty subroutines and a host of others.
**
** I have attempted to break with this tradition, but I guess that that
** does make the performance sub-optimal. Oh well, too bad...
**
** Jack Jansen, CWI, July 1995.
**
** Added support for quoted-printable encoding, based on rfc 1521 et al
** quoted-printable encoding specifies that non printable characters (anything
** below 32 and above 126) be encoded as =XX where XX is the hexadecimal value
** of the character.  It also specifies some other behavior to enable 8bit data
** in a mail message with little difficulty (maximum line sizes, protecting
** some cases of whitespace, etc).
**
** Brandon Long, September 2001.
*/

static PyObject *Error;
static PyObject *Incomplete;

/*
** hqx lookup table, ascii->binary.
*/

#define RUNCHAR 0x90

#define DONE 0x7F
#define SKIP 0x7E
#define FAIL 0x7D

static const unsigned char table_a2b_hqx[256] = {
/*       ^@    ^A    ^B    ^C    ^D    ^E    ^F    ^G   */
/* 0*/  FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
/*       \b    \t    \n    ^K    ^L    \r    ^N    ^O   */
/* 1*/  FAIL, FAIL, SKIP, FAIL, FAIL, SKIP, FAIL, FAIL,
/*       ^P    ^Q    ^R    ^S    ^T    ^U    ^V    ^W   */
/* 2*/  FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
/*       ^X    ^Y    ^Z    ^[    ^\    ^]    ^^    ^_   */
/* 3*/  FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
/*              !     "     #     $     %     &     '   */
/* 4*/  FAIL, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
/*        (     )     *     +     ,     -     .     /   */
/* 5*/  0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, FAIL, FAIL,
/*        0     1     2     3     4     5     6     7   */
/* 6*/  0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, FAIL,
/*        8     9     :     ;     <     =     >     ?   */
/* 7*/  0x14, 0x15, DONE, FAIL, FAIL, FAIL, FAIL, FAIL,
/*        @     A     B     C     D     E     F     G   */
/* 8*/  0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
/*        H     I     J     K     L     M     N     O   */
/* 9*/  0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, FAIL,
/*        P     Q     R     S     T     U     V     W   */
/*10*/  0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, FAIL,
/*        X     Y     Z     [     \     ]     ^     _   */
/*11*/  0x2C, 0x2D, 0x2E, 0x2F, FAIL, FAIL, FAIL, FAIL,
/*        `     a     b     c     d     e     f     g   */
/*12*/  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, FAIL,
/*        h     i     j     k     l     m     n     o   */
/*13*/  0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, FAIL, FAIL,
/*        p     q     r     s     t     u     v     w   */
/*14*/  0x3D, 0x3E, 0x3F, FAIL, FAIL, FAIL, FAIL, FAIL,
/*        x     y     z     {     |     }     ~    ^?   */
/*15*/  FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
/*16*/  FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
    FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL,
};

static const unsigned char table_b2a_hqx[] =
"!\"#$%&'()*+,-012345689@ABCDEFGHIJKLMNPQRSTUVXYZ[`abcdefhijklmpqr";

static const char table_a2b_base64[] = {
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,62, -1,-1,-1,63,
    52,53,54,55, 56,57,58,59, 60,61,-1,-1, -1, 0,-1,-1, /* Note PAD->0 */
    -1, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22, 23,24,25,-1, -1,-1,-1,-1,
    -1,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48, 49,50,51,-1, -1,-1,-1,-1
};

#define BASE64_PAD '='

/* Max binary chunk size; limited only by available memory */
#define BASE64_MAXBIN ((PY_SSIZE_T_MAX - 3) / 2)

static const unsigned char table_b2a_base64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned short crctab_hqx[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

/*[clinic input]
module binascii
[clinic start generated code]*/
/*[clinic end generated code: output=da39a3ee5e6b4b0d input=de89fb46bcaf3fec]*/

/*[python input]

class ascii_buffer_converter(CConverter):
    type = 'Py_buffer'
    converter = 'ascii_buffer_converter'
    impl_by_reference = True
    c_default = "{NULL, NULL}"

    def cleanup(self):
        name = self.name
        return "".join(["if (", name, ".obj)\n   PyBuffer_Release(&", name, ");\n"])

[python start generated code]*/
/*[python end generated code: output=da39a3ee5e6b4b0d input=3eb7b63610da92cd]*/

static int
ascii_buffer_converter(PyObject *arg, Py_buffer *buf)
{
    if (arg == NULL) {
        PyBuffer_Release(buf);
        return 1;
    }
    if (PyUnicode_Check(arg)) {
        if (PyUnicode_READY(arg) < 0)
            return 0;
        if (!PyUnicode_IS_ASCII(arg)) {
            PyErr_SetString(PyExc_ValueError,
                            "string argument should contain only ASCII characters");
            return 0;
        }
        assert(PyUnicode_KIND(arg) == PyUnicode_1BYTE_KIND);
        buf->buf = (void *) PyUnicode_1BYTE_DATA(arg);
        buf->len = PyUnicode_GET_LENGTH(arg);
        buf->obj = NULL;
        return 1;
    }
    if (PyObject_GetBuffer(arg, buf, PyBUF_SIMPLE) != 0) {
        PyErr_Format(PyExc_TypeError,
                     "argument should be bytes, buffer or ASCII string, "
                     "not '%.100s'", Py_TYPE(arg)->tp_name);
        return 0;
    }
    if (!PyBuffer_IsContiguous(buf, 'C')) {
        PyErr_Format(PyExc_TypeError,
                     "argument should be a contiguous buffer, "
                     "not '%.100s'", Py_TYPE(arg)->tp_name);
        PyBuffer_Release(buf);
        return 0;
    }
    return Py_CLEANUP_SUPPORTED;
}

#include "third_party/python/Modules/clinic/binascii.inc"

/*[clinic input]
binascii.a2b_uu

    data: ascii_buffer
    /

Decode a line of uuencoded data.
[clinic start generated code]*/

static PyObject *
binascii_a2b_uu_impl(PyObject *module, Py_buffer *data)
/*[clinic end generated code: output=e027f8e0b0598742 input=7cafeaf73df63d1c]*/
{
    const unsigned char *ascii_data;
    unsigned char *bin_data;
    int leftbits = 0;
    unsigned char this_ch;
    unsigned int leftchar = 0;
    PyObject *rv;
    Py_ssize_t ascii_len, bin_len;

    ascii_data = data->buf;
    ascii_len = data->len;

    assert(ascii_len >= 0);

    /* First byte: binary data length (in bytes) */
    bin_len = (*ascii_data++ - ' ') & 077;
    ascii_len--;

    /* Allocate the buffer */
    if ( (rv=PyBytes_FromStringAndSize(NULL, bin_len)) == NULL )
        return NULL;
    bin_data = (unsigned char *)PyBytes_AS_STRING(rv);

    for( ; bin_len > 0 ; ascii_len--, ascii_data++ ) {
        /* XXX is it really best to add NULs if there's no more data */
        this_ch = (ascii_len > 0) ? *ascii_data : 0;
        if ( this_ch == '\n' || this_ch == '\r' || ascii_len <= 0) {
            /*
            ** Whitespace. Assume some spaces got eaten at
            ** end-of-line. (We check this later)
            */
            this_ch = 0;
        } else {
            /* Check the character for legality
            ** The 64 in stead of the expected 63 is because
            ** there are a few uuencodes out there that use
            ** '`' as zero instead of space.
            */
            if ( this_ch < ' ' || this_ch > (' ' + 64)) {
                PyErr_SetString(Error, "Illegal char");
                Py_DECREF(rv);
                return NULL;
            }
            this_ch = (this_ch - ' ') & 077;
        }
        /*
        ** Shift it in on the low end, and see if there's
        ** a byte ready for output.
        */
        leftchar = (leftchar << 6) | (this_ch);
        leftbits += 6;
        if ( leftbits >= 8 ) {
            leftbits -= 8;
            *bin_data++ = (leftchar >> leftbits) & 0xff;
            leftchar &= ((1 << leftbits) - 1);
            bin_len--;
        }
    }
    /*
    ** Finally, check that if there's anything left on the line
    ** that it's whitespace only.
    */
    while( ascii_len-- > 0 ) {
        this_ch = *ascii_data++;
        /* Extra '`' may be written as padding in some cases */
        if ( this_ch != ' ' && this_ch != ' '+64 &&
             this_ch != '\n' && this_ch != '\r' ) {
            PyErr_SetString(Error, "Trailing garbage");
            Py_DECREF(rv);
            return NULL;
        }
    }
    return rv;
}

/*[clinic input]
binascii.b2a_uu

    data: Py_buffer
    /

Uuencode line of data.
[clinic start generated code]*/

static PyObject *
binascii_b2a_uu_impl(PyObject *module, Py_buffer *data)
/*[clinic end generated code: output=0070670e52e4aa6b input=00fdf458ce8b465b]*/
{
    unsigned char *ascii_data;
    const unsigned char *bin_data;
    int leftbits = 0;
    unsigned char this_ch;
    unsigned int leftchar = 0;
    Py_ssize_t bin_len, out_len;
    _PyBytesWriter writer;

    _PyBytesWriter_Init(&writer);
    bin_data = data->buf;
    bin_len = data->len;
    if ( bin_len > 45 ) {
        /* The 45 is a limit that appears in all uuencode's */
        PyErr_SetString(Error, "At most 45 bytes at once");
        return NULL;
    }

    /* We're lazy and allocate to much (fixed up later) */
    out_len = 2 + (bin_len + 2) / 3 * 4;
    ascii_data = _PyBytesWriter_Alloc(&writer, out_len);
    if (ascii_data == NULL)
        return NULL;

    /* Store the length */
    *ascii_data++ = ' ' + (bin_len & 077);

    for( ; bin_len > 0 || leftbits != 0 ; bin_len--, bin_data++ ) {
        /* Shift the data (or padding) into our buffer */
        if ( bin_len > 0 )              /* Data */
            leftchar = (leftchar << 8) | *bin_data;
        else                            /* Padding */
            leftchar <<= 8;
        leftbits += 8;

        /* See if there are 6-bit groups ready */
        while ( leftbits >= 6 ) {
            this_ch = (leftchar >> (leftbits-6)) & 0x3f;
            leftbits -= 6;
            *ascii_data++ = this_ch + ' ';
        }
    }
    *ascii_data++ = '\n';       /* Append a courtesy newline */

    return _PyBytesWriter_Finish(&writer, ascii_data);
}


static int
binascii_find_valid(const unsigned char *s, Py_ssize_t slen, int num)
{
    /* Finds & returns the (num+1)th
    ** valid character for base64, or -1 if none.
    */

    int ret = -1;
    unsigned char c, b64val;

    while ((slen > 0) && (ret == -1)) {
        c = *s;
        b64val = table_a2b_base64[c & 0x7f];
        if ( ((c <= 0x7f) && (b64val != (unsigned char)-1)) ) {
            if (num == 0)
                ret = *s;
            num--;
        }

        s++;
        slen--;
    }
    return ret;
}

/*[clinic input]
binascii.a2b_base64

    data: ascii_buffer
    /

Decode a line of base64 data.
[clinic start generated code]*/

static PyObject *
binascii_a2b_base64_impl(PyObject *module, Py_buffer *data)
/*[clinic end generated code: output=0628223f19fd3f9b input=5872acf6e1cac243]*/
{
    const unsigned char *ascii_data;
    unsigned char *bin_data;
    int leftbits = 0;
    unsigned char this_ch;
    unsigned int leftchar = 0;
    Py_ssize_t ascii_len, bin_len;
    int quad_pos = 0;
    _PyBytesWriter writer;

    ascii_data = data->buf;
    ascii_len = data->len;

    assert(ascii_len >= 0);

    if (ascii_len > PY_SSIZE_T_MAX - 3)
        return PyErr_NoMemory();

    bin_len = ((ascii_len+3)/4)*3; /* Upper bound, corrected later */

    _PyBytesWriter_Init(&writer);

    /* Allocate the buffer */
    bin_data = _PyBytesWriter_Alloc(&writer, bin_len);
    if (bin_data == NULL)
        return NULL;

    for( ; ascii_len > 0; ascii_len--, ascii_data++) {
        this_ch = *ascii_data;

        if (this_ch > 0x7f ||
            this_ch == '\r' || this_ch == '\n' || this_ch == ' ')
            continue;

        /* Check for pad sequences and ignore
        ** the invalid ones.
        */
        if (this_ch == BASE64_PAD) {
            if ( (quad_pos < 2) ||
                 ((quad_pos == 2) &&
                  (binascii_find_valid(ascii_data, ascii_len, 1)
                   != BASE64_PAD)) )
            {
                continue;
            }
            else {
                /* A pad sequence means no more input.
                ** We've already interpreted the data
                ** from the quad at this point.
                */
                leftbits = 0;
                break;
            }
        }

        this_ch = table_a2b_base64[*ascii_data];
        if ( this_ch == (unsigned char) -1 )
            continue;

        /*
        ** Shift it in on the low end, and see if there's
        ** a byte ready for output.
        */
        quad_pos = (quad_pos + 1) & 0x03;
        leftchar = (leftchar << 6) | (this_ch);
        leftbits += 6;

        if ( leftbits >= 8 ) {
            leftbits -= 8;
            *bin_data++ = (leftchar >> leftbits) & 0xff;
            leftchar &= ((1 << leftbits) - 1);
        }
    }

    if (leftbits != 0) {
        PyErr_SetString(Error, "Incorrect padding");
        _PyBytesWriter_Dealloc(&writer);
        return NULL;
    }

    return _PyBytesWriter_Finish(&writer, bin_data);
}


/*[clinic input]
binascii.b2a_base64

    data: Py_buffer
    *
    newline: int(c_default="1") = True

Base64-code line of data.
[clinic start generated code]*/

static PyObject *
binascii_b2a_base64_impl(PyObject *module, Py_buffer *data, int newline)
/*[clinic end generated code: output=4ad62c8e8485d3b3 input=7b2ea6fa38d8924c]*/
{
    unsigned char *ascii_data;
    const unsigned char *bin_data;
    int leftbits = 0;
    unsigned char this_ch;
    unsigned int leftchar = 0;
    Py_ssize_t bin_len, out_len;
    _PyBytesWriter writer;

    bin_data = data->buf;
    bin_len = data->len;
    _PyBytesWriter_Init(&writer);

    assert(bin_len >= 0);

    if ( bin_len > BASE64_MAXBIN ) {
        PyErr_SetString(Error, "Too much data for base64 line");
        return NULL;
    }

    /* We're lazy and allocate too much (fixed up later).
       "+2" leaves room for up to two pad characters.
       Note that 'b' gets encoded as 'Yg==\n' (1 in, 5 out). */
    out_len = bin_len*2 + 2;
    if (newline)
        out_len++;
    ascii_data = _PyBytesWriter_Alloc(&writer, out_len);
    if (ascii_data == NULL)
        return NULL;

    for( ; bin_len > 0 ; bin_len--, bin_data++ ) {
        /* Shift the data into our buffer */
        leftchar = (leftchar << 8) | *bin_data;
        leftbits += 8;

        /* See if there are 6-bit groups ready */
        while ( leftbits >= 6 ) {
            this_ch = (leftchar >> (leftbits-6)) & 0x3f;
            leftbits -= 6;
            *ascii_data++ = table_b2a_base64[this_ch];
        }
    }
    if ( leftbits == 2 ) {
        *ascii_data++ = table_b2a_base64[(leftchar&3) << 4];
        *ascii_data++ = BASE64_PAD;
        *ascii_data++ = BASE64_PAD;
    } else if ( leftbits == 4 ) {
        *ascii_data++ = table_b2a_base64[(leftchar&0xf) << 2];
        *ascii_data++ = BASE64_PAD;
    }
    if (newline)
        *ascii_data++ = '\n';       /* Append a courtesy newline */

    return _PyBytesWriter_Finish(&writer, ascii_data);
}

/*[clinic input]
binascii.a2b_hqx

    data: ascii_buffer
    /

Decode .hqx coding.
[clinic start generated code]*/

static PyObject *
binascii_a2b_hqx_impl(PyObject *module, Py_buffer *data)
/*[clinic end generated code: output=4d6d8c54d54ea1c1 input=0d914c680e0eed55]*/
{
    const unsigned char *ascii_data;
    unsigned char *bin_data;
    int leftbits = 0;
    unsigned char this_ch;
    unsigned int leftchar = 0;
    PyObject *res;
    Py_ssize_t len;
    int done = 0;
    _PyBytesWriter writer;

    ascii_data = data->buf;
    len = data->len;
    _PyBytesWriter_Init(&writer);

    assert(len >= 0);

    if (len > PY_SSIZE_T_MAX - 2)
        return PyErr_NoMemory();

    /* Allocate a string that is too big (fixed later)
       Add two to the initial length to prevent interning which
       would preclude subsequent resizing.  */
    bin_data = _PyBytesWriter_Alloc(&writer, len + 2);
    if (bin_data == NULL)
        return NULL;

    for( ; len > 0 ; len--, ascii_data++ ) {
        /* Get the byte and look it up */
        this_ch = table_a2b_hqx[*ascii_data];
        if ( this_ch == SKIP )
            continue;
        if ( this_ch == FAIL ) {
            PyErr_SetString(Error, "Illegal char");
            _PyBytesWriter_Dealloc(&writer);
            return NULL;
        }
        if ( this_ch == DONE ) {
            /* The terminating colon */
            done = 1;
            break;
        }

        /* Shift it into the buffer and see if any bytes are ready */
        leftchar = (leftchar << 6) | (this_ch);
        leftbits += 6;
        if ( leftbits >= 8 ) {
            leftbits -= 8;
            *bin_data++ = (leftchar >> leftbits) & 0xff;
            leftchar &= ((1 << leftbits) - 1);
        }
    }

    if ( leftbits && !done ) {
        PyErr_SetString(Incomplete,
                        "String has incomplete number of bytes");
        _PyBytesWriter_Dealloc(&writer);
        return NULL;
    }

    res = _PyBytesWriter_Finish(&writer, bin_data);
    if (res == NULL)
        return NULL;
    return Py_BuildValue("Ni", res, done);
}


/*[clinic input]
binascii.rlecode_hqx

    data: Py_buffer
    /

Binhex RLE-code binary data.
[clinic start generated code]*/

static PyObject *
binascii_rlecode_hqx_impl(PyObject *module, Py_buffer *data)
/*[clinic end generated code: output=393d79338f5f5629 input=e1f1712447a82b09]*/
{
    const unsigned char *in_data;
    unsigned char *out_data;
    unsigned char ch;
    Py_ssize_t in, inend, len;
    _PyBytesWriter writer;

    _PyBytesWriter_Init(&writer);
    in_data = data->buf;
    len = data->len;

    assert(len >= 0);

    if (len > PY_SSIZE_T_MAX / 2 - 2)
        return PyErr_NoMemory();

    /* Worst case: output is twice as big as input (fixed later) */
    out_data = _PyBytesWriter_Alloc(&writer, len * 2 + 2);
    if (out_data == NULL)
        return NULL;

    for( in=0; in<len; in++) {
        ch = in_data[in];
        if ( ch == RUNCHAR ) {
            /* RUNCHAR. Escape it. */
            *out_data++ = RUNCHAR;
            *out_data++ = 0;
        } else {
            /* Check how many following are the same */
            for(inend=in+1;
                inend<len && in_data[inend] == ch &&
                    inend < in+255;
                inend++) ;
            if ( inend - in > 3 ) {
                /* More than 3 in a row. Output RLE. */
                *out_data++ = ch;
                *out_data++ = RUNCHAR;
                *out_data++ = (unsigned char) (inend-in);
                in = inend-1;
            } else {
                /* Less than 3. Output the byte itself */
                *out_data++ = ch;
            }
        }
    }

    return _PyBytesWriter_Finish(&writer, out_data);
}


/*[clinic input]
binascii.b2a_hqx

    data: Py_buffer
    /

Encode .hqx data.
[clinic start generated code]*/

static PyObject *
binascii_b2a_hqx_impl(PyObject *module, Py_buffer *data)
/*[clinic end generated code: output=d0aa5a704bc9f7de input=9596ebe019fe12ba]*/
{
    unsigned char *ascii_data;
    const unsigned char *bin_data;
    int leftbits = 0;
    unsigned char this_ch;
    unsigned int leftchar = 0;
    Py_ssize_t len;
    _PyBytesWriter writer;

    bin_data = data->buf;
    len = data->len;
    _PyBytesWriter_Init(&writer);

    assert(len >= 0);

    if (len > PY_SSIZE_T_MAX / 2 - 2)
        return PyErr_NoMemory();

    /* Allocate a buffer that is at least large enough */
    ascii_data = _PyBytesWriter_Alloc(&writer, len * 2 + 2);
    if (ascii_data == NULL)
        return NULL;

    for( ; len > 0 ; len--, bin_data++ ) {
        /* Shift into our buffer, and output any 6bits ready */
        leftchar = (leftchar << 8) | *bin_data;
        leftbits += 8;
        while ( leftbits >= 6 ) {
            this_ch = (leftchar >> (leftbits-6)) & 0x3f;
            leftbits -= 6;
            *ascii_data++ = table_b2a_hqx[this_ch];
        }
    }
    /* Output a possible runt byte */
    if ( leftbits ) {
        leftchar <<= (6-leftbits);
        *ascii_data++ = table_b2a_hqx[leftchar & 0x3f];
    }

    return _PyBytesWriter_Finish(&writer, ascii_data);
}


/*[clinic input]
binascii.rledecode_hqx

    data: Py_buffer
    /

Decode hexbin RLE-coded string.
[clinic start generated code]*/

static PyObject *
binascii_rledecode_hqx_impl(PyObject *module, Py_buffer *data)
/*[clinic end generated code: output=9826619565de1c6c input=54cdd49fc014402c]*/
{
    const unsigned char *in_data;
    unsigned char *out_data;
    unsigned char in_byte, in_repeat;
    Py_ssize_t in_len;
    _PyBytesWriter writer;

    in_data = data->buf;
    in_len = data->len;
    _PyBytesWriter_Init(&writer);

    assert(in_len >= 0);

    /* Empty string is a special case */
    if ( in_len == 0 )
        return PyBytes_FromStringAndSize("", 0);
    else if (in_len > PY_SSIZE_T_MAX / 2)
        return PyErr_NoMemory();

    /* Allocate a buffer of reasonable size. Resized when needed */
    out_data = _PyBytesWriter_Alloc(&writer, in_len);
    if (out_data == NULL)
        return NULL;

    /* Use overallocation */
    writer.overallocate = 1;

    /*
    ** We need two macros here to get/put bytes and handle
    ** end-of-buffer for input and output strings.
    */
#define INBYTE(b)                                                       \
    do {                                                                \
         if ( --in_len < 0 ) {                                          \
           PyErr_SetString(Incomplete, "");                             \
           goto error;                                                  \
         }                                                              \
         b = *in_data++;                                                \
    } while(0)

    /*
    ** Handle first byte separately (since we have to get angry
    ** in case of an orphaned RLE code).
    */
    INBYTE(in_byte);

    if (in_byte == RUNCHAR) {
        INBYTE(in_repeat);
        /* only 1 byte will be written, but 2 bytes were preallocated:
           subtract 1 byte to prevent overallocation */
        writer.min_size--;

        if (in_repeat != 0) {
            /* Note Error, not Incomplete (which is at the end
            ** of the string only). This is a programmer error.
            */
            PyErr_SetString(Error, "Orphaned RLE code at start");
            goto error;
        }
        *out_data++ = RUNCHAR;
    } else {
        *out_data++ = in_byte;
    }

    while( in_len > 0 ) {
        INBYTE(in_byte);

        if (in_byte == RUNCHAR) {
            INBYTE(in_repeat);
            /* only 1 byte will be written, but 2 bytes were preallocated:
               subtract 1 byte to prevent overallocation */
            writer.min_size--;

            if ( in_repeat == 0 ) {
                /* Just an escaped RUNCHAR value */
                *out_data++ = RUNCHAR;
            } else {
                /* Pick up value and output a sequence of it */
                in_byte = out_data[-1];

                /* enlarge the buffer if needed */
                if (in_repeat > 1) {
                    /* -1 because we already preallocated 1 byte */
                    out_data = _PyBytesWriter_Prepare(&writer, out_data,
                                                      in_repeat - 1);
                    if (out_data == NULL)
                        goto error;
                }

                while ( --in_repeat > 0 )
                    *out_data++ = in_byte;
            }
        } else {
            /* Normal byte */
            *out_data++ = in_byte;
        }
    }
    return _PyBytesWriter_Finish(&writer, out_data);

error:
    _PyBytesWriter_Dealloc(&writer);
    return NULL;
}


/*[clinic input]
binascii.crc_hqx -> unsigned_int

    data: Py_buffer
    crc: unsigned_int(bitwise=True)
    /

Compute CRC-CCITT incrementally.
[clinic start generated code]*/

static unsigned int
binascii_crc_hqx_impl(PyObject *module, Py_buffer *data, unsigned int crc)
/*[clinic end generated code: output=8ec2a78590d19170 input=f18240ff8c705b79]*/
{
    const unsigned char *bin_data;
    Py_ssize_t len;

    crc &= 0xffff;
    bin_data = data->buf;
    len = data->len;

    while(len-- > 0) {
        crc = ((crc<<8)&0xff00) ^ crctab_hqx[(crc>>8)^*bin_data++];
    }

    return crc;
}

/*[clinic input]
binascii.crc32 -> unsigned_int

    data: Py_buffer
    crc: unsigned_int(bitwise=True) = 0
    /

Compute CRC-32 incrementally.
[clinic start generated code]*/

static unsigned int
binascii_crc32_impl(PyObject *module, Py_buffer *data, unsigned int crc)
/*[clinic end generated code: output=52cf59056a78593b input=bbe340bc99d25aa8]*/

/* This was taken from zlibmodule.c PyZlib_crc32 (but is PY_SSIZE_T_CLEAN) */
{
    const Byte *buf;
    Py_ssize_t len;
    int signed_val;

    buf = (Byte*)data->buf;
    len = data->len;
    signed_val = crc32(crc, buf, len);
    return (unsigned int)signed_val & 0xffffffffU;
}

/*[clinic input]
binascii.b2a_hex

    data: Py_buffer
    /

Hexadecimal representation of binary data.

The return value is a bytes object.  This function is also
available as "hexlify()".
[clinic start generated code]*/

static PyObject *
binascii_b2a_hex_impl(PyObject *module, Py_buffer *data)
/*[clinic end generated code: output=92fec1a95c9897a0 input=96423cfa299ff3b1]*/
{
    return _Py_strhex_bytes((const char *)data->buf, data->len);
}

/*[clinic input]
binascii.hexlify = binascii.b2a_hex

Hexadecimal representation of binary data.

The return value is a bytes object.
[clinic start generated code]*/

static PyObject *
binascii_hexlify_impl(PyObject *module, Py_buffer *data)
/*[clinic end generated code: output=749e95e53c14880c input=2e3afae7f083f061]*/
{
    return _Py_strhex_bytes((const char *)data->buf, data->len);
}

static int
to_int(int c)
{
    if (Py_ISDIGIT(c))
        return c - '0';
    else {
        if (Py_ISUPPER(c))
            c = Py_TOLOWER(c);
        if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
    }
    return -1;
}


/*[clinic input]
binascii.a2b_hex

    hexstr: ascii_buffer
    /

Binary data of hexadecimal representation.

hexstr must contain an even number of hex digits (upper or lower case).
This function is also available as "unhexlify()".
[clinic start generated code]*/

static PyObject *
binascii_a2b_hex_impl(PyObject *module, Py_buffer *hexstr)
/*[clinic end generated code: output=0cc1a139af0eeecb input=9e1e7f2f94db24fd]*/
{
    const char* argbuf;
    Py_ssize_t arglen;
    PyObject *retval;
    char* retbuf;
    Py_ssize_t i, j;

    argbuf = hexstr->buf;
    arglen = hexstr->len;

    assert(arglen >= 0);

    /* XXX What should we do about strings with an odd length?  Should
     * we add an implicit leading zero, or a trailing zero?  For now,
     * raise an exception.
     */
    if (arglen % 2) {
        PyErr_SetString(Error, "Odd-length string");
        return NULL;
    }

    retval = PyBytes_FromStringAndSize(NULL, (arglen/2));
    if (!retval)
        return NULL;
    retbuf = PyBytes_AS_STRING(retval);

    for (i=j=0; i < arglen; i += 2) {
        int top = to_int(Py_CHARMASK(argbuf[i]));
        int bot = to_int(Py_CHARMASK(argbuf[i+1]));
        if (top == -1 || bot == -1) {
            PyErr_SetString(Error,
                            "Non-hexadecimal digit found");
            goto finally;
        }
        retbuf[j++] = (top << 4) + bot;
    }
    return retval;

  finally:
    Py_DECREF(retval);
    return NULL;
}

/*[clinic input]
binascii.unhexlify = binascii.a2b_hex

Binary data of hexadecimal representation.

hexstr must contain an even number of hex digits (upper or lower case).
[clinic start generated code]*/

static PyObject *
binascii_unhexlify_impl(PyObject *module, Py_buffer *hexstr)
/*[clinic end generated code: output=51a64c06c79629e3 input=dd8c012725f462da]*/
{
    return binascii_a2b_hex_impl(module, hexstr);
}

static const int table_hex[128] = {
  -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
  -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
  -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
   0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
  -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
  -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
  -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
  -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
};

#define hexval(c) table_hex[(unsigned int)(c)]

#define MAXLINESIZE 76


/*[clinic input]
binascii.a2b_qp

    data: ascii_buffer
    header: int(c_default="0") = False

Decode a string of qp-encoded data.
[clinic start generated code]*/

static PyObject *
binascii_a2b_qp_impl(PyObject *module, Py_buffer *data, int header)
/*[clinic end generated code: output=e99f7846cfb9bc53 input=5187a0d3d8e54f3b]*/
{
    Py_ssize_t in, out;
    char ch;
    const unsigned char *ascii_data;
    unsigned char *odata;
    Py_ssize_t datalen = 0;
    PyObject *rv;

    ascii_data = data->buf;
    datalen = data->len;

    /* We allocate the output same size as input, this is overkill.
     * The previous implementation used calloc() so we'll zero out the
     * memory here too, since PyMem_Malloc() does not guarantee that.
     */
    odata = (unsigned char *) PyMem_Malloc(datalen);
    if (odata == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    bzero(odata, datalen);

    in = out = 0;
    while (in < datalen) {
        if (ascii_data[in] == '=') {
            in++;
            if (in >= datalen) break;
            /* Soft line breaks */
            if ((ascii_data[in] == '\n') || (ascii_data[in] == '\r')) {
                if (ascii_data[in] != '\n') {
                    while (in < datalen && ascii_data[in] != '\n') in++;
                }
                if (in < datalen) in++;
            }
            else if (ascii_data[in] == '=') {
                /* broken case from broken python qp */
                odata[out++] = '=';
                in++;
            }
            else if ((in + 1 < datalen) &&
                     ((ascii_data[in] >= 'A' && ascii_data[in] <= 'F') ||
                      (ascii_data[in] >= 'a' && ascii_data[in] <= 'f') ||
                      (ascii_data[in] >= '0' && ascii_data[in] <= '9')) &&
                     ((ascii_data[in+1] >= 'A' && ascii_data[in+1] <= 'F') ||
                      (ascii_data[in+1] >= 'a' && ascii_data[in+1] <= 'f') ||
                      (ascii_data[in+1] >= '0' && ascii_data[in+1] <= '9'))) {
                /* hexval */
                ch = hexval(ascii_data[in]) << 4;
                in++;
                ch |= hexval(ascii_data[in]);
                in++;
                odata[out++] = ch;
            }
            else {
              odata[out++] = '=';
            }
        }
        else if (header && ascii_data[in] == '_') {
            odata[out++] = ' ';
            in++;
        }
        else {
            odata[out] = ascii_data[in];
            in++;
            out++;
        }
    }
    if ((rv = PyBytes_FromStringAndSize((char *)odata, out)) == NULL) {
        PyMem_Free(odata);
        return NULL;
    }
    PyMem_Free(odata);
    return rv;
}

static int
to_hex (unsigned char ch, unsigned char *s)
{
    unsigned int uvalue = ch;

    s[1] = "0123456789ABCDEF"[uvalue % 16];
    uvalue = (uvalue / 16);
    s[0] = "0123456789ABCDEF"[uvalue % 16];
    return 0;
}

/* XXX: This is ridiculously complicated to be backward compatible
 * (mostly) with the quopri module.  It doesn't re-create the quopri
 * module bug where text ending in CRLF has the CR encoded */

/*[clinic input]
binascii.b2a_qp

    data: Py_buffer
    quotetabs: int(c_default="0") = False
    istext: int(c_default="1") = True
    header: int(c_default="0") = False

Encode a string using quoted-printable encoding.

On encoding, when istext is set, newlines are not encoded, and white
space at end of lines is.  When istext is not set, \r and \n (CR/LF)
are both encoded.  When quotetabs is set, space and tabs are encoded.
[clinic start generated code]*/

static PyObject *
binascii_b2a_qp_impl(PyObject *module, Py_buffer *data, int quotetabs,
                     int istext, int header)
/*[clinic end generated code: output=e9884472ebb1a94c input=7f2a9aaa008e92b2]*/
{
    Py_ssize_t in, out;
    const unsigned char *databuf;
    unsigned char *odata;
    Py_ssize_t datalen = 0, odatalen = 0;
    PyObject *rv;
    unsigned int linelen = 0;
    unsigned char ch;
    int crlf = 0;
    const unsigned char *p;

    databuf = data->buf;
    datalen = data->len;

    /* See if this string is using CRLF line ends */
    /* XXX: this function has the side effect of converting all of
     * the end of lines to be the same depending on this detection
     * here */
    p = (const unsigned char *) memchr(databuf, '\n', datalen);
    if ((p != NULL) && (p > databuf) && (*(p-1) == '\r'))
        crlf = 1;

    /* First, scan to see how many characters need to be encoded */
    in = 0;
    while (in < datalen) {
        Py_ssize_t delta = 0;
        if ((databuf[in] > 126) ||
            (databuf[in] == '=') ||
            (header && databuf[in] == '_') ||
            ((databuf[in] == '.') && (linelen == 0) &&
             (in + 1 == datalen || databuf[in+1] == '\n' ||
              databuf[in+1] == '\r' || databuf[in+1] == 0)) ||
            (!istext && ((databuf[in] == '\r') || (databuf[in] == '\n'))) ||
            ((databuf[in] == '\t' || databuf[in] == ' ') && (in + 1 == datalen)) ||
            ((databuf[in] < 33) &&
             (databuf[in] != '\r') && (databuf[in] != '\n') &&
             (quotetabs || ((databuf[in] != '\t') && (databuf[in] != ' ')))))
        {
            if ((linelen + 3) >= MAXLINESIZE) {
                linelen = 0;
                if (crlf)
                    delta += 3;
                else
                    delta += 2;
            }
            linelen += 3;
            delta += 3;
            in++;
        }
        else {
            if (istext &&
                ((databuf[in] == '\n') ||
                 ((in+1 < datalen) && (databuf[in] == '\r') &&
                 (databuf[in+1] == '\n'))))
            {
                linelen = 0;
                /* Protect against whitespace on end of line */
                if (in && ((databuf[in-1] == ' ') || (databuf[in-1] == '\t')))
                    delta += 2;
                if (crlf)
                    delta += 2;
                else
                    delta += 1;
                if (databuf[in] == '\r')
                    in += 2;
                else
                    in++;
            }
            else {
                if ((in + 1 != datalen) &&
                    (databuf[in+1] != '\n') &&
                    (linelen + 1) >= MAXLINESIZE) {
                    linelen = 0;
                    if (crlf)
                        delta += 3;
                    else
                        delta += 2;
                }
                linelen++;
                delta++;
                in++;
            }
        }
        if (PY_SSIZE_T_MAX - delta < odatalen) {
            PyErr_NoMemory();
            return NULL;
        }
        odatalen += delta;
    }

    /* We allocate the output same size as input, this is overkill.
     * The previous implementation used calloc() so we'll zero out the
     * memory here too, since PyMem_Malloc() does not guarantee that.
     */
    odata = (unsigned char *) PyMem_Malloc(odatalen);
    if (odata == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    bzero(odata, odatalen);

    in = out = linelen = 0;
    while (in < datalen) {
        if ((databuf[in] > 126) ||
            (databuf[in] == '=') ||
            (header && databuf[in] == '_') ||
            ((databuf[in] == '.') && (linelen == 0) &&
             (in + 1 == datalen || databuf[in+1] == '\n' ||
              databuf[in+1] == '\r' || databuf[in+1] == 0)) ||
            (!istext && ((databuf[in] == '\r') || (databuf[in] == '\n'))) ||
            ((databuf[in] == '\t' || databuf[in] == ' ') && (in + 1 == datalen)) ||
            ((databuf[in] < 33) &&
             (databuf[in] != '\r') && (databuf[in] != '\n') &&
             (quotetabs || ((databuf[in] != '\t') && (databuf[in] != ' ')))))
        {
            if ((linelen + 3 )>= MAXLINESIZE) {
                odata[out++] = '=';
                if (crlf) odata[out++] = '\r';
                odata[out++] = '\n';
                linelen = 0;
            }
            odata[out++] = '=';
            to_hex(databuf[in], &odata[out]);
            out += 2;
            in++;
            linelen += 3;
        }
        else {
            if (istext &&
                ((databuf[in] == '\n') ||
                 ((in+1 < datalen) && (databuf[in] == '\r') &&
                 (databuf[in+1] == '\n'))))
            {
                linelen = 0;
                /* Protect against whitespace on end of line */
                if (out && ((odata[out-1] == ' ') || (odata[out-1] == '\t'))) {
                    ch = odata[out-1];
                    odata[out-1] = '=';
                    to_hex(ch, &odata[out]);
                    out += 2;
                }

                if (crlf) odata[out++] = '\r';
                odata[out++] = '\n';
                if (databuf[in] == '\r')
                    in += 2;
                else
                    in++;
            }
            else {
                if ((in + 1 != datalen) &&
                    (databuf[in+1] != '\n') &&
                    (linelen + 1) >= MAXLINESIZE) {
                    odata[out++] = '=';
                    if (crlf) odata[out++] = '\r';
                    odata[out++] = '\n';
                    linelen = 0;
                }
                linelen++;
                if (header && databuf[in] == ' ') {
                    odata[out++] = '_';
                    in++;
                }
                else {
                    odata[out++] = databuf[in++];
                }
            }
        }
    }
    if ((rv = PyBytes_FromStringAndSize((char *)odata, out)) == NULL) {
        PyMem_Free(odata);
        return NULL;
    }
    PyMem_Free(odata);
    return rv;
}

/* List of functions defined in the module */

static struct PyMethodDef binascii_module_methods[] = {
    BINASCII_A2B_UU_METHODDEF
    BINASCII_B2A_UU_METHODDEF
    BINASCII_A2B_BASE64_METHODDEF
    BINASCII_B2A_BASE64_METHODDEF
    BINASCII_A2B_HQX_METHODDEF
    BINASCII_B2A_HQX_METHODDEF
    BINASCII_A2B_HEX_METHODDEF
    BINASCII_B2A_HEX_METHODDEF
    BINASCII_HEXLIFY_METHODDEF
    BINASCII_UNHEXLIFY_METHODDEF
    BINASCII_RLECODE_HQX_METHODDEF
    BINASCII_RLEDECODE_HQX_METHODDEF
    BINASCII_CRC_HQX_METHODDEF
    BINASCII_CRC32_METHODDEF
    BINASCII_A2B_QP_METHODDEF
    BINASCII_B2A_QP_METHODDEF
    {NULL, NULL}                             /* sentinel */
};

/* Initialization function for the module (*must* be called PyInit_binascii) */
PyDoc_STRVAR(doc_binascii, "Conversion between binary data and ASCII");

static struct PyModuleDef binasciimodule = {
    PyModuleDef_HEAD_INIT,
    "binascii",
    doc_binascii,
    -1,
    binascii_module_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_binascii(void)
{
    PyObject *m, *d;

    /* Create the module and add the functions */
    m = PyModule_Create(&binasciimodule);
    if (m == NULL)
        return NULL;

    d = PyModule_GetDict(m);

    Error = PyErr_NewException("binascii.Error", PyExc_ValueError, NULL);
    PyDict_SetItemString(d, "Error", Error);
    Incomplete = PyErr_NewException("binascii.Incomplete", NULL, NULL);
    PyDict_SetItemString(d, "Incomplete", Incomplete);
    if (PyErr_Occurred()) {
        Py_DECREF(m);
        m = NULL;
    }
    return m;
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab_binascii = {
    "binascii",
    PyInit_binascii,
};
