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
#define PY_SSIZE_T_CLEAN
#include "dsp/scale/cdecimate2xuint8x8.h"
#include "libc/bits/popcnt.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/rdtscp.h"
#include "libc/runtime/runtime.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/methodobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/moduleobject.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/xed/x86.h"
/* clang-format off */

PYTHON_PROVIDE("cosmo");

PyDoc_STRVAR(cosmo_doc,
"Cosmopolitan Libc Module\n\
\n\
This module exposes low-level utilities from the Cosmopolitan library.\n\
\n\
Static objects:\n\
\n\
MODE -- make build mode, e.g. \"\", \"tiny\", \"opt\", \"rel\", etc.\n\
IMAGE_BASE_VIRTUAL -- base address of actually portable executable image\n\
kernel -- o/s platform, e.g. \"linux\", \"xnu\", \"metal\", \"nt\", etc.\n\
kStartTsc -- the rdtsc() value at process creation.");

PyDoc_STRVAR(syscount_doc,
"syscount($module)\n\
--\n\n\
Returns number of SYSCALL instructions issued to kernel by C library.\n\
\n\
Context switching from userspace to kernelspace is expensive! So it is\n\
useful to be able to know how many times that's happening in your app.\n\
\n\
This value currently isn't meaningful on Windows NT, where it currently\n\
tracks the number of POSIX calls that were attempted, but have not been\n\
polyfilled yet.");

static PyObject *
cosmo_syscount(PyObject *self, PyObject *noargs)
{
    return PyLong_FromSize_t(g_syscount);
}

PyDoc_STRVAR(rdtsc_doc,
"rdtsc($module)\n\
--\n\n\
Returns CPU timestamp counter.");

static PyObject *
cosmo_rdtsc(PyObject *self, PyObject *noargs)
{
    return PyLong_FromUnsignedLong(rdtsc());
}

PyDoc_STRVAR(getcpucore_doc,
"getcpucore($module)\n\
--\n\n\
Returns 0-indexed CPU core on which process is currently scheduled.");

static PyObject *
cosmo_getcpucore(PyObject *self, PyObject *noargs)
{
    return PyLong_FromUnsignedLong(TSC_AUX_CORE(rdpid()));
}

PyDoc_STRVAR(getcpunode_doc,
"getcpunode($module)\n\
--\n\n\
Returns 0-indexed NUMA node on which process is currently scheduled.");

static PyObject *
cosmo_getcpunode(PyObject *self, PyObject *noargs)
{
    return PyLong_FromUnsignedLong(TSC_AUX_NODE(rdpid()));
}

PyDoc_STRVAR(ftrace_doc,
"ftrace($module)\n\
--\n\n\
Enables logging of C function calls to stderr, e.g.\n\
\n\
    cosmo.ftrace()\n\
    WeirdFunction()\n\
    os._exit(1)\n\
\n\
Please be warned this prints massive amount of text. In order for it\n\
to work, the concomitant .com.dbg binary needs to be present.");

static PyObject *
cosmo_ftrace(PyObject *self, PyObject *noargs)
{
    ftrace_install();
    return Py_None;
}

PyDoc_STRVAR(crc32c_doc,
"crc32c($module, bytes, init=0)\n\
--\n\n\
Computes 32-bit Castagnoli Cyclic Redundancy Check.\n\
\n\
Used by ISCSI, TensorFlow, etc.\n\
Similar to zlib.crc32().");

static PyObject *
cosmo_crc32c(PyObject *self, PyObject *args)
{
    Py_ssize_t n;
    Py_buffer data;
    unsigned crc, init = 0;
    if (!PyArg_ParseTuple(args, "y*|I:crc32c", &data, &init)) return 0;
    crc = crc32c(init, data.buf, data.len);
    PyBuffer_Release(&data);
    return PyLong_FromUnsignedLong(crc);
}

PyDoc_STRVAR(decimate_doc,
"decimate($module, bytes)\n\
--\n\n\
Shrinks byte buffer in half using John Costella's magic kernel.\n\
\n\
This downscales data 2x using an eight-tap convolution, e.g.\n\
\n\
    >>> cosmo.decimate(b'\\xff\\xff\\x00\\x00\\xff\\xff\\x00\\x00\\xff\\xff\\x00\\x00')\n\
    b'\\xff\\x00\\xff\\x00\\xff\\x00'\n\
\n\
This is very fast if SSSE3 is available (Intel 2004+ / AMD 2011+).");

static PyObject *
cosmo_decimate(PyObject *self, PyObject *args)
{
    Py_ssize_t n;
    PyObject *buf;
    Py_buffer data;
    if (!PyArg_ParseTuple(args, "y*:decimate", &data)) return 0;
    if ((buf = PyBytes_FromStringAndSize(0, (n = ROUNDUP(data.len, 16))))) {
        memcpy(PyBytes_AS_STRING(buf), data.buf, data.len);
        memset(PyBytes_AS_STRING(buf) + data.len, 0, n - data.len);
        cDecimate2xUint8x8(n, (void *)PyBytes_AS_STRING(buf),
                           (signed char[8]){-1, -3, 3, 17, 17, 3, -3, -1});
        _PyBytes_Resize(&buf, (data.len + 1) >> 1);
    }
    PyBuffer_Release(&data);
    return buf;
}

PyDoc_STRVAR(ild_doc,
"ild($module, bytes)\n\
--\n\n\
Decodes byte-length of first machine instruction in byte sequence.\n\
\n\
This function makes it possible to tokenize raw x86 binary instructions.\n\
Return value is negative on error, where -1 is defined as buffer being\n\
too short, and lower numbers represent other errors.");

