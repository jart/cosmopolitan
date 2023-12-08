/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/struct/termios.h"
#include "libc/calls/termios.h"
#include "libc/calls/ttydefaults.h"
#include "libc/calls/weirdtypes.h"
#include "libc/dce.h"
#include "libc/sysv/consts/baud.internal.h"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/modem.h"
#include "libc/sysv/consts/pty.h"
#include "libc/sysv/consts/termios.h"
#include "third_party/python/Include/bytesobject.h"
#include "third_party/python/Include/fileobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/listobject.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/yoink.h"

PYTHON_PROVIDE("termios");
PYTHON_PROVIDE("termios.B1000000");
PYTHON_PROVIDE("termios.B110");
PYTHON_PROVIDE("termios.B115200");
PYTHON_PROVIDE("termios.B1152000");
PYTHON_PROVIDE("termios.B1200");
PYTHON_PROVIDE("termios.B134");
PYTHON_PROVIDE("termios.B150");
PYTHON_PROVIDE("termios.B1500000");
PYTHON_PROVIDE("termios.B1800");
PYTHON_PROVIDE("termios.B19200");
PYTHON_PROVIDE("termios.B200");
PYTHON_PROVIDE("termios.B2000000");
PYTHON_PROVIDE("termios.B230400");
PYTHON_PROVIDE("termios.B2400");
PYTHON_PROVIDE("termios.B2500000");
PYTHON_PROVIDE("termios.B300");
PYTHON_PROVIDE("termios.B3000000");
PYTHON_PROVIDE("termios.B3500000");
PYTHON_PROVIDE("termios.B38400");
PYTHON_PROVIDE("termios.B4000000");
PYTHON_PROVIDE("termios.B4800");
PYTHON_PROVIDE("termios.B50");
PYTHON_PROVIDE("termios.B500000");
PYTHON_PROVIDE("termios.B57600");
PYTHON_PROVIDE("termios.B576000");
PYTHON_PROVIDE("termios.B600");
PYTHON_PROVIDE("termios.B75");
PYTHON_PROVIDE("termios.B9600");
PYTHON_PROVIDE("termios.BRKINT");
PYTHON_PROVIDE("termios.BS0");
PYTHON_PROVIDE("termios.BS1");
PYTHON_PROVIDE("termios.BSDLY");
PYTHON_PROVIDE("termios.CDSUSP");
PYTHON_PROVIDE("termios.CEOF");
PYTHON_PROVIDE("termios.CEOL");
PYTHON_PROVIDE("termios.CEOT");
PYTHON_PROVIDE("termios.CERASE");
PYTHON_PROVIDE("termios.CFLUSH");
PYTHON_PROVIDE("termios.CINTR");
PYTHON_PROVIDE("termios.CKILL");
PYTHON_PROVIDE("termios.CLNEXT");
PYTHON_PROVIDE("termios.CLOCAL");
PYTHON_PROVIDE("termios.CQUIT");
PYTHON_PROVIDE("termios.CR0");
PYTHON_PROVIDE("termios.CR1");
PYTHON_PROVIDE("termios.CR2");
PYTHON_PROVIDE("termios.CR3");
PYTHON_PROVIDE("termios.CRDLY");
PYTHON_PROVIDE("termios.CREAD");
PYTHON_PROVIDE("termios.CRPRNT");
PYTHON_PROVIDE("termios.CS5");
PYTHON_PROVIDE("termios.CS6");
PYTHON_PROVIDE("termios.CS7");
PYTHON_PROVIDE("termios.CS8");
PYTHON_PROVIDE("termios.CSIZE");
PYTHON_PROVIDE("termios.CSTART");
PYTHON_PROVIDE("termios.CSTOP");
PYTHON_PROVIDE("termios.CSTOPB");
PYTHON_PROVIDE("termios.CSUSP");
PYTHON_PROVIDE("termios.CWERASE");
PYTHON_PROVIDE("termios.ECHO");
PYTHON_PROVIDE("termios.ECHOCTL");
PYTHON_PROVIDE("termios.ECHOE");
PYTHON_PROVIDE("termios.ECHOK");
PYTHON_PROVIDE("termios.ECHOKE");
PYTHON_PROVIDE("termios.ECHONL");
PYTHON_PROVIDE("termios.ECHOPRT");
PYTHON_PROVIDE("termios.EXTA");
PYTHON_PROVIDE("termios.EXTB");
PYTHON_PROVIDE("termios.FF0");
PYTHON_PROVIDE("termios.FF1");
PYTHON_PROVIDE("termios.FFDLY");
PYTHON_PROVIDE("termios.FIOASYNC");
PYTHON_PROVIDE("termios.FIOCLEX");
PYTHON_PROVIDE("termios.FIONBIO");
PYTHON_PROVIDE("termios.FIONCLEX");
PYTHON_PROVIDE("termios.FIONREAD");
PYTHON_PROVIDE("termios.FLUSHO");
PYTHON_PROVIDE("termios.HUPCL");
PYTHON_PROVIDE("termios.ICANON");
PYTHON_PROVIDE("termios.ICRNL");
PYTHON_PROVIDE("termios.IEXTEN");
PYTHON_PROVIDE("termios.IGNBRK");
PYTHON_PROVIDE("termios.IGNCR");
PYTHON_PROVIDE("termios.IGNPAR");
PYTHON_PROVIDE("termios.IMAXBEL");
PYTHON_PROVIDE("termios.INLCR");
PYTHON_PROVIDE("termios.INPCK");
PYTHON_PROVIDE("termios.ISIG");
PYTHON_PROVIDE("termios.ISTRIP");
PYTHON_PROVIDE("termios.IUCLC");
PYTHON_PROVIDE("termios.IXANY");
PYTHON_PROVIDE("termios.IXOFF");
PYTHON_PROVIDE("termios.IXON");
PYTHON_PROVIDE("termios.NCCS");
PYTHON_PROVIDE("termios.NL0");
PYTHON_PROVIDE("termios.NL1");
PYTHON_PROVIDE("termios.NLDLY");
PYTHON_PROVIDE("termios.NOFLSH");
PYTHON_PROVIDE("termios.OCRNL");
PYTHON_PROVIDE("termios.OFDEL");
PYTHON_PROVIDE("termios.OFILL");
PYTHON_PROVIDE("termios.OLCUC");
PYTHON_PROVIDE("termios.ONLCR");
PYTHON_PROVIDE("termios.ONLRET");
PYTHON_PROVIDE("termios.ONOCR");
PYTHON_PROVIDE("termios.OPOST");
PYTHON_PROVIDE("termios.PARENB");
PYTHON_PROVIDE("termios.PARMRK");
PYTHON_PROVIDE("termios.PARODD");
PYTHON_PROVIDE("termios.PENDIN");
PYTHON_PROVIDE("termios.TAB0");
PYTHON_PROVIDE("termios.TAB1");
PYTHON_PROVIDE("termios.TAB2");
PYTHON_PROVIDE("termios.TAB3");
PYTHON_PROVIDE("termios.TABDLY");
PYTHON_PROVIDE("termios.TCIFLUSH");
PYTHON_PROVIDE("termios.TCIOFF");
PYTHON_PROVIDE("termios.TCIOFLUSH");
PYTHON_PROVIDE("termios.TCION");
PYTHON_PROVIDE("termios.TCOFLUSH");
PYTHON_PROVIDE("termios.TCOOFF");
PYTHON_PROVIDE("termios.TCOON");
PYTHON_PROVIDE("termios.TCSADRAIN");
PYTHON_PROVIDE("termios.TCSAFLUSH");
PYTHON_PROVIDE("termios.TCSANOW");
PYTHON_PROVIDE("termios.TCXONC");
PYTHON_PROVIDE("termios.TIOCCONS");
PYTHON_PROVIDE("termios.TIOCGETD");
PYTHON_PROVIDE("termios.TIOCMBIC");
PYTHON_PROVIDE("termios.TIOCMBIS");
PYTHON_PROVIDE("termios.TIOCMGET");
PYTHON_PROVIDE("termios.TIOCMSET");
PYTHON_PROVIDE("termios.TIOCM_CAR");
PYTHON_PROVIDE("termios.TIOCM_CD");
PYTHON_PROVIDE("termios.TIOCM_CTS");
PYTHON_PROVIDE("termios.TIOCM_DSR");
PYTHON_PROVIDE("termios.TIOCM_DTR");
PYTHON_PROVIDE("termios.TIOCM_LE");
PYTHON_PROVIDE("termios.TIOCM_RI");
PYTHON_PROVIDE("termios.TIOCM_RNG");
PYTHON_PROVIDE("termios.TIOCM_RTS");
PYTHON_PROVIDE("termios.TIOCM_SR");
PYTHON_PROVIDE("termios.TIOCM_ST");
PYTHON_PROVIDE("termios.TIOCNOTTY");
PYTHON_PROVIDE("termios.TIOCNXCL");
PYTHON_PROVIDE("termios.TIOCOUTQ");
PYTHON_PROVIDE("termios.TIOCPKT");
PYTHON_PROVIDE("termios.TIOCSCTTY");
PYTHON_PROVIDE("termios.TIOCSETD");
PYTHON_PROVIDE("termios.TIOCSPGRP");
PYTHON_PROVIDE("termios.TIOCSTI");
PYTHON_PROVIDE("termios.TIOCSWINSZ");
PYTHON_PROVIDE("termios.TOSTOP");
PYTHON_PROVIDE("termios.VDISCARD");
PYTHON_PROVIDE("termios.VEOF");
PYTHON_PROVIDE("termios.VEOL");
PYTHON_PROVIDE("termios.VEOL2");
PYTHON_PROVIDE("termios.VERASE");
PYTHON_PROVIDE("termios.VINTR");
PYTHON_PROVIDE("termios.VKILL");
PYTHON_PROVIDE("termios.VLNEXT");
PYTHON_PROVIDE("termios.VMIN");
PYTHON_PROVIDE("termios.VQUIT");
PYTHON_PROVIDE("termios.VREPRINT");
PYTHON_PROVIDE("termios.VSTART");
PYTHON_PROVIDE("termios.VSTOP");
PYTHON_PROVIDE("termios.VSUSP");
PYTHON_PROVIDE("termios.VSWTC");
PYTHON_PROVIDE("termios.VT0");
PYTHON_PROVIDE("termios.VT1");
PYTHON_PROVIDE("termios.VTDLY");
PYTHON_PROVIDE("termios.VTIME");
PYTHON_PROVIDE("termios.VWERASE");
PYTHON_PROVIDE("termios.XCASE");
PYTHON_PROVIDE("termios.XTABS");
PYTHON_PROVIDE("termios.error");
PYTHON_PROVIDE("termios.tcdrain");
PYTHON_PROVIDE("termios.tcflow");
PYTHON_PROVIDE("termios.tcflush");
PYTHON_PROVIDE("termios.tcgetattr");
PYTHON_PROVIDE("termios.tcsendbreak");
PYTHON_PROVIDE("termios.tcsetattr");

