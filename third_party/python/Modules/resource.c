/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/rusage.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/floatobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/longobject.h"
#include "third_party/python/Include/modsupport.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pymacro.h"
#include "third_party/python/Include/structseq.h"
#include "third_party/python/Include/yoink.h"
#include "third_party/python/pyconfig.h"

PYTHON_PROVIDE("resource");
PYTHON_PROVIDE("resource.RLIMIT_AS");
PYTHON_PROVIDE("resource.RLIMIT_CORE");
PYTHON_PROVIDE("resource.RLIMIT_CPU");
PYTHON_PROVIDE("resource.RLIMIT_DATA");
PYTHON_PROVIDE("resource.RLIMIT_FSIZE");
PYTHON_PROVIDE("resource.RLIMIT_MEMLOCK");
PYTHON_PROVIDE("resource.RLIMIT_MSGQUEUE");
PYTHON_PROVIDE("resource.RLIMIT_NICE");
PYTHON_PROVIDE("resource.RLIMIT_NOFILE");
PYTHON_PROVIDE("resource.RLIMIT_NPROC");
PYTHON_PROVIDE("resource.RLIMIT_RSS");
PYTHON_PROVIDE("resource.RLIMIT_RTPRIO");
PYTHON_PROVIDE("resource.RLIMIT_RTTIME");
PYTHON_PROVIDE("resource.RLIMIT_SIGPENDING");
PYTHON_PROVIDE("resource.RLIMIT_STACK");
PYTHON_PROVIDE("resource.RLIMIT_VMEM");
PYTHON_PROVIDE("resource.RLIM_INFINITY");
PYTHON_PROVIDE("resource.RUSAGE_BOTH");
PYTHON_PROVIDE("resource.RUSAGE_CHILDREN");
PYTHON_PROVIDE("resource.RUSAGE_SELF");
PYTHON_PROVIDE("resource.RUSAGE_THREAD");
PYTHON_PROVIDE("resource.error");
PYTHON_PROVIDE("resource.getpagesize");
PYTHON_PROVIDE("resource.getrlimit");
PYTHON_PROVIDE("resource.getrusage");
PYTHON_PROVIDE("resource.setrlimit");
PYTHON_PROVIDE("resource.struct_rusage");

/* On some systems, these aren't in any header file.
   On others they are, with inconsistent prototypes.
   We declare the (default) return type, to shut up gcc -Wall;
   but we can't declare the prototype, to avoid errors
   when the header files declare it different.
   Worse, on some Linuxes, getpagesize() returns a size_t... */

#define doubletime(TV) ((double)(TV).tv_sec + (TV).tv_usec * 0.000001)

PyDoc_STRVAR(struct_rusage__doc__,
"struct_rusage: Result from getrusage.\n\n"
"This object may be accessed either as a tuple of\n"
"    (utime,stime,maxrss,ixrss,idrss,isrss,minflt,majflt,\n"
"    nswap,inblock,oublock,msgsnd,msgrcv,nsignals,nvcsw,nivcsw)\n"
"or via the attributes ru_utime, ru_stime, ru_maxrss, and so on.");

static PyStructSequence_Field struct_rusage_fields[] = {
    {"ru_utime",        PyDoc_STR("user time used")},
    {"ru_stime",        PyDoc_STR("system time used")},
    {"ru_maxrss",       PyDoc_STR("max. resident set size")},
    {"ru_ixrss",        PyDoc_STR("shared memory size")},
    {"ru_idrss",        PyDoc_STR("unshared data size")},
    {"ru_isrss",        PyDoc_STR("unshared stack size")},
    {"ru_minflt",       PyDoc_STR("page faults not requiring I/O")},
    {"ru_majflt",       PyDoc_STR("page faults requiring I/O")},
    {"ru_nswap",        PyDoc_STR("number of swap outs")},
    {"ru_inblock",      PyDoc_STR("block input operations")},
    {"ru_oublock",      PyDoc_STR("block output operations")},
    {"ru_msgsnd",       PyDoc_STR("IPC messages sent")},
    {"ru_msgrcv",       PyDoc_STR("IPC messages received")},
    {"ru_nsignals",     PyDoc_STR("signals received")},
    {"ru_nvcsw",        PyDoc_STR("voluntary context switches")},
    {"ru_nivcsw",       PyDoc_STR("involuntary context switches")},
    {0}
};

