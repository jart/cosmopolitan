#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_
#include "libc/nt/struct/overlapped.h"
#include "libc/thread/thread.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define DO_NOTHING 0
#define DO_RESTART 1
#define DO_EINTR   2

#define kFdEmpty    0
#define kFdFile     1
#define kFdSocket   2
#define kFdProcess  3
#define kFdConsole  4
#define kFdSerial   5
#define kFdZip      6
#define kFdEpoll    7
#define kFdReserved 8

#define kFdTtyEchoing 1 /* read()→write() (ECHO && !ICANON) */
#define kFdTtyEchoRaw 2 /* don't ^X visualize control codes */
#define kFdTtyUncanon 4 /* enables non-canonical (raw) mode */
#define kFdTtyNoCr2Nl 8 /* don't map \r → \n (a.k.a !ICRNL) */
#define kFdTtyNoIsigs 16
#define kFdTtyNoBlock 32
#define kFdTtyXtMouse 64

struct Fd {
  char kind;
  bool eoftty;
  bool dontclose;
  unsigned flags;
  unsigned mode;
  int64_t handle;
  int64_t extra;
  int64_t pointer;
  pthread_mutex_t lock;
};

struct StdinRelay {
  _Atomic(uint32_t) once;
  int64_t inisem; /* semaphore to delay 1st read */
  int64_t handle; /* should == g_fds.p[0].handle */
  int64_t reader; /* ReadFile() use this instead */
  int64_t writer; /* only used by WinStdinThread */
  int64_t thread; /* handle for the stdio thread */
  struct NtOverlapped overlap;
};

struct Fds {
  _Atomic(int) f; /* lowest free slot */
  size_t n;
  struct Fd *p, *e;
  struct StdinRelay stdin;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_ */
