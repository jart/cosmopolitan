#ifndef Py_LIMITED_API
#ifndef Py_PYTIME_H
#define Py_PYTIME_H
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/weirdtypes.h"
#include "libc/time/struct/tm.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/pyconfig.h"
COSMOPOLITAN_C_START_

typedef int64_t _PyTime_t;
#define _PyTime_MIN PY_LLONG_MIN
#define _PyTime_MAX PY_LLONG_MAX

typedef enum {
  _PyTime_ROUND_FLOOR = 0,
  _PyTime_ROUND_CEILING = 1,
  _PyTime_ROUND_HALF_EVEN = 2,
  _PyTime_ROUND_UP = 3,
  _PyTime_ROUND_TIMEOUT = _PyTime_ROUND_UP
} _PyTime_round_t;

PyObject *_PyLong_FromTime_t(time_t);
time_t _PyLong_AsTime_t(PyObject *);
int _PyTime_ObjectToTime_t(PyObject *, time_t *, _PyTime_round_t);
int _PyTime_ObjectToTimeval(PyObject *, time_t *, long *, _PyTime_round_t);
int _PyTime_ObjectToTimespec(PyObject *, time_t *, long *, _PyTime_round_t);

_PyTime_t _PyTime_FromSeconds(int);

#define _PYTIME_FROMSECONDS(seconds) \
  ((_PyTime_t)(seconds) * (1000 * 1000 * 1000))

_PyTime_t _PyTime_FromNanoseconds(long long);
int _PyTime_FromSecondsObject(_PyTime_t *, PyObject *, _PyTime_round_t);
int _PyTime_FromMillisecondsObject(_PyTime_t *, PyObject *, _PyTime_round_t);
double _PyTime_AsSecondsDouble(_PyTime_t);
_PyTime_t _PyTime_AsMilliseconds(_PyTime_t, _PyTime_round_t);
_PyTime_t _PyTime_AsMicroseconds(_PyTime_t, _PyTime_round_t);
PyObject *_PyTime_AsNanosecondsObject(_PyTime_t);
int _PyTime_AsTimeval(_PyTime_t, struct timeval *, _PyTime_round_t);
int _PyTime_AsTimeval_noraise(_PyTime_t, struct timeval *, _PyTime_round_t);
int _PyTime_AsTimevalTime_t(_PyTime_t, time_t *, int *, _PyTime_round_t);
int _PyTime_AsTimespec(_PyTime_t, struct timespec *);
int _PyTime_FromTimeval(_PyTime_t *, struct timeval *);
int _PyTime_FromTimespec(_PyTime_t *, struct timespec *);
_PyTime_t _PyTime_GetSystemClock(void);
_PyTime_t _PyTime_GetMonotonicClock(void);
_PyTime_t _PyTime_MulDiv(_PyTime_t, _PyTime_t, _PyTime_t);

typedef struct {
  const char *implementation;
  int monotonic;
  int adjustable;
  double resolution;
} _Py_clock_info_t;

int _PyTime_GetSystemClockWithInfo(_PyTime_t *, _Py_clock_info_t *);
int _PyTime_GetMonotonicClockWithInfo(_PyTime_t *, _Py_clock_info_t *);
int _PyTime_GetPerfCounterWithInfo(_PyTime_t *, _Py_clock_info_t *);
int _PyTime_Init(void);
int _PyTime_localtime(time_t, struct tm *);
int _PyTime_gmtime(time_t, struct tm *);

COSMOPOLITAN_C_END_
#endif /* Py_PYTIME_H */
#endif /* Py_LIMITED_API */