static PyStructSequence_Desc struct_rusage_desc = {
    "resource.struct_rusage",           /* name */
    struct_rusage__doc__,       /* doc */
    struct_rusage_fields,       /* fields */
    16          /* n_in_sequence */
};

static int initialized;
static PyTypeObject StructRUsageType;

static PyObject *
resource_getrusage(PyObject *self, PyObject *args)
{
    int who;
    struct rusage ru;
    PyObject *r;
    if (!PyArg_ParseTuple(args, "i:getrusage", &who))
        return NULL;
    if (getrusage(who, &ru) == -1) {
        if (errno == EINVAL) {
            PyErr_SetString(PyExc_ValueError,
                            "invalid who parameter");
            return NULL;
        }
        PyErr_SetFromErrno(PyExc_OSError);
        return NULL;
    }
    r = PyStructSequence_New(&StructRUsageType);
    if (!r)
        return NULL;
    PyStructSequence_SET_ITEM(r, 0, PyFloat_FromDouble(doubletime(ru.ru_utime)));
    PyStructSequence_SET_ITEM(r, 1, PyFloat_FromDouble(doubletime(ru.ru_stime)));
    PyStructSequence_SET_ITEM(r, 2, PyLong_FromLong(ru.ru_maxrss));
    PyStructSequence_SET_ITEM(r, 3, PyLong_FromLong(ru.ru_ixrss));
    PyStructSequence_SET_ITEM(r, 4, PyLong_FromLong(ru.ru_idrss));
    PyStructSequence_SET_ITEM(r, 5, PyLong_FromLong(ru.ru_isrss));
    PyStructSequence_SET_ITEM(r, 6, PyLong_FromLong(ru.ru_minflt));
    PyStructSequence_SET_ITEM(r, 7, PyLong_FromLong(ru.ru_majflt));
    PyStructSequence_SET_ITEM(r, 8, PyLong_FromLong(ru.ru_nswap));
    PyStructSequence_SET_ITEM(r, 9, PyLong_FromLong(ru.ru_inblock));
    PyStructSequence_SET_ITEM(r, 10, PyLong_FromLong(ru.ru_oublock));
    PyStructSequence_SET_ITEM(r, 11, PyLong_FromLong(ru.ru_msgsnd));
    PyStructSequence_SET_ITEM(r, 12, PyLong_FromLong(ru.ru_msgrcv));
    PyStructSequence_SET_ITEM(r, 13, PyLong_FromLong(ru.ru_nsignals));
    PyStructSequence_SET_ITEM(r, 14, PyLong_FromLong(ru.ru_nvcsw));
    PyStructSequence_SET_ITEM(r, 15, PyLong_FromLong(ru.ru_nivcsw));
    if (PyErr_Occurred()) {
        Py_DECREF(r);
        return NULL;
    }
    return r;
}

static int
py2rlimit(PyObject *limits, struct rlimit *rl_out)
{
    PyObject *curobj, *maxobj;
    limits = PySequence_Tuple(limits);
    if (!limits)
        /* Here limits is a borrowed reference */
        return -1;
    if (PyTuple_GET_SIZE(limits) != 2) {
        PyErr_SetString(PyExc_ValueError,
                        "expected a tuple of 2 integers");
        goto error;
    }
    curobj = PyTuple_GET_ITEM(limits, 0);
    maxobj = PyTuple_GET_ITEM(limits, 1);
#if !defined(HAVE_LARGEFILE_SUPPORT)
    rl_out->rlim_cur = PyLong_AsLong(curobj);
    if (rl_out->rlim_cur == (rlim_t)-1 && PyErr_Occurred())
        goto error;
    rl_out->rlim_max = PyLong_AsLong(maxobj);
    if (rl_out->rlim_max == (rlim_t)-1 && PyErr_Occurred())
        goto error;
#else
    /* The limits are probably bigger than a long */
    rl_out->rlim_cur = PyLong_AsLongLong(curobj);
    if (rl_out->rlim_cur == (rlim_t)-1 && PyErr_Occurred())
        goto error;
    rl_out->rlim_max = PyLong_AsLongLong(maxobj);
    if (rl_out->rlim_max == (rlim_t)-1 && PyErr_Occurred())
        goto error;
#endif
    Py_DECREF(limits);
    rl_out->rlim_cur = rl_out->rlim_cur & RLIM_INFINITY;
    rl_out->rlim_max = rl_out->rlim_max & RLIM_INFINITY;
    return 0;
error:
    Py_DECREF(limits);
    return -1;
}

