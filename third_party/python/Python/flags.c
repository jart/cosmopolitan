/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pylifecycle.h"

/* Global configuration variable declarations are in pydebug.h */
/* XXX (ncoghlan): move those declarations to pylifecycle.h? */
int Py_DebugFlag; /* Needed by parser.c */
int Py_VerboseFlag; /* Needed by import.c */
int Py_QuietFlag; /* Needed by sysmodule.c */
int Py_InteractiveFlag; /* Needed by Py_FdIsInteractive() below */
int Py_InspectFlag; /* Needed to determine whether to exit at SystemExit */
int Py_OptimizeFlag; /* Needed by compile.c */
int Py_NoSiteFlag; /* Suppress 'import site' */
int Py_BytesWarningFlag; /* Warn on str(bytes) and str(buffer) */
int Py_UseClassExceptionsFlag = 1; /* Needed by bltinmodule.c: deprecated */
int Py_FrozenFlag; /* Needed by getpath.c */
int Py_IgnoreEnvironmentFlag; /* e.g. PYTHONPATH, PYTHONHOME */
int Py_DontWriteBytecodeFlag; /* Suppress writing bytecode files (*.pyc) */
int Py_NoUserSiteDirectory; /* for -s and site.py */
int Py_UnbufferedStdioFlag; /* Unbuffered binary std{in,out,err} */
int Py_HashRandomizationFlag; /* for -R and PYTHONHASHSEED */
int Py_IsolatedFlag; /* for -I, isolate from user's env */
#ifdef MS_WINDOWS
int Py_LegacyWindowsFSEncodingFlag; /* Uses mbcs instead of utf-8 */
int Py_LegacyWindowsStdioFlag; /* Uses FileIO instead of WindowsConsoleIO */
#endif

PyThreadState *_Py_Finalizing;
