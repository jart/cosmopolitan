/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/pyctype.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Modules/bextra.h"
#include "third_party/python/Modules/unicodedata.h"
#include "third_party/python/Modules/unicodedata_unidata.h"

/* macros used to determine if the given code point is in the PUA range that
 * we are using to store aliases and named sequences */
#define IS_ALIAS(cp) ((cp >= _PyUnicode_AliasesStart) && \
                      (cp <  _PyUnicode_AliasesEnd))
#define IS_NAMED_SEQ(cp) ((cp >= _PyUnicode_NamedSequencesStart) && \
                          (cp <  _PyUnicode_NamedSequencesEnd))

static const char kHangulSyllables[][3][4] = {
    { "G",  "A",   ""   },
    { "GG", "AE",  "G"  },
    { "N",  "YA",  "GG" },
    { "D",  "YAE", "GS" },
    { "DD", "EO",  "N", },
    { "R",  "E",   "NJ" },
    { "M",  "YEO", "NH" },
    { "B",  "YE",  "D"  },
    { "BB", "O",   "L"  },
    { "S",  "WA",  "LG" },
    { "SS", "WAE", "LM" },
    { "",   "OE",  "LB" },
    { "J",  "YO",  "LS" },
    { "JJ", "U",   "LT" },
    { "C",  "WEO", "LP" },
    { "K",  "WE",  "LH" },
    { "T",  "WI",  "M"  },
    { "P",  "YU",  "B"  },
    { "H",  "EU",  "BS" },
    { "",   "YI",  "S"  },
    { "",   "I",   "SS" },
    { "",   "",    "NG" },
    { "",   "",    "J"  },
    { "",   "",    "C"  },
    { "",   "",    "K"  },
    { "",   "",    "T"  },
    { "",   "",    "P"  },
    { "",   "",    "H"  }
};

void
_PyUnicode_FindSyllable(const char *str, int *len, int *pos,
                        int count, int column)
{
    int i, len1;
    *len = -1;
    for (i = 0; i < count; i++) {
        const char *s = kHangulSyllables[i][column];
        len1 = Py_SAFE_DOWNCAST(strlen(s), size_t, int);
        if (len1 <= *len)
            continue;
        if (strncmp(str, s, len1) == 0) {
            *len = len1;
            *pos = i;
        }
    }
    if (*len == -1) {
        *len = 0;
    }
}

static unsigned long
_gethash(const char *s, int len, int scale)
{
    int i;
    unsigned long h = 0;
    unsigned long ix;
    for (i = 0; i < len; i++) {
        h = (h * scale) + (unsigned char) Py_TOUPPER(Py_CHARMASK(s[i]));
        ix = h & 0xff000000;
        if (ix)
            h = (h ^ ((ix>>24) & 0xff)) & 0x00ffffff;
    }
    return h;
}

static int
_cmpname(PyObject *self, int code, const char* name, int namelen)
{
    /* check if code corresponds to the given name */
    int i;
    char buffer[UNIDATA_NAME_MAXLEN+1];
    if (!_PyUnicode_GetUcName(self, code, buffer, UNIDATA_NAME_MAXLEN, 1))
        return 0;
    for (i = 0; i < namelen; i++) {
        if (Py_TOUPPER(Py_CHARMASK(name[i])) != buffer[i])
            return 0;
    }
    return buffer[namelen] == '\0';
}

static int
_check_alias_and_seq(unsigned int cp, Py_UCS4* code, int with_named_seq)
{
    /* check if named sequences are allowed */
    if (!with_named_seq && IS_NAMED_SEQ(cp))
        return 0;
    /* if the code point is in the PUA range that we use for aliases,
     * convert it to obtain the right code point */
    if (IS_ALIAS(cp))
        *code = _PyUnicode_NameAliases[cp - _PyUnicode_AliasesStart];
    else
        *code = cp;
    return 1;
}

