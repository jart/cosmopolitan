#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_
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
#define kFdTtyMunging 4 /* enable input / output remappings */
#define kFdTtyNoCr2Nl 8 /* don't map \r → \n (a.k.a !ICRNL) */
#define kFdTtyNoIsigs 16

struct Fd {
  char kind;
  bool zombie;
  bool dontclose;
  char buflen;
  char buf[4];
  unsigned flags;
  unsigned mode;
  int64_t handle;
  int64_t extra;
};

struct StdinRelay {
  _Atomic(uint32_t) once;
  int64_t inisem; /* semaphore to delay 1st read */
  int64_t handle; /* should == g_fds.p[0].handle */
  int64_t reader; /* ReadFile() use this instead */
  int64_t writer; /* only used by WinStdinThread */
  int64_t thread; /* handle for the stdio thread */
};

struct Fds {
  _Atomic(int) f; /* lowest free slot */
  size_t n;
  struct Fd *p, *e;
  struct StdinRelay stdin;
};

void WinMainStdin(void);
int64_t __resolve_stdin_handle(int64_t);
int __munge_terminal_input(char *, uint32_t *);
void __echo_terminal_input(struct Fd *, char *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FD_INTERNAL_H_ */