/* termiosmodule.c -- POSIX terminal I/O module implementation.  */

PyDoc_STRVAR(termios__doc__,
"This module provides an interface to the Posix calls for tty I/O control.\n\
For a complete description of these calls, see the Posix or Unix manual\n\
pages. It is only available for those Unix versions that support Posix\n\
termios style tty I/O control.\n\
\n\
All functions in this module take a file descriptor fd as their first\n\
argument. This can be an integer file descriptor, such as returned by\n\
sys.stdin.fileno(), or a file object, such as sys.stdin itself.");

static PyObject *TermiosError;

static int fdconv(PyObject* obj, void* p)
{
    int fd;

    fd = PyObject_AsFileDescriptor(obj);
    if (fd >= 0) {
        *(int*)p = fd;
        return 1;
    }
    return 0;
}

PyDoc_STRVAR(termios_tcgetattr__doc__,
"tcgetattr(fd) -> list_of_attrs\n\
\n\
Get the tty attributes for file descriptor fd, as follows:\n\
[iflag, oflag, cflag, lflag, ispeed, ospeed, cc] where cc is a list\n\
of the tty special characters (each a string of length 1, except the items\n\
with indices VMIN and VTIME, which are integers when these fields are\n\
defined).  The interpretation of the flags and the speeds as well as the\n\
indexing in the cc array must be done using the symbolic constants defined\n\
in this module.");

