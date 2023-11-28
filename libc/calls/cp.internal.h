#ifndef COSMOPOLITAN_LIBC_CALLS_CP_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_CP_INTERNAL_H_
COSMOPOLITAN_C_START_

int begin_cancelation_point(void);
void end_cancelation_point(int);

#ifndef MODE_DBG
#define BEGIN_CANCELATION_POINT (void)0
#define END_CANCELATION_POINT   (void)0
#else
#define BEGIN_CANCELATION_POINT \
  do {                           \
    int _Cp;                     \
  _Cp = begin_cancelation_point()
#define END_CANCELATION_POINT \
  end_cancelation_point(_Cp); \
  }                            \
  while (0)
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_CP_INTERNAL_H_ */