static PyObject *
cosmo_ild(PyObject *self, PyObject *args)
{
    Py_ssize_t n;
    const char *p;
    enum XedError e;
    struct XedDecodedInst xedd;
    if (!PyArg_ParseTuple(args, "y#:ild", &p, &n)) return 0;
    xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
    e = xed_instruction_length_decode(&xedd, p, n);
    return PyLong_FromUnsignedLong(e ? -e : xedd.length);
}

PyDoc_STRVAR(popcount_doc,
"popcount($module, bytes)\n\
--\n\n\
Returns population count of byte sequence, e.g.\n\
\n\
    >>> cosmo.popcount(b'\\xff\\x00\\xff')\n\
    16\n\
\n\
The population count is the number of bits that are set to one.\n\
It does the same thing as `Long.bit_count()` but for data buffers.\n\
This goes 30gbps on Nehalem (Intel 2008+) so it's quite fast.");

static PyObject *
cosmo_popcount(PyObject *self, PyObject *args)
{
    Py_ssize_t n;
    const char *p;
    if (!PyArg_ParseTuple(args, "y#:popcount", &p, &n)) return 0;
    return PyLong_FromSize_t(_countbits(p, n));
}

PyDoc_STRVAR(rgb2xterm256_doc,
"rgb2xterm256($module, r, g, b)\n\
--\n\n\
Quantizes RGB to color to XTERM256 ANSI terminal code.\n\
\n\
This helps you print colors in the terminal faster. For example:\n\
\n\
    print(\"\\x1b[38;5;%dmhello\\x1b[0m\" % (cosmo.rgb2xterm256(255,0,0)))\n\
\n\
Will print red text to the terminal.");

static PyObject *
cosmo_rgb2xterm256(PyObject *self, PyObject *args)
{
    unsigned char r, g, b;
    int res, cerr, gerr, ir, ig, ib, gray, grai, cr, cg, cb, gv;
    const unsigned char kXtermCube[6] = {0, 0137, 0207, 0257, 0327, 0377};
    if (!PyArg_ParseTuple(args, "BBB:rgb2xterm256", &r, &g, &b)) return 0;
    gray = round(r * .299 + g * .587 + b * .114);
    grai = gray > 238 ? 23 : (gray - 3) / 10;
    ir = r < 48 ? 0 : r < 115 ? 1 : (r - 35) / 40;
    ig = g < 48 ? 0 : g < 115 ? 1 : (g - 35) / 40;
    ib = b < 48 ? 0 : b < 115 ? 1 : (b - 35) / 40;
    cr = kXtermCube[ir];
    cg = kXtermCube[ig];
    cb = kXtermCube[ib];
    gv = 8 + 10 * grai;
    cerr = (cr-r)*(cr-r) + (cg-g)*(cg-g) + (cb-b)*(cb-b);
    gerr = (gv-r)*(gv-r) + (gv-g)*(gv-g) + (gv-b)*(gv-b);
    if (cerr <= gerr) {
        res = 16 + 36 * ir + 6 * ig + ib;
    } else {
        res = 232 + grai;
    }
    return PyLong_FromUnsignedLong(res);
}

static PyMethodDef cosmo_methods[] = {
    {"ild", cosmo_ild, METH_VARARGS, ild_doc},
    {"rdtsc", cosmo_rdtsc, METH_NOARGS, rdtsc_doc},
    {"crc32c", cosmo_crc32c, METH_VARARGS, crc32c_doc},
    {"syscount", cosmo_syscount, METH_NOARGS, syscount_doc},
    {"popcount", cosmo_popcount, METH_VARARGS, popcount_doc},
    {"decimate", cosmo_decimate, METH_VARARGS, decimate_doc},
    {"getcpucore", cosmo_getcpucore, METH_NOARGS, getcpucore_doc},
    {"getcpunode", cosmo_getcpunode, METH_NOARGS, getcpunode_doc},
    {"rgb2xterm256", cosmo_rgb2xterm256, METH_VARARGS, rgb2xterm256_doc},
#ifdef __PG__
    {"ftrace", cosmo_ftrace, METH_NOARGS, ftrace_doc},
#endif
    {0},
};

static struct PyModuleDef cosmomodule = {
    PyModuleDef_HEAD_INIT,
    "cosmo",
    cosmo_doc,
    -1,
    cosmo_methods,
};

static const char *
GetKernelName(void) {
    if (IsLinux()) {
        return "linux";
    } else if (IsXnu()) {
        return "xnu";
    } else if (IsMetal()) {
        return "metal";
    } else if (IsWindows()) {
        return "nt";
    } else if (IsFreebsd()) {
        return "freebsd";
    } else if (IsOpenbsd()) {
        return "openbsd";
    } else if (IsNetbsd()) {
        return "netbsd";
    } else {
        return "wut";
    }
}

PyMODINIT_FUNC
PyInit_cosmo(void)
{
    PyObject *m;
    if (!(m = PyModule_Create(&cosmomodule))) return 0;
    PyModule_AddStringConstant(m, "MODE", MODE);
    PyModule_AddIntConstant(m, "IMAGE_BASE_VIRTUAL", IMAGE_BASE_VIRTUAL);
    PyModule_AddStringConstant(m, "kernel", GetKernelName());
    PyModule_AddIntConstant(m, "kStartTsc", kStartTsc);
    return !PyErr_Occurred() ? m : 0;
}

_Section(".rodata.pytab.1") const struct _inittab _PyImport_Inittab_cosmo = {
    "cosmo",
    PyInit_cosmo,
};