static PyObject *
termios_tcgetattr(PyObject *self, PyObject *args)
{
    int fd;
    struct termios mode;
    PyObject *cc;
    speed_t ispeed, ospeed;
    PyObject *v;
    int i;
    char ch;

    if (!PyArg_ParseTuple(args, "O&:tcgetattr",
                          fdconv, (void*)&fd))
        return NULL;

    if (tcgetattr(fd, &mode) == -1)
        return PyErr_SetFromErrno(TermiosError);

    ispeed = cfgetispeed(&mode);
    ospeed = cfgetospeed(&mode);

    cc = PyList_New(NCCS);
    if (cc == NULL)
        return NULL;
    for (i = 0; i < NCCS; i++) {
        ch = (char)mode.c_cc[i];
        v = PyBytes_FromStringAndSize(&ch, 1);
        if (v == NULL)
            goto err;
        PyList_SetItem(cc, i, v);
    }

    /* Convert the MIN and TIME slots to integer.  On some systems, the
       MIN and TIME slots are the same as the EOF and EOL slots.  So we
       only do this in noncanonical input mode.  */
    if ((mode.c_lflag & ICANON) == 0) {
        v = PyLong_FromLong((long)mode.c_cc[VMIN]);
        if (v == NULL)
            goto err;
        PyList_SetItem(cc, VMIN, v);
        v = PyLong_FromLong((long)mode.c_cc[VTIME]);
        if (v == NULL)
            goto err;
        PyList_SetItem(cc, VTIME, v);
    }

    if (!(v = PyList_New(7)))
        goto err;

    PyList_SetItem(v, 0, PyLong_FromLong((long)mode.c_iflag));
    PyList_SetItem(v, 1, PyLong_FromLong((long)mode.c_oflag));
    PyList_SetItem(v, 2, PyLong_FromLong((long)mode.c_cflag));
    PyList_SetItem(v, 3, PyLong_FromLong((long)mode.c_lflag));
    PyList_SetItem(v, 4, PyLong_FromLong((long)ispeed));
    PyList_SetItem(v, 5, PyLong_FromLong((long)ospeed));
    if (PyErr_Occurred()) {
        Py_DECREF(v);
        goto err;
    }
    PyList_SetItem(v, 6, cc);
    return v;
  err:
    Py_DECREF(cc);
    return NULL;
}