static PyObject*
rlimit2py(struct rlimit rl)
{
    if (sizeof(rl.rlim_cur) > sizeof(long)) {
        return Py_BuildValue("LL",
                             (long long) rl.rlim_cur,
                             (long long) rl.rlim_max);
    }
    return Py_BuildValue("ll", (long) rl.rlim_cur, (long) rl.rlim_max);
}

static PyObject *
resource_getrlimit(PyObject *self, PyObject *args)
{
    struct rlimit rl;
    int resource;
    if (!PyArg_ParseTuple(args, "i:getrlimit", &resource))
        return NULL;
    if (resource < 0 || resource >= RLIM_NLIMITS) {
        PyErr_SetString(PyExc_ValueError,
                        "invalid resource specified");
        return NULL;
    }
    if (getrlimit(resource, &rl) == -1) {
        PyErr_SetFromErrno(PyExc_OSError);
        return NULL;
    }
    return rlimit2py(rl);
}

static PyObject *
resource_setrlimit(PyObject *self, PyObject *args)
{
    struct rlimit rl;
    int resource;
    PyObject *limits;
    if (!PyArg_ParseTuple(args, "iO:setrlimit", &resource, &limits))
        return NULL;
    if (resource < 0 || resource >= RLIM_NLIMITS) {
        PyErr_SetString(PyExc_ValueError,
                        "invalid resource specified");
        return NULL;
    }
    if (py2rlimit(limits, &rl) < 0) {
        return NULL;
    }
    if (setrlimit(resource, &rl) == -1) {
        if (errno == EINVAL)
            PyErr_SetString(PyExc_ValueError,
                            "current limit exceeds maximum limit");
        else if (errno == EPERM)
            PyErr_SetString(PyExc_ValueError,
                            "not allowed to raise maximum limit");
        else
            PyErr_SetFromErrno(PyExc_OSError);
        return NULL;
    }
    Py_RETURN_NONE;
}

#ifdef HAVE_PRLIMIT
static PyObject *
resource_prlimit(PyObject *self, PyObject *args)
{
    struct rlimit old_limit, new_limit;
    int resource, retval;
    pid_t pid;
    PyObject *limits = NULL;
    if (!PyArg_ParseTuple(args, _Py_PARSE_PID "i|O:prlimit",
                          &pid, &resource, &limits))
        return NULL;
    if (resource < 0 || resource >= RLIM_NLIMITS) {
        PyErr_SetString(PyExc_ValueError,
                        "invalid resource specified");
        return NULL;
    }
    if (limits != NULL) {
        if (py2rlimit(limits, &new_limit) < 0) {
            return NULL;
        }
        retval = prlimit(pid, resource, &new_limit, &old_limit);
    }
    else {
        retval = prlimit(pid, resource, NULL, &old_limit);
    }
    if (retval == -1) {
        if (errno == EINVAL) {
            PyErr_SetString(PyExc_ValueError,
                            "current limit exceeds maximum limit");
        } else {
            PyErr_SetFromErrno(PyExc_OSError);
        }
        return NULL;
    }
    return rlimit2py(old_limit);
}
#endif /* HAVE_PRLIMIT */

static PyObject *
resource_getpagesize(PyObject *self, PyObject *unused)
{
    long pagesize = 0;
#if defined(HAVE_GETPAGESIZE)
    pagesize = getpagesize();
#elif defined(HAVE_SYSCONF)
#if defined(_SC_PAGE_SIZE)
    pagesize = sysconf(_SC_PAGE_SIZE);
#else
    /* Irix 5.3 has _SC_PAGESIZE, but not _SC_PAGE_SIZE */
    pagesize = sysconf(_SC_PAGESIZE);
#endif
#endif
    return Py_BuildValue("i", pagesize);
}

static struct PyMethodDef
resource_methods[] = {
    {"getrusage",    resource_getrusage,   METH_VARARGS},
    {"getrlimit",    resource_getrlimit,   METH_VARARGS},
#ifdef HAVE_PRLIMIT
    {"prlimit",      resource_prlimit,     METH_VARARGS},
#endif
    {"setrlimit",    resource_setrlimit,   METH_VARARGS},
    {"getpagesize",  resource_getpagesize, METH_NOARGS},
    {NULL, NULL}                             /* sentinel */
};

