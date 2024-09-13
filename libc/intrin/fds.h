#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_
#include "libc/sock/struct/sockaddr.h"
#include "libc/thread/thread.h"
COSMOPOLITAN_C_START_

#define kFdEmpty     0
#define kFdFile      1
#define kFdSocket    2
#define kFdConsole   4
#define kFdSerial    5
#define kFdZip       6
#define kFdEpoll     7 /* epoll() deleted on 2024-09-01 */
#define kFdReserved  8
#define kFdDevNull   9
#define kFdDevRandom 10

struct CursorShared {
  pthread_mutex_t lock;
  long pointer;
};

struct Cursor {
  struct CursorShared *shared;
  _Atomic(int) refs;
};

struct Fd {
  char kind;
  bool isbound;
  char connecting;
  unsigned flags;
  unsigned mode;
  long handle;
  int family;
  int type;
  int protocol;
  unsigned rcvtimeo; /* millis; 0 means wait forever */
  unsigned sndtimeo; /* millis; 0 means wait forever */
  void *connect_op;
  struct Cursor *cursor;
};

struct Fds {
  _Atomic(int) f; /* lowest free slot */
  size_t n;
  struct Fd *p, *e;
};

struct Cursor *__cursor_new(void);
void __cursor_ref(struct Cursor *);
int __cursor_unref(struct Cursor *);
void __cursor_lock(struct Cursor *);
void __cursor_unlock(struct Cursor *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_ */
