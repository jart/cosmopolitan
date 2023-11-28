#ifndef Py_LIMITED_API
#ifndef Py_PYDEBUG_H
#define Py_PYDEBUG_H
COSMOPOLITAN_C_START_
#include "libc/runtime/runtime.h"

/* These global variable are defined in pylifecycle.c */
/* XXX (ncoghlan): move these declarations to pylifecycle.h? */
extern int Py_DebugFlag;
extern int Py_VerboseFlag;
extern int Py_QuietFlag;
extern int Py_InteractiveFlag;
extern int Py_InspectFlag;
extern int Py_OptimizeFlag;
extern int Py_NoSiteFlag;
extern int Py_BytesWarningFlag;
extern int Py_UseClassExceptionsFlag;
extern int Py_FrozenFlag;
extern int Py_IgnoreEnvironmentFlag;
extern int Py_DontWriteBytecodeFlag;
extern int Py_NoUserSiteDirectory;
extern int Py_UnbufferedStdioFlag;
extern int Py_HashRandomizationFlag;
extern int Py_IsolatedFlag;

#ifdef MS_WINDOWS
extern int Py_LegacyWindowsStdioFlag;
#endif

/* this is a wrapper around getenv() that pays attention to
   Py_IgnoreEnvironmentFlag.  It should be used for getting variables like
   PYTHONPATH and PYTHONHOME from the environment */
#define Py_GETENV(s) (Py_IgnoreEnvironmentFlag ? NULL : getenv(s))

COSMOPOLITAN_C_END_
#endif /* !Py_PYDEBUG_H */
#endif /* Py_LIMITED_API */
