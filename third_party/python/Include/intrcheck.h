#ifndef Py_INTRCHECK_H
#define Py_INTRCHECK_H
COSMOPOLITAN_C_START_

int PyOS_InterruptOccurred(void);
void PyOS_InitInterrupts(void);
void PyOS_AfterFork(void);

#ifndef Py_LIMITED_API
int _PyOS_IsMainThread(void);

#ifdef MS_WINDOWS
/* windows.h is not included by Python.h so use void* instead of HANDLE */
void* _PyOS_SigintEvent(void);
#endif
#endif /* !Py_LIMITED_API */

COSMOPOLITAN_C_END_
#endif /* !Py_INTRCHECK_H */
