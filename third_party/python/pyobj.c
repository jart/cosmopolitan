/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/stat.h"
#include "libc/elf/def.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/compile.h"
#include "third_party/python/Include/fileutils.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/marshal.h"
#include "third_party/python/Include/opcode.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/tupleobject.h"
#include "third_party/python/Include/unicodeobject.h"
#include "tool/build/lib/elfwriter.h"
#include "tool/build/lib/stripcomponents.h"
/* clang-format off */

#define MANUAL "\
SYNOPSIS\n\
\n\
  pyobj.com [FLAGS] SOURCE\n\
\n\
OVERVIEW\n\
\n\
  Python Objectifier\n\
\n\
FLAGS\n\
\n\
  -o PATH      output elf object file\n\
  -P STR       prefix fake directory in zip\n\
  -C INT       strip directory components from src in zip\n\
  -O0          don't optimize [default]\n\
  -O1          remove debug statements\n\
  -O2          remove debug statements and docstrings\n\
  -b           binary only (don't include .py file)\n\
  -0           zip uncompressed\n\
  -n           do nothing\n\
  -h           help\n\
\n"

const char *const kIgnoredModules[] = /* sorted */ {

    "__main__", /* todo? */
    "_dummy_threading", /* evil code */
    "_overlapped", /* don't recognize if sys.platform yet */
    "_scproxy", /* don't recognize if sys.platform yet */
    "_thread",
    "_winapi", /* don't recognize if sys.platform yet */
    "asyncio.test_support", /* todo??? */
    "builtins",
    "concurrent.futures", /* asyncio's fault */
    "concurrent.futures._base",
    "concurrent.futures.process",
    "concurrent.futures.thread",
    "java", /* don't recognize if sys.platform yet */
    "java.lang", /* don't recognize if sys.platform yet */
    "msvcrt", /* don't recognize if sys.platform yet */
    "nt", /* os module don't care */
    "os.path", /* todo */
    "sys",
    "test.test_warnings.data", /* todo */

};

static bool binonly;
static int optimize;
static char *pyfile;
static char *outpath;
static struct stat st;
static PyObject *code;
static PyObject *marsh;
static bool nocompress;
static uint64_t image_base;
static int strip_components;
static struct ElfWriter *elf;
static const char *path_prefix;
static struct timespec timestamp;

static void
GetOpts(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "hn0Bb:O:o:C:P:")) != -1) {
        switch (opt) {
        case 'O':
            optimize = atoi(optarg);
            break;
        case 'o':
            outpath = optarg;
            break;
        case 'P':
            path_prefix = optarg;
            break;
        case 'C':
             strip_components = atoi(optarg);
            break;
        case 'b':
            image_base = strtoul(optarg, NULL, 0);
            break;
        case 'B':
            binonly = true;
            break;
        case '0':
            nocompress = true;
            break;
        case 'n':
            exit(0);
        case 'h':
            fputs(MANUAL, stdout);
            exit(0);
        default:
            fputs(MANUAL, stderr);
            exit(1);
        }
    }
    if (argc - optind != 1) {
        fputs("error: need one input file\n", stderr);
        exit(1);
    }
    pyfile = argv[optind];
    if (stat(pyfile, &st)) {
        perror(pyfile);
        exit(1);
    }
    if (!outpath) {
        outpath = xstrcat(pyfile, ".o");
    }
}

static char *
Dotify(char *s)
{
    size_t i;
    for (i = 0; s[i]; ++i) {
        if (s[i] == '/') {
            s[i] = '.';
        }
    }
    return s;
}

static char *
GetZipFile(void)
{
    const char *zipfile;
    zipfile = pyfile;
    zipfile = StripComponents(zipfile, strip_components);
    if (path_prefix) {
        zipfile = gc(xjoinpaths(path_prefix, zipfile));
    }
    return strdup(zipfile);
}

static char *
GetZipDir(void)
{
    return xstrcat(gc(xdirname(gc(GetZipFile()))), '/');
}

static char *
GetSynFile(void)
{
    return xstrcat("/zip/", gc(GetZipFile()));
}

static char *
GetModName(bool *ispkg)
{
    char *mod;
    mod = Dotify(xstripexts(StripComponents(pyfile, strip_components)));
    if ((*ispkg = endswith(mod, ".__init__"))) {
        mod[strlen(mod) - strlen(".__init__")] = 0;
    }
    return mod;
}

static char *
GetModSibling(const char *rel)
{
    char *p, *mod, *sib;
    assert(*rel);
    mod = Dotify(xstripexts(StripComponents(pyfile, strip_components)));
    if ((p = strrchr(mod, '.'))) {
        p[1] = 0;
        sib = xstrcat(mod, rel);
    } else {
        sib = strdup(rel);
    }
    free(mod);
    return sib;
}

static bool
IsIgnoredModule(const char *s)
{
    int m, l, r, x;
    l = 0;
    r = ARRAYLEN(kIgnoredModules) - 1;
    while (l <= r) {
        m = (l + r) >> 1;
        x = strcmp(s, kIgnoredModules[m]);
        if (x < 0) {
            r = m - 1;
        } else if (x > 0) {
            l = m + 1;
        } else {
            return true;
        }
    }
    return false;
}