PyDoc_STRVAR(termios_tcsetattr__doc__,
"tcsetattr(fd, when, attributes) -> None\n\
\n\
Set the tty attributes for file descriptor fd.\n\
The attributes to be set are taken from the attributes argument, which\n\
is a list like the one returned by tcgetattr(). The when argument\n\
determines when the attributes are changed: termios.TCSANOW to\n\
change immediately, termios.TCSADRAIN to change after transmitting all\n\
queued output, or termios.TCSAFLUSH to change after transmitting all\n\
queued output and discarding all queued input. ");

static PyObject *
termios_tcsetattr(PyObject *self, PyObject *args)
{
    int fd, when;
    struct termios mode;
    speed_t ispeed, ospeed;
    PyObject *term, *cc, *v;
    int i;

    if (!PyArg_ParseTuple(args, "O&iO:tcsetattr",
                          fdconv, &fd, &when, &term))
        return NULL;
    if (!PyList_Check(term) || PyList_Size(term) != 7) {
        PyErr_SetString(PyExc_TypeError,
                     "tcsetattr, arg 3: must be 7 element list");
        return NULL;
    }

    /* Get the old mode, in case there are any hidden fields... */
    if (tcgetattr(fd, &mode) == -1)
        return PyErr_SetFromErrno(TermiosError);
    mode.c_iflag = (tcflag_t) PyLong_AsLong(PyList_GetItem(term, 0));
    mode.c_oflag = (tcflag_t) PyLong_AsLong(PyList_GetItem(term, 1));
    mode.c_cflag = (tcflag_t) PyLong_AsLong(PyList_GetItem(term, 2));
    mode.c_lflag = (tcflag_t) PyLong_AsLong(PyList_GetItem(term, 3));
    ispeed = (speed_t) PyLong_AsLong(PyList_GetItem(term, 4));
    ospeed = (speed_t) PyLong_AsLong(PyList_GetItem(term, 5));
    cc = PyList_GetItem(term, 6);
    if (PyErr_Occurred())
        return NULL;

    if (!PyList_Check(cc) || PyList_Size(cc) != NCCS) {
        PyErr_Format(PyExc_TypeError,
            "tcsetattr: attributes[6] must be %d element list",
                 NCCS);
        return NULL;
    }

    for (i = 0; i < NCCS; i++) {
        v = PyList_GetItem(cc, i);

        if (PyBytes_Check(v) && PyBytes_Size(v) == 1)
            mode.c_cc[i] = (cc_t) * PyBytes_AsString(v);
        else if (PyLong_Check(v))
            mode.c_cc[i] = (cc_t) PyLong_AsLong(v);
        else {
            PyErr_SetString(PyExc_TypeError,
     "tcsetattr: elements of attributes must be characters or integers");
                        return NULL;
                }
    }

    if (cfsetispeed(&mode, (speed_t) ispeed) == -1)
        return PyErr_SetFromErrno(TermiosError);
    if (cfsetospeed(&mode, (speed_t) ospeed) == -1)
        return PyErr_SetFromErrno(TermiosError);
    if (tcsetattr(fd, when, &mode) == -1)
        return PyErr_SetFromErrno(TermiosError);

    Py_INCREF(Py_None);
    return Py_None;
}

PyDoc_STRVAR(termios_tcsendbreak__doc__,
"tcsendbreak(fd, duration) -> None\n\
\n\
Send a break on file descriptor fd.\n\
A zero duration sends a break for 0.25-0.5 seconds; a nonzero duration\n\
has a system dependent meaning.");

static PyObject *
termios_tcsendbreak(PyObject *self, PyObject *args)
{
    int fd, duration;

    if (!PyArg_ParseTuple(args, "O&i:tcsendbreak",
                          fdconv, &fd, &duration))
        return NULL;
    if (tcsendbreak(fd, duration) == -1)
        return PyErr_SetFromErrno(TermiosError);

    Py_INCREF(Py_None);
    return Py_None;
}

PyDoc_STRVAR(termios_tcdrain__doc__,
"tcdrain(fd) -> None\n\
\n\
Wait until all output written to file descriptor fd has been transmitted.");

static PyObject *
termios_tcdrain(PyObject *self, PyObject *args)
{
    int fd;

    if (!PyArg_ParseTuple(args, "O&:tcdrain",
                          fdconv, &fd))
        return NULL;
    if (tcdrain(fd) == -1)
        return PyErr_SetFromErrno(TermiosError);

    Py_INCREF(Py_None);
    return Py_None;
}

PyDoc_STRVAR(termios_tcflush__doc__,
"tcflush(fd, queue) -> None\n\
\n\
Discard queued data on file descriptor fd.\n\
The queue selector specifies which queue: termios.TCIFLUSH for the input\n\
queue, termios.TCOFLUSH for the output queue, or termios.TCIOFLUSH for\n\
both queues. ");

static PyObject *
termios_tcflush(PyObject *self, PyObject *args)
{
    int fd, queue;

    if (!PyArg_ParseTuple(args, "O&i:tcflush",
                          fdconv, &fd, &queue))
        return NULL;
    if (tcflush(fd, queue) == -1)
        return PyErr_SetFromErrno(TermiosError);

    Py_INCREF(Py_None);
    return Py_None;
}

