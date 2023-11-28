#ifndef Py_CURSES_H
#define Py_CURSES_H
#include "third_party/python/Include/object.h"

#if !defined(HAVE_CURSES_IS_PAD) && defined(WINDOW_HAS_FLAGS)
/* The following definition is necessary for ncurses 5.7; without it,
   some of [n]curses.h set NCURSES_OPAQUE to 1, and then Python
   can't get at the WINDOW flags field. */
#define NCURSES_OPAQUE 0
#endif

#ifdef HAVE_NCURSES_H
/* configure was checking <curses.h>, but we will
   use <ncurses.h>, which has some or all these features. */
#if !defined(WINDOW_HAS_FLAGS) && !(NCURSES_OPAQUE+0)
#define WINDOW_HAS_FLAGS 1
#endif
#if !defined(HAVE_CURSES_IS_PAD) && NCURSES_VERSION_PATCH+0 >= 20090906
#define HAVE_CURSES_IS_PAD 1
#endif
#ifndef MVWDELCH_IS_EXPRESSION
#define MVWDELCH_IS_EXPRESSION 1
#endif
#endif

COSMOPOLITAN_C_START_

#define PyCurses_API_pointers 4

/* Type declarations */

typedef struct {
    PyObject_HEAD
    WINDOW *win;
    char *encoding;
} PyCursesWindowObject;

#define PyCursesWindow_Check(v)  (Py_TYPE(v) == &PyCursesWindow_Type)

#define PyCurses_CAPSULE_NAME "_curses._C_API"


#ifdef CURSES_MODULE
/* This section is used when compiling _cursesmodule.c */

#else
/* This section is used in modules that use the _cursesmodule API */

static void **PyCurses_API;

#define PyCursesWindow_Type (*(PyTypeObject *) PyCurses_API[0])
#define PyCursesSetupTermCalled  {if (! ((int (*)(void))PyCurses_API[1]) () ) return NULL;}
#define PyCursesInitialised      {if (! ((int (*)(void))PyCurses_API[2]) () ) return NULL;}
#define PyCursesInitialisedColor {if (! ((int (*)(void))PyCurses_API[3]) () ) return NULL;}

#define import_curses() \
    PyCurses_API = (void **)PyCapsule_Import(PyCurses_CAPSULE_NAME, 1);

#endif

/* general error messages */
static const char catchall_ERR[]  = "curses function returned ERR";
static const char catchall_NULL[] = "curses function returned NULL";

/* Function Prototype Macros - They are ugly but very, very useful. ;-)

   X - function name
   TYPE - parameter Type
   ERGSTR - format string for construction of the return value
   PARSESTR - format string for argument parsing
   */

#define NoArgNoReturnFunction(X) \
static PyObject *PyCurses_ ## X (PyObject *self) \
{ \
  PyCursesInitialised \
  return PyCursesCheckERR(X(), # X); }

#define NoArgOrFlagNoReturnFunction(X) \
static PyObject *PyCurses_ ## X (PyObject *self, PyObject *args) \
{ \
  int flag = 0; \
  PyCursesInitialised \
  switch(PyTuple_Size(args)) { \
  case 0: \
    return PyCursesCheckERR(X(), # X); \
  case 1: \
    if (!PyArg_ParseTuple(args, "i;True(1) or False(0)", &flag)) return NULL; \
    if (flag) return PyCursesCheckERR(X(), # X); \
    else return PyCursesCheckERR(no ## X (), # X); \
  default: \
    PyErr_SetString(PyExc_TypeError, # X " requires 0 or 1 arguments"); \
    return NULL; } }

#define NoArgReturnIntFunction(X) \
static PyObject *PyCurses_ ## X (PyObject *self) \
{ \
 PyCursesInitialised \
 return PyLong_FromLong((long) X()); }


#define NoArgReturnStringFunction(X) \
static PyObject *PyCurses_ ## X (PyObject *self) \
{ \
  PyCursesInitialised \
  return PyBytes_FromString(X()); }

#define NoArgTrueFalseFunction(X) \
static PyObject *PyCurses_ ## X (PyObject *self) \
{ \
  PyCursesInitialised \
  if (X () == FALSE) { \
    Py_INCREF(Py_False); \
    return Py_False; \
  } \
  Py_INCREF(Py_True); \
  return Py_True; }

#define NoArgNoReturnVoidFunction(X) \
static PyObject *PyCurses_ ## X (PyObject *self) \
{ \
  PyCursesInitialised \
  X(); \
  Py_INCREF(Py_None); \
  return Py_None; }

COSMOPOLITAN_C_END_
#endif /* !defined(Py_CURSES_H) */