int
_PyUnicode_GetCode(PyObject *self, const char *name, int namelen, Py_UCS4 *code,
                   int with_named_seq)
{
    /* Return the code point associated with the given name.
     * Named aliases are resolved too (unless self != NULL (i.e. we are using
     * 3.2.0)).  If with_named_seq is 1, returns the PUA code point that we are
     * using for the named sequence, and the caller must then convert it. */
    unsigned int h, v;
    unsigned int mask = _PyUnicode_CodeSize - 1;
    unsigned int i, incr;
    /* Check for hangul syllables. */
    if (strncmp(name, "HANGUL SYLLABLE ", 16) == 0) {
        int len, L = -1, V = -1, T = -1;
        const char *pos = name + 16;
        _PyUnicode_FindSyllable(pos, &len, &L, _Hanghoul_LCount, 0);
        pos += len;
        _PyUnicode_FindSyllable(pos, &len, &V, _Hanghoul_VCount, 1);
        pos += len;
        _PyUnicode_FindSyllable(pos, &len, &T, _Hanghoul_TCount, 2);
        pos += len;
        if (L != -1 && V != -1 && T != -1 && pos-name == namelen) {
            *code = _Hanghoul_SBase + (L * _Hanghoul_VCount+V) * _Hanghoul_TCount + T;
            return 1;
        }
        /* Otherwise, it's an illegal syllable name. */
        return 0;
    }
    /* Check for unified ideographs. */
    if (strncmp(name, "CJK UNIFIED IDEOGRAPH-", 22) == 0) {
        /* Four or five hexdigits must follow. */
        v = 0;
        name += 22;
        namelen -= 22;
        if (namelen != 4 && namelen != 5)
            return 0;
        while (namelen--) {
            v *= 16;
            if (*name >= '0' && *name <= '9')
                v += *name - '0';
            else if (*name >= 'A' && *name <= 'F')
                v += *name - 'A' + 10;
            else
                return 0;
            name++;
        }
        if (!_PyUnicode_IsUnifiedIdeograph(v))
            return 0;
        *code = v;
        return 1;
    }
    /* the following is the same as python's dictionary lookup, with
       only minor changes.  see the makeunicodedata script for more
       details */
    h = (unsigned int)_gethash(name, namelen, _PyUnicode_CodeMagic);
    i = ~h & mask;
    v = BitFieldExtract(_PyUnicode_CodeHash, i, _PyUnicode_CodeHashBits);
    if (!v)
        return 0;
    if (_cmpname(self, v, name, namelen))
        return _check_alias_and_seq(v, code, with_named_seq);
    incr = (h ^ (h >> 3)) & mask;
    if (!incr)
        incr = mask;
    for (;;) {
        i = (i + incr) & mask;
        v = BitFieldExtract(_PyUnicode_CodeHash, i, _PyUnicode_CodeHashBits);
        if (!v)
            return 0;
        if (_cmpname(self, v, name, namelen))
            return _check_alias_and_seq(v, code, with_named_seq);
        incr = incr << 1;
        if (incr > mask)
            incr = incr ^ _PyUnicode_CodePoly;
    }
}

int
_PyUnicode_GetUcName(PyObject *self, Py_UCS4 code, char *buffer, int buflen,
                     int with_alias_and_seq)
{
    /* Find the name associated with the given code point.
     * If with_alias_and_seq is 1, check for names in the Private Use Area 15
     * that we are using for aliases and named sequences. */
    char *p;
    unsigned char *w;
    int i, word, offset;
    if (code >= 0x110000)
        return 0;
    /* XXX should we just skip all the code points in the PUAs here? */
    if (!with_alias_and_seq && (IS_ALIAS(code) || IS_NAMED_SEQ(code)))
        return 0;
    if (self && UCD_Check(self)) {
        /* in 3.2.0 there are no aliases and named sequences */
        const _PyUnicode_ChangeRecord *old;
        if (IS_ALIAS(code) || IS_NAMED_SEQ(code))
            return 0;
        old = get_old_record(self, code);
        if (!old->category_changed) {
            /* unassigned */
            return 0;
        }
    }
    if (_Hanghoul_SBase <= code && code < _Hanghoul_SBase + _Hanghoul_SCount) {
        /* Hangul syllable. */
        int SIndex = code - _Hanghoul_SBase;
        int L = SIndex / _Hanghoul_NCount;
        int V = (SIndex % _Hanghoul_NCount) / _Hanghoul_TCount;
        int T = SIndex % _Hanghoul_TCount;
        if (buflen < 27)
            /* Worst case: HANGUL SYLLABLE <10chars>. */
            return 0;
        p = buffer;
        p = stpcpy(p, "HANGUL SYLLABLE ");
        p = stpcpy(p, kHangulSyllables[L][0]);
        p = stpcpy(p, kHangulSyllables[V][1]);
        p = stpcpy(p, kHangulSyllables[T][2]);
        *p = 0;
        return 1;
    }
    if (_PyUnicode_IsUnifiedIdeograph(code)) {
        if (buflen < 28)
            /* Worst case: CJK UNIFIED IDEOGRAPH-20000 */
            return 0;
        sprintf(buffer, "CJK UNIFIED IDEOGRAPH-%X", code);
        return 1;
    }
    /* get offset into phrasebook */
    offset = _PyUnicode_PhrasebookOffset1[(code>>_PyUnicode_PhrasebookShift)];
    offset = BitFieldExtract(_PyUnicode_PhrasebookOffset2,
                             (offset << _PyUnicode_PhrasebookShift) +
                             (code & ((1 << _PyUnicode_PhrasebookShift) - 1)),
                             _PyUnicode_PhrasebookOffset2Bits);
    if (!offset)
        return 0;
    i = 0;
    for (;;) {
        /* get word index */
        word = _PyUnicode_Phrasebook[offset] - _PyUnicode_PhrasebookShort;
        if (word >= 0) {
            word = (word << 8) + _PyUnicode_Phrasebook[offset+1];
            offset += 2;
        } else
            word = _PyUnicode_Phrasebook[offset++];
        if (i) {
            if (i > buflen)
                return 0; /* buffer overflow */
            buffer[i++] = ' ';
        }
        /* copy word string from lexicon.  the last character in the
           word has bit 7 set.  the last word in a string ends with
           0x80 */
        w = (void *)(_PyUnicode_Lexicon +
             BitFieldExtract(_PyUnicode_LexiconOffset, word,
                             _PyUnicode_LexiconOffsetBits));
        while (*w < 128) {
            if (i >= buflen)
                return 0; /* buffer overflow */
            buffer[i++] = *w++;
        }
        if (i >= buflen)
            return 0; /* buffer overflow */
        buffer[i++] = *w & 127;
        if (*w == 128)
            break; /* end of word */
    }
    return 1;
}
