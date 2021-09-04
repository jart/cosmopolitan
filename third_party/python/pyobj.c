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
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/x/x.h"
#include "third_party/getopt/getopt.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/compile.h"
#include "third_party/python/Include/fileutils.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/marshal.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/pythonrun.h"
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

bool binonly;
int optimize;
char *pyfile;
char *outpath;
bool nocompress;
uint64_t image_base;
int strip_components;
const char *path_prefix;

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

static void
Yoink(struct ElfWriter *elf, const char *symbol)
{
    elfwriter_align(elf, 1, 0);
    elfwriter_startsection(elf, ".yoink", SHT_PROGBITS,
                           SHF_ALLOC | SHF_EXECINSTR);
    elfwriter_yoink(elf, symbol);
    elfwriter_finishsection(elf);
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

int
main(int argc, char *argv[])
{
    char t[12];
    bool ispkg;
    ssize_t rc;
    struct stat st;
    struct ElfWriter *elf;
    struct timespec timestamp;
    PyObject *code, *marshalled;
    size_t i, pysize, pycsize, marsize;
    char *s, *pydata, *pycdata, *mardata, *zipfile, *zipdir, *synfile, *modname;
    ShowCrashReports();
    GetOpts(argc, argv);
    marshalled = 0;
    if (stat(pyfile, &st) == -1) perror(pyfile), exit(1);
    CHECK_NOTNULL((pydata = gc(xslurp(pyfile, &pysize))));
    Py_NoUserSiteDirectory++;
    Py_NoSiteFlag++;
    Py_IgnoreEnvironmentFlag++;
    Py_FrozenFlag++;
    Py_SetProgramName(gc(utf8toutf32(argv[0], -1, 0)));
    _Py_InitializeEx_Private(1, 0);
    zipdir = gc(GetZipDir());
    zipfile = gc(GetZipFile());
    synfile = gc(GetSynFile());
    modname = gc(GetModName(&ispkg));
    code = Py_CompileStringExFlags(pydata, synfile, Py_file_input, NULL, optimize);
    if (!code) goto error;
    marshalled = PyMarshal_WriteObjectToString(code, Py_MARSHAL_VERSION);
    Py_CLEAR(code);
    if (!marshalled) goto error;
    memset(&timestamp, 0, sizeof(timestamp));
    assert(PyBytes_CheckExact(marshalled));
    mardata = PyBytes_AS_STRING(marshalled);
    marsize = PyBytes_GET_SIZE(marshalled);
    elf = elfwriter_open(outpath, 0644);
    elfwriter_cargoculting(elf);
    WRITE16LE(t+0, 3379); /* Python 3.6rc1 */
    WRITE16LE(t+2, READ16LE("\r\n"));
    WRITE32LE(t+4, timestamp.tv_sec);
    WRITE32LE(t+8, marsize);
    pycsize = sizeof(t) + marsize;
    pycdata = gc(malloc(pycsize));
    memcpy(pycdata, t, sizeof(t));
    memcpy(pycdata + sizeof(t), mardata, marsize);
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
    Yoink(elf, "__zip_start");
    elfwriter_close(elf);
    Py_CLEAR(marshalled);
    Py_Finalize();
    return 0;
error:
    PyErr_Print();
    Py_Finalize();
    if (marshalled) Py_DECREF(marshalled);
    return 1;
}