static void
YoinkImports(void)
{
    size_t i, n;
    bool istry, isrel;
    char *p, *mod, *symbol;
    int x, y, op, arg, extra;
    PyObject *co_code, *co_names, *co_consts, *name, *cnst;
    co_code = PyObject_GetAttrString(code, "co_code");
    co_names = PyObject_GetAttrString(code, "co_names");
    co_consts = PyObject_GetAttrString(code, "co_consts");
    n = PyBytes_GET_SIZE(co_code);
    p = PyBytes_AS_STRING(co_code);
    for (istry = isrel = extra = i = 0; i + 2 <= n; i += 2) {
        x = p[i + 0] & 255;
        y = p[i + 1] & 255;
        op = x;
        arg = y | extra;
        extra = op == EXTENDED_ARG ? y << 8 : 0;
        switch (op) {
        case SETUP_EXCEPT:
            istry = true;
            break;
        case POP_BLOCK:
            istry = false;
            break;
        case LOAD_CONST:
            if (PyLong_Check((cnst = PyTuple_GetItem(co_consts, arg)))) {
                isrel = PyLong_AsLong(cnst) == 1;
            }
            break;
        case IMPORT_NAME:
            name = PyUnicode_AsUTF8String(PyTuple_GetItem(co_names, arg));
            if (*PyBytes_AS_STRING(name)) {  /* TODO: empty? */
                if (isrel) {
                    mod = GetModSibling(PyBytes_AS_STRING(name));
                } else {
                    mod = strdup(PyBytes_AS_STRING(name));
                }
                if (!IsIgnoredModule(mod)) {
                    symbol = xstrcat("pyc:", mod);
                    elfwriter_yoink(elf, symbol, istry ? STB_WEAK : STB_GLOBAL);
                    free(symbol);
                }
                free(mod);
            }
            Py_DECREF(name);
            break;
        default:
            break;
        }
    }
    Py_DECREF(co_consts);
    Py_DECREF(co_names);
    Py_DECREF(co_code);
}

static int
Objectify(void)
{
    bool ispkg;
    char header[12];
    size_t pysize, pycsize, marsize;
    char *pydata, *pycdata, *mardata, *zipfile, *zipdir, *synfile, *modname;
    ShowCrashReports();
    zipdir = gc(GetZipDir());
    zipfile = gc(GetZipFile());
    synfile = gc(GetSynFile());
    modname = gc(GetModName(&ispkg));
    CHECK_NOTNULL((pydata = gc(xslurp(pyfile, &pysize))));
    if ((!(code = Py_CompileStringExFlags(pydata, synfile, Py_file_input, 
                                          NULL, optimize)) ||
         !(marsh = PyMarshal_WriteObjectToString(code, Py_MARSHAL_VERSION)))) {
        PyErr_Print();
        return 1;
    }
    assert(PyBytes_CheckExact(marsh));
    mardata = PyBytes_AS_STRING(marsh);
    marsize = PyBytes_GET_SIZE(marsh);
    WRITE16LE(header+0, 3379); /* Python 3.6rc1 */
    WRITE16LE(header+2, READ16LE("\r\n"));
    WRITE32LE(header+4, timestamp.tv_sec);
    WRITE32LE(header+8, marsize);
    pycsize = sizeof(header) + marsize;
    pycdata = gc(malloc(pycsize));
    memcpy(pycdata, header, sizeof(header));
    memcpy(pycdata + sizeof(header), mardata, marsize);
    elf = elfwriter_open(outpath, 0644);
    elfwriter_cargoculting(elf);
    if (ispkg) {
        elfwriter_zip(elf, zipdir, zipdir, strlen(zipdir),
                      pydata, pysize, 040755, timestamp, timestamp,
                      timestamp, nocompress, image_base);
    }
    if (!binonly) {
        elfwriter_zip(elf, gc(xstrcat("py:", modname)), zipfile,
                      strlen(zipfile), pydata, pysize, st.st_mode, timestamp,
                      timestamp, timestamp, nocompress, image_base);
    }
    elfwriter_zip(elf, gc(xstrcat("pyc:", modname)), gc(xstrcat(zipfile, 'c')),
                  strlen(zipfile) + 1, pycdata, pycsize, st.st_mode, timestamp,
                  timestamp, timestamp, nocompress, image_base);
    elfwriter_align(elf, 1, 0);
    elfwriter_startsection(elf, ".yoink", SHT_PROGBITS,
                           SHF_ALLOC | SHF_EXECINSTR);
    YoinkImports();
    if (path_prefix && !strchr(modname, '.')) {
        elfwriter_yoink(elf, gc(xstrcat(path_prefix, "/")), STB_GLOBAL);
    }
    elfwriter_yoink(elf, "__zip_start", STB_GLOBAL);
    elfwriter_finishsection(elf);
    elfwriter_close(elf);
    return 0;
}

int
main(int argc, char *argv[])
{
    int rc;
    GetOpts(argc, argv);
    Py_NoUserSiteDirectory++;
    Py_NoSiteFlag++;
    Py_IgnoreEnvironmentFlag++;
    Py_FrozenFlag++;
    Py_SetProgramName(gc(utf8toutf32(argv[0], -1, 0)));
    _Py_InitializeEx_Private(1, 0);
    rc = Objectify();
    Py_XDECREF(code);
    Py_XDECREF(marsh);
    Py_Finalize();
    return rc;
}
