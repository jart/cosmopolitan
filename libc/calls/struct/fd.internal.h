#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_
COSMOPOLITAN_C_START_

#define kFdEmpty    0
#define kFdFile     1
#define kFdSocket   2
#define kFdConsole  4
#define kFdSerial   5
#define kFdZip      6
#define kFdEpoll    7
#define kFdReserved 8
#define kFdDevNull  9

struct Fd {
  char kind;
  bool isbound;
  unsigned flags;
  unsigned mode;
  long handle;
  long pointer;
  int family;
  int type;
  int protocol;
  unsigned rcvtimeo; /* millis; 0 means wait forever */
  unsigned sndtimeo; /* millis; 0 means wait forever */
  void *connect_op;
};

struct Fds {
  _Atomic(int) f; /* lowest free slot */
  size_t n;
  struct Fd *p, *e;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_ */