PyDoc_STRVAR(termios_tcflow__doc__,
"tcflow(fd, action) -> None\n\
\n\
Suspend or resume input or output on file descriptor fd.\n\
The action argument can be termios.TCOOFF to suspend output,\n\
termios.TCOON to restart output, termios.TCIOFF to suspend input,\n\
or termios.TCION to restart input.");

static PyObject *
termios_tcflow(PyObject *self, PyObject *args)
{
    int fd, action;

    if (!PyArg_ParseTuple(args, "O&i:tcflow",
                          fdconv, &fd, &action))
        return NULL;
    if (tcflow(fd, action) == -1)
        return PyErr_SetFromErrno(TermiosError);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef termios_methods[] =
{
    {"tcgetattr", termios_tcgetattr,
     METH_VARARGS, termios_tcgetattr__doc__},
    {"tcsetattr", termios_tcsetattr,
     METH_VARARGS, termios_tcsetattr__doc__},
    {"tcsendbreak", termios_tcsendbreak,
     METH_VARARGS, termios_tcsendbreak__doc__},
    {"tcdrain", termios_tcdrain,
     METH_VARARGS, termios_tcdrain__doc__},
    {"tcflush", termios_tcflush,
     METH_VARARGS, termios_tcflush__doc__},
    {"tcflow", termios_tcflow,
     METH_VARARGS, termios_tcflow__doc__},
    {NULL, NULL}
};


#if defined(VSWTCH) && !defined(VSWTC)
#define VSWTC VSWTCH
#endif

#if defined(VSWTC) && !defined(VSWTCH)
#define VSWTCH VSWTC
#endif


static struct PyModuleDef termiosmodule = {
    PyModuleDef_HEAD_INIT,
    "termios",
    termios__doc__,
    -1,
    termios_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_termios(void)
{
    PyObject *m;

    m = PyModule_Create(&termiosmodule);
    if (m == NULL)
        return NULL;

    if (TermiosError == NULL) {
        TermiosError = PyErr_NewException("termios.error", NULL, NULL);
    }
    Py_INCREF(TermiosError);
    PyModule_AddObject(m, "error", TermiosError);

    if (B50) PyModule_AddIntConstant(m, "B50", B50);
    if (B75) PyModule_AddIntConstant(m, "B75", B75);
    if (B110) PyModule_AddIntConstant(m, "B110", B110);
    if (B134) PyModule_AddIntConstant(m, "B134", B134);
    if (B150) PyModule_AddIntConstant(m, "B150", B150);
    if (B200) PyModule_AddIntConstant(m, "B200", B200);
    if (B300) PyModule_AddIntConstant(m, "B300", B300);
    if (B600) PyModule_AddIntConstant(m, "B600", B600);
    if (B1200) PyModule_AddIntConstant(m, "B1200", B1200);
    if (B1800) PyModule_AddIntConstant(m, "B1800", B1800);
    if (B2400) PyModule_AddIntConstant(m, "B2400", B2400);
    if (B4800) PyModule_AddIntConstant(m, "B4800", B4800);
    if (B9600) PyModule_AddIntConstant(m, "B9600", B9600);
    if (B19200) PyModule_AddIntConstant(m, "B19200", B19200);
    if (B38400) PyModule_AddIntConstant(m, "B38400", B38400);
    if (B57600) PyModule_AddIntConstant(m, "B57600", B57600);
    if (B115200) PyModule_AddIntConstant(m, "B115200", B115200);
    if (B230400) PyModule_AddIntConstant(m, "B230400", B230400);
    if (B500000) PyModule_AddIntConstant(m, "B500000", B500000);
    if (B576000) PyModule_AddIntConstant(m, "B576000", B576000);
    if (B1000000) PyModule_AddIntConstant(m, "B1000000", B1000000);
    if (B1152000) PyModule_AddIntConstant(m, "B1152000", B1152000);
    if (B1500000) PyModule_AddIntConstant(m, "B1500000", B1500000);
    if (B2000000) PyModule_AddIntConstant(m, "B2000000", B2000000);
    if (B2500000) PyModule_AddIntConstant(m, "B2500000", B2500000);
    if (B3000000) PyModule_AddIntConstant(m, "B3000000", B3000000);
    if (B3500000) PyModule_AddIntConstant(m, "B3500000", B3500000);
    if (B4000000) PyModule_AddIntConstant(m, "B4000000", B4000000);
    /* TODO(jart): B460800 */
    /* TODO(jart): B921600 */
    /* TODO(jart): B460800 */

    PyModule_AddIntConstant(m, "TCSANOW", TCSANOW);
    PyModule_AddIntConstant(m, "TCSADRAIN", TCSADRAIN);
    PyModule_AddIntConstant(m, "TCSAFLUSH", TCSAFLUSH);
    /* TODO(jart): TCSASOFT */
    PyModule_AddIntConstant(m, "TCIFLUSH", TCIFLUSH);
    PyModule_AddIntConstant(m, "TCOFLUSH", TCOFLUSH);
    PyModule_AddIntConstant(m, "TCIOFLUSH", TCIOFLUSH);
    PyModule_AddIntConstant(m, "TCOOFF", TCOOFF);
    PyModule_AddIntConstant(m, "TCOON", TCOON);
    PyModule_AddIntConstant(m, "TCIOFF", TCIOFF);
    PyModule_AddIntConstant(m, "TCION", TCION);
    if (IGNBRK) PyModule_AddIntConstant(m, "IGNBRK", IGNBRK);
    if (BRKINT) PyModule_AddIntConstant(m, "BRKINT", BRKINT);
    if (IGNPAR) PyModule_AddIntConstant(m, "IGNPAR", IGNPAR);
    if (PARMRK) PyModule_AddIntConstant(m, "PARMRK", PARMRK);
    if (INPCK) PyModule_AddIntConstant(m, "INPCK", INPCK);
    if (ISTRIP) PyModule_AddIntConstant(m, "ISTRIP", ISTRIP);
    if (INLCR) PyModule_AddIntConstant(m, "INLCR", INLCR);
    if (IGNCR) PyModule_AddIntConstant(m, "IGNCR", IGNCR);
    if (ICRNL) PyModule_AddIntConstant(m, "ICRNL", ICRNL);
    if (IUCLC) PyModule_AddIntConstant(m, "IUCLC", IUCLC);
    if (IXON) PyModule_AddIntConstant(m, "IXON", IXON);
    if (IXANY) PyModule_AddIntConstant(m, "IXANY", IXANY);
    if (IXOFF) PyModule_AddIntConstant(m, "IXOFF", IXOFF);
    if (IMAXBEL) PyModule_AddIntConstant(m, "IMAXBEL", IMAXBEL);
    PyModule_AddIntConstant(m, "OPOST", OPOST);
    if (OLCUC) PyModule_AddIntConstant(m, "OLCUC", OLCUC);
    if (ONLCR) PyModule_AddIntConstant(m, "ONLCR", ONLCR);
    if (OCRNL) PyModule_AddIntConstant(m, "OCRNL", OCRNL);
    if (ONOCR) PyModule_AddIntConstant(m, "ONOCR", ONOCR);
    if (ONLRET) PyModule_AddIntConstant(m, "ONLRET", ONLRET);
    if (OFILL) PyModule_AddIntConstant(m, "OFILL", OFILL);
    if (OFDEL) PyModule_AddIntConstant(m, "OFDEL", OFDEL);
    if (NLDLY) PyModule_AddIntConstant(m, "NLDLY", NLDLY);
    if (CRDLY) PyModule_AddIntConstant(m, "CRDLY", CRDLY);
    if (TABDLY) PyModule_AddIntConstant(m, "TABDLY", TABDLY);
    if (BSDLY) PyModule_AddIntConstant(m, "BSDLY", BSDLY);
    if (VTDLY) PyModule_AddIntConstant(m, "VTDLY", VTDLY);
    if (FFDLY) PyModule_AddIntConstant(m, "FFDLY", FFDLY);
    PyModule_AddIntConstant(m, "NL0", NL0);
    if (NL1) PyModule_AddIntConstant(m, "NL1", NL1);
    PyModule_AddIntConstant(m, "CR0", CR0);
    if (CR1) PyModule_AddIntConstant(m, "CR1", CR1);
    if (CR2) PyModule_AddIntConstant(m, "CR2", CR2);
    if (CR3) PyModule_AddIntConstant(m, "CR3", CR3);
    PyModule_AddIntConstant(m, "TAB0", TAB0);
    if (TAB1) PyModule_AddIntConstant(m, "TAB1", TAB1);
    if (TAB2) PyModule_AddIntConstant(m, "TAB2", TAB2);
    if (TAB3) PyModule_AddIntConstant(m, "TAB3", TAB3);
    if (XTABS) PyModule_AddIntConstant(m, "XTABS", XTABS);
    PyModule_AddIntConstant(m, "BS0", BS0);
    if (BS1) PyModule_AddIntConstant(m, "BS1", BS1);
    PyModule_AddIntConstant(m, "VT0", VT0);
    if (VT1) PyModule_AddIntConstant(m, "VT1", VT1);
    PyModule_AddIntConstant(m, "FF0", FF0);
    if (FF1) PyModule_AddIntConstant(m, "FF1", FF1);
    if (CSIZE) PyModule_AddIntConstant(m, "CSIZE", CSIZE);
    if (CSTOPB) PyModule_AddIntConstant(m, "CSTOPB", CSTOPB);
    if (CREAD) PyModule_AddIntConstant(m, "CREAD", CREAD);
    if (PARENB) PyModule_AddIntConstant(m, "PARENB", PARENB);
    if (PARODD) PyModule_AddIntConstant(m, "PARODD", PARODD);
    if (HUPCL) PyModule_AddIntConstant(m, "HUPCL", HUPCL);
    if (CLOCAL) PyModule_AddIntConstant(m, "CLOCAL", CLOCAL);
    PyModule_AddIntConstant(m, "CS5", CS5);
    if (CS6) PyModule_AddIntConstant(m, "CS6", CS6);
    if (CS7) PyModule_AddIntConstant(m, "CS7", CS7);
    if (CS8) PyModule_AddIntConstant(m, "CS8", CS8);
    PyModule_AddIntConstant(m, "ISIG", ISIG);
    PyModule_AddIntConstant(m, "ICANON", ICANON);
    if (XCASE) PyModule_AddIntConstant(m, "XCASE", XCASE);
    PyModule_AddIntConstant(m, "ECHO", ECHO);
    PyModule_AddIntConstant(m, "ECHOE", ECHOE);
    PyModule_AddIntConstant(m, "ECHOK", ECHOK);
    PyModule_AddIntConstant(m, "ECHONL", ECHONL);
    PyModule_AddIntConstant(m, "ECHOCTL", ECHOCTL);
    PyModule_AddIntConstant(m, "ECHOPRT", ECHOPRT);
    PyModule_AddIntConstant(m, "ECHOKE", ECHOKE);
    PyModule_AddIntConstant(m, "FLUSHO", FLUSHO);
    PyModule_AddIntConstant(m, "NOFLSH", NOFLSH);
    PyModule_AddIntConstant(m, "TOSTOP", TOSTOP);
    PyModule_AddIntConstant(m, "PENDIN", PENDIN);
    PyModule_AddIntConstant(m, "IEXTEN", IEXTEN);

    /* TODO(jart): CRTSCTS */

    /* termios.c_cc[𝑖] */
    PyModule_AddIntConstant(m, "VINTR", VINTR);
    PyModule_AddIntConstant(m, "VQUIT", VQUIT);
    PyModule_AddIntConstant(m, "VERASE", VERASE);
    PyModule_AddIntConstant(m, "VKILL", VKILL);
    PyModule_AddIntConstant(m, "VEOF", VEOF);
    PyModule_AddIntConstant(m, "VTIME", VTIME);
    PyModule_AddIntConstant(m, "VMIN", VMIN);
    if (VSWTC) PyModule_AddIntConstant(m, "VSWTC", VSWTC);
    PyModule_AddIntConstant(m, "VSTART", VSTART);
    PyModule_AddIntConstant(m, "VSTOP", VSTOP);
    PyModule_AddIntConstant(m, "VSUSP", VSUSP);
    PyModule_AddIntConstant(m, "VEOL", VEOL);
    PyModule_AddIntConstant(m, "VREPRINT", VREPRINT);
    PyModule_AddIntConstant(m, "VDISCARD", VDISCARD);
    PyModule_AddIntConstant(m, "VWERASE", VWERASE);
    PyModule_AddIntConstant(m, "VLNEXT", VLNEXT);
    PyModule_AddIntConstant(m, "VEOL2", VEOL2);

    /* <sys/ttydefaults.h> */
    PyModule_AddIntConstant(m, "CEOF", CEOF);
    PyModule_AddIntConstant(m, "CDSUSP", CDSUSP);
    PyModule_AddIntConstant(m, "CEOL", CEOL);
    PyModule_AddIntConstant(m, "CFLUSH", CFLUSH);
    PyModule_AddIntConstant(m, "CINTR", CINTR);
    PyModule_AddIntConstant(m, "CKILL", CKILL);
    PyModule_AddIntConstant(m, "CLNEXT", CLNEXT);
    PyModule_AddIntConstant(m, "CEOT", CEOT);
    PyModule_AddIntConstant(m, "CERASE", CERASE);
    PyModule_AddIntConstant(m, "CQUIT", CQUIT);
    PyModule_AddIntConstant(m, "CRPRNT", CRPRNT);
    PyModule_AddIntConstant(m, "CSTART", CSTART);
    PyModule_AddIntConstant(m, "CSTOP", CSTOP);
    PyModule_AddIntConstant(m, "CSUSP", CSUSP);
    PyModule_AddIntConstant(m, "CWERASE", CWERASE);

    /* ioctl */
#ifdef FIOCLEX
    PyModule_AddIntConstant(m, "FIOCLEX", FIOCLEX);
#endif
#ifdef FIONCLEX
    PyModule_AddIntConstant(m, "FIONCLEX", FIONCLEX);
#endif
#ifdef FIONBIO
    PyModule_AddIntConstant(m, "FIONBIO", FIONBIO);
#endif
#ifdef FIONREAD
    PyModule_AddIntConstant(m, "FIONREAD", FIONREAD);
#endif
#ifdef FIOASYNC
    PyModule_AddIntConstant(m, "FIOASYNC", FIOASYNC);
#endif
    if (EXTA) PyModule_AddIntConstant(m, "EXTA", EXTA);
    if (EXTB) PyModule_AddIntConstant(m, "EXTB", EXTB);

    /* TODO(jart): IBSHIFT */
    /* TODO(jart): CC */
    /* TODO(jart): MASK */
    /* TODO(jart): SHIFT */
    /* TODO(jart): NCC */
    if (NCCS) PyModule_AddIntConstant(m, "NCCS", NCCS);
    /* TODO(jart): MOUSE */
    /* TODO(jart): PPP */
    /* TODO(jart): SLIP */
    /* TODO(jart): STRIP */
    /* TODO(jart): TTY */
    /* TODO(jart): TCGETA */
    /* TODO(jart): TCSBRKP */
    /* TODO(jart): TCSETA */
    /* TODO(jart): TCSETAF */
    /* TODO(jart): TCSETAW */
    if (TIOCCONS) PyModule_AddIntConstant(m, "TIOCCONS", TIOCCONS);
    /* TODO(jart): TIOCEXCL */
    if (TIOCGETD) PyModule_AddIntConstant(m, "TIOCGETD", TIOCGETD);
    /* TODO(jart): TIOCGICOUNT */
    /* TODO(jart): TIOCGLCKTRMIOS */
    /* TODO(jart): TIOCGSERIAL */
    /* TODO(jart): TIOCGSOFTCAR */
    /* TODO(jart): TIOCINQ */
    /* TODO(jart): TIOCLINUX */
    if (TIOCMBIC) PyModule_AddIntConstant(m, "TIOCMBIC", TIOCMBIC);
    if (TIOCMBIS) PyModule_AddIntConstant(m, "TIOCMBIS", TIOCMBIS);
    if (TIOCMGET) PyModule_AddIntConstant(m, "TIOCMGET", TIOCMGET);
    /* TODO(jart): TIOCMIWAIT */
    if (TIOCMSET) PyModule_AddIntConstant(m, "TIOCMSET", TIOCMSET);
    if (TIOCM_CAR) PyModule_AddIntConstant(m, "TIOCM_CAR", TIOCM_CAR);
    if (TIOCM_CD) PyModule_AddIntConstant(m, "TIOCM_CD", TIOCM_CD);
    if (TIOCM_CTS) PyModule_AddIntConstant(m, "TIOCM_CTS", TIOCM_CTS);
    if (TIOCM_DSR) PyModule_AddIntConstant(m, "TIOCM_DSR", TIOCM_DSR);
    if (TIOCM_DTR) PyModule_AddIntConstant(m, "TIOCM_DTR", TIOCM_DTR);
    if (TIOCM_LE) PyModule_AddIntConstant(m, "TIOCM_LE", TIOCM_LE);
    if (TIOCM_RI) PyModule_AddIntConstant(m, "TIOCM_RI", TIOCM_RI);
    if (TIOCM_RNG) PyModule_AddIntConstant(m, "TIOCM_RNG", TIOCM_RNG);
    if (TIOCM_RTS) PyModule_AddIntConstant(m, "TIOCM_RTS", TIOCM_RTS);
    if (TIOCM_SR) PyModule_AddIntConstant(m, "TIOCM_SR", TIOCM_SR);
    if (TIOCM_ST) PyModule_AddIntConstant(m, "TIOCM_ST", TIOCM_ST);
    if (TIOCNOTTY) PyModule_AddIntConstant(m, "TIOCNOTTY", TIOCNOTTY);
    if (TIOCNXCL) PyModule_AddIntConstant(m, "TIOCNXCL", TIOCNXCL);
    if (TIOCOUTQ) PyModule_AddIntConstant(m, "TIOCOUTQ", TIOCOUTQ);
    if (TIOCPKT != -1) PyModule_AddIntConstant(m, "TIOCPKT", TIOCPKT);
    /* TODO(jart): DATA */
    /* TODO(jart): DOSTOP */
    /* TODO(jart): FLUSHREAD */
    /* TODO(jart): FLUSHWRITE */
    /* TODO(jart): NOSTOP */
    /* TODO(jart): START */
    /* TODO(jart): STOP */
#ifdef TIOCSCTTY
    if (TIOCSCTTY) PyModule_AddIntConstant(m, "TIOCSCTTY", TIOCSCTTY);
#endif
    /* TODO(jart): TIOCSERCONFIG */
    /* TODO(jart): TIOCSERGSTRUCT */
    /* TODO(jart): TIOCSERGWILD */
    /* TODO(jart): TIOCSERSWILD */
#ifdef TIOCSETD
    if (TIOCSETD) PyModule_AddIntConstant(m, "TIOCSETD", TIOCSETD);
#endif
    /* TODO(jart): TIOCSLCKTRMIOS */
#ifdef TIOCSPGRP
    if (TIOCSPGRP) PyModule_AddIntConstant(m, "TIOCSPGRP", TIOCSPGRP);
#endif
    /* TODO(jart): TIOCSSERIAL */
    /* TODO(jart): TIOCSSOFTCAR */
#ifdef TIOCSTI
    if (TIOCSTI) PyModule_AddIntConstant(m, "TIOCSTI", TIOCSTI);
#endif
    /* TODO(jart): TIOCTTYGSTRUCT */

    return m;
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab_termios = {
    "termios",
    PyInit_termios,
};
