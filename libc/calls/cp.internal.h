#ifndef COSMOPOLITAN_LIBC_CALLS_CP_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_CP_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int begin_cancellation_point(void);
void end_cancellation_point(int);

#ifndef MODE_DBG
#define BEGIN_CANCELLATION_POINT (void)0
#define END_CANCELLATION_POINT   (void)0
#else
#define BEGIN_CANCELLATION_POINT \
  do {                           \
    int _Cp;                     \
  _Cp = begin_cancellation_point()
#define END_CANCELLATION_POINT \
  end_cancellation_point(_Cp); \
  }                            \
  while (0)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_CP_INTERNAL_H_ */