static struct PyModuleDef resourcemodule = {
    PyModuleDef_HEAD_INIT,
    "resource",
    NULL,
    -1,
    resource_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_resource(void)
{
    PyObject *m, *v;

    /* Create the module and add the functions */
    m = PyModule_Create(&resourcemodule);
    if (m == NULL)
        return NULL;

    /* Add some symbolic constants to the module */
    Py_INCREF(PyExc_OSError);
    PyModule_AddObject(m, "error", PyExc_OSError);
    if (!initialized) {
        if (PyStructSequence_InitType2(&StructRUsageType,
                                       &struct_rusage_desc) < 0)
            return NULL;
    }

    Py_INCREF(&StructRUsageType);
    PyModule_AddObject(m, "struct_rusage",
                       (PyObject*) &StructRUsageType);

    /* insert constants */
    if (RLIMIT_CPU!=127) PyModule_AddIntMacro(m, RLIMIT_CPU);
    if (RLIMIT_FSIZE!=127) PyModule_AddIntMacro(m, RLIMIT_FSIZE);
    if (RLIMIT_DATA!=127) PyModule_AddIntMacro(m, RLIMIT_DATA);
    if (RLIMIT_STACK!=127) PyModule_AddIntMacro(m, RLIMIT_STACK);
    if (RLIMIT_CORE!=127) PyModule_AddIntMacro(m, RLIMIT_CORE);
    if (RLIMIT_NOFILE!=127) PyModule_AddIntMacro(m, RLIMIT_NOFILE);
    if (RLIMIT_VMEM!=127) PyModule_AddIntMacro(m, RLIMIT_VMEM);
    if (RLIMIT_AS!=127) PyModule_AddIntMacro(m, RLIMIT_AS);
    if (RLIMIT_RSS!=127) PyModule_AddIntMacro(m, RLIMIT_RSS);
    if (RLIMIT_NPROC!=127) PyModule_AddIntMacro(m, RLIMIT_NPROC);
    if (RLIMIT_MEMLOCK!=127) PyModule_AddIntMacro(m, RLIMIT_MEMLOCK);
    if (RLIMIT_SBSIZE!=127) PyModule_AddIntMacro(m, RLIMIT_SBSIZE);
    
    /* Linux specific */
    if (RLIMIT_MSGQUEUE!=127) PyModule_AddIntMacro(m, RLIMIT_MSGQUEUE);
    if (RLIMIT_NICE!=127) PyModule_AddIntMacro(m, RLIMIT_NICE);
    if (RLIMIT_RTPRIO!=127) PyModule_AddIntMacro(m, RLIMIT_RTPRIO);
    if (RLIMIT_RTTIME!=127) PyModule_AddIntMacro(m, RLIMIT_RTTIME);
    if (RLIMIT_SIGPENDING!=127) PyModule_AddIntMacro(m, RLIMIT_SIGPENDING);

    /* FreeBSD specific */
    if (RLIMIT_SWAP!=127) PyModule_AddIntMacro(m, RLIMIT_SWAP);
    if (RLIMIT_SBSIZE!=127) PyModule_AddIntMacro(m, RLIMIT_SBSIZE);
    if (RLIMIT_NPTS!=127) PyModule_AddIntMacro(m, RLIMIT_NPTS);

    /* target */
    PyModule_AddIntMacro(m, RUSAGE_SELF);
    if (RUSAGE_CHILDREN!=99) PyModule_AddIntMacro(m, RUSAGE_CHILDREN);
    if (RUSAGE_BOTH!=99) PyModule_AddIntMacro(m, RUSAGE_BOTH);
    if (RUSAGE_THREAD!=99) PyModule_AddIntMacro(m, RUSAGE_THREAD);

    if (sizeof(RLIM_INFINITY) > sizeof(long)) {
        v = PyLong_FromLongLong((long long) RLIM_INFINITY);
    } else
    {
        v = PyLong_FromLong((long) RLIM_INFINITY);
    }
    if (v) {
        PyModule_AddObject(m, "RLIM_INFINITY", v);
    }
    initialized = 1;
    return m;
}

#ifdef __aarch64__
_Section(".rodata.pytab.1 //")
#else
_Section(".rodata.pytab.1")
#endif
 const struct _inittab _PyImport_Inittab_resource = {
    "resource",
    PyInit_resource,
};
