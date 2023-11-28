#ifndef Py_FILEOBJECT_H
#define Py_FILEOBJECT_H
#include "third_party/python/Include/object.h"
COSMOPOLITAN_C_START_

#define PY_STDIOTEXTMODE "b"

PyObject * PyFile_FromFd(int, const char *, const char *, int,
                                     const char *, const char *,
                                     const char *, int);
PyObject * PyFile_GetLine(PyObject *, int);
int PyFile_WriteObject(PyObject *, PyObject *, int);
int PyFile_WriteString(const char *, PyObject *);
int PyObject_AsFileDescriptor(PyObject *);
#ifndef Py_LIMITED_API
char * Py_UniversalNewlineFgets(char *, int, FILE*, PyObject *);
#endif

/* The default encoding used by the platform file system APIs
   If non-NULL, this is different than the default encoding for strings
*/
extern const char * Py_FileSystemDefaultEncoding;
#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x03060000
extern const char * Py_FileSystemDefaultEncodeErrors;
#endif
extern int Py_HasFileSystemDefaultEncoding;

/* Internal API

   The std printer acts as a preliminary sys.stderr until the new io
   infrastructure is in place. */
#ifndef Py_LIMITED_API
PyObject * PyFile_NewStdPrinter(int);
extern PyTypeObject PyStdPrinter_Type;
#endif /* Py_LIMITED_API */

/* A routine to check if a file descriptor can be select()-ed. */
#ifdef HAVE_SELECT
 #define _PyIsSelectable_fd(FD) ((unsigned int)(FD) < (unsigned int)FD_SETSIZE)
#else
 #define _PyIsSelectable_fd(FD) (1)
#endif /* HAVE_SELECT */

COSMOPOLITAN_C_END_
#endif /* !Py_FILEOBJECT_H */
