#ifndef COSMOPOLITAN_LIBC_SOCK_NTSTDIN_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_NTSTDIN_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtStdinWorker { /* non-inherited */
  volatile char sync;  /* spin sync start */
  int refs;            /* reference count */
  uint32_t tid;        /* of the worker */
  int64_t reader;      /* the real handle */
  int64_t writer;      /* for the worker */
  int64_t worker;      /* thread handle */
  int64_t consumer;    /* same as Fd::handle */
  char16_t name[64];   /* for named pipe */
};

struct NtStdinWorker *NewNtStdinWorker(int) hidden;
struct NtStdinWorker *RefNtStdinWorker(struct NtStdinWorker *) hidden;
bool UnrefNtStdinWorker(struct NtStdinWorker *) hidden;
void ForkNtStdinWorker(void) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_NTSTDIN_INTERNAL_H_ */
