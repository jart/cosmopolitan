#ifndef Py_INTRCHECK_H
#define Py_INTRCHECK_H
COSMOPOLITAN_C_START_
/* clang-format off */

PyAPI_FUNC(int) PyOS_InterruptOccurred(void);
PyAPI_FUNC(void) PyOS_InitInterrupts(void);
PyAPI_FUNC(void) PyOS_AfterFork(void);

#ifndef Py_LIMITED_API
PyAPI_FUNC(int) _PyOS_IsMainThread(void);

#ifdef MS_WINDOWS
/* windows.h is not included by Python.h so use void* instead of HANDLE */
PyAPI_FUNC(void*) _PyOS_SigintEvent(void);
#endif
#endif /* !Py_LIMITED_API */

COSMOPOLITAN_C_END_
#endif /* !Py_INTRCHECK_H */
