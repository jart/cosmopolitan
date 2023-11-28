#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_USER_REGS_STRUCT_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_USER_REGS_STRUCT_H_
COSMOPOLITAN_C_START_

/**
 * Linux Kernel user registers.
 *
 * @note superset of struct pt_regs
 * @see ptrace() w/ PTRACE_SYSCALL
 */
struct user_regs_struct {
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t rbp;
  uint64_t rbx;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rax;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t orig_rax;
  uint64_t rip;
  uint64_t cs;
  uint64_t eflags;
  uint64_t rsp;
  uint64_t ss;
  uint64_t fs_base;
  uint64_t gs_base;
  uint64_t ds;
  uint64_t es;
  uint64_t fs;
  uint64_t gs;
};

struct useregs_struct_freebsd {
  int64_t r15;
  int64_t r14;
  int64_t r13;
  int64_t r12;
  int64_t r11;
  int64_t r10;
  int64_t r9;
  int64_t r8;
  int64_t rdi;
  int64_t rsi;
  int64_t rbp;
  int64_t rbx;
  int64_t rdx;
  int64_t rcx;
  int64_t rax;
  uint32_t trapno;
  uint16_t fs;
  uint16_t gs;
  uint32_t err;
  uint16_t es;
  uint16_t ds;
  int64_t rip;
  int64_t cs;
  int64_t rflags;
  int64_t rsp;
  int64_t ss;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_USER_REGS_STRUCT_H_ */
