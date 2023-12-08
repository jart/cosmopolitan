/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/codecs.h"
#include "third_party/python/Include/fileobject.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/pystate.h"
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Include/yoink.h"

_Py_IDENTIFIER(name);

static char *
get_codec_name(const char *encoding)
{
    char *name_utf8, *name_str;
    PyObject *codec, *name = NULL;
    codec = _PyCodec_Lookup(encoding);
    if (!codec)
        goto error;
    name = _PyObject_GetAttrId(codec, &PyId_name);
    Py_CLEAR(codec);
    if (!name)
        goto error;
    name_utf8 = PyUnicode_AsUTF8(name);
    if (name_utf8 == NULL)
        goto error;
    name_str = _PyMem_RawStrdup(name_utf8);
    Py_DECREF(name);
    if (name_str == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    return name_str;
error:
    Py_XDECREF(codec);
    Py_XDECREF(name);
    return NULL;
}

static char *
get_locale_encoding(void)
{
#ifdef MS_WINDOWS
    char codepage[100];
    PyOS_snprintf(codepage, sizeof(codepage), "cp%d", GetACP());
    return get_codec_name(codepage);
#elif defined(__ANDROID__) || defined(__COSMOPOLITAN__)
    return get_codec_name("UTF-8");
#elif defined(HAVE_LANGINFO_H) && defined(CODESET)
    char* codeset = nl_langinfo(CODESET);
    if (!codeset || codeset[0] == '\0') {
        PyErr_SetString(PyExc_ValueError, "CODESET is not set or empty");
        return NULL;
    }
    return get_codec_name(codeset);
#else
    PyErr_SetNone(PyExc_NotImplementedError);
    return NULL;
#endif
}

int
_Py_InitFsEncoding(PyInterpreterState *interp)
{
    PyObject *codec;

#ifdef MS_WINDOWS
    if (Py_LegacyWindowsFSEncodingFlag)
    {
        Py_FileSystemDefaultEncoding = "mbcs";
        Py_FileSystemDefaultEncodeErrors = "replace";
    }
    else
    {
        Py_FileSystemDefaultEncoding = "utf-8";
        Py_FileSystemDefaultEncodeErrors = "surrogatepass";
    }
#else
    if (Py_FileSystemDefaultEncoding == NULL)
    {
        Py_FileSystemDefaultEncoding = get_locale_encoding();
        if (Py_FileSystemDefaultEncoding == NULL)
            Py_FatalError("Py_Initialize: Unable to get the locale encoding");
        Py_HasFileSystemDefaultEncoding = 0;
        interp->fscodec_initialized = 1;
        return 0;
    }
#endif

    /* the encoding is mbcs, utf-8 or ascii */
    codec = _PyCodec_Lookup(Py_FileSystemDefaultEncoding);
    if (!codec) {
        /* Such error can only occurs in critical situations: no more
         * memory, import a module of the standard library failed,
         * etc. */
        return -1;
    }
    Py_DECREF(codec);
    interp->fscodec_initialized = 1;
    return 0;
}
