#ifndef _SYS_USER_H
#define _SYS_USER_H
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __x86_64__

#undef __WORDSIZE
#define __WORDSIZE 64

typedef struct user_fpregs_struct {
  uint16_t cwd, swd, ftw, fop;
  uint64_t rip, rdp;
  uint32_t mxcsr, mxcr_mask;
  uint32_t st_space[32], xmm_space[64], padding[24];
} elf_fpregset_t;

struct user_regs_struct {
  unsigned long r15, r14, r13, r12, rbp, rbx, r11, r10, r9, r8;
  unsigned long rax, rcx, rdx, rsi, rdi, orig_rax, rip;
  unsigned long cs, eflags, rsp, ss, fs_base, gs_base, ds, es, fs, gs;
};
#define ELF_NGREG 27
typedef unsigned long long elf_greg_t, elf_gregset_t[ELF_NGREG];

struct user {
  struct user_regs_struct regs;
  int u_fpvalid;
  struct user_fpregs_struct i387;
  unsigned long u_tsize;
  unsigned long u_dsize;
  unsigned long u_ssize;
  unsigned long start_code;
  unsigned long start_stack;
  long signal;
  int reserved;
  struct user_regs_struct *u_ar0;
  struct user_fpregs_struct *u_fpstate;
  unsigned long magic;
  char u_comm[32];
  unsigned long u_debugreg[8];
};

#define PAGE_MASK            (~(4096 - 1))
#define NBPG                 4096
#define UPAGES               1
#define HOST_TEXT_START_ADDR (u.start_code)
#define HOST_STACK_END_ADDR  (u.start_stack + u.u_ssize * NBPG)

#elif defined(__aarch64__)

struct user_regs_struct {
  unsigned long long regs[31];
  unsigned long long sp;
  unsigned long long pc;
  unsigned long long pstate;
};

struct user_fpsimd_struct {
  unsigned __int128 vregs[32];
  unsigned int fpsr;
  unsigned int fpcr;
};

#define ELF_NREG 34
typedef unsigned long elf_greg_t, elf_gregset_t[ELF_NREG];
typedef struct user_fpsimd_struct elf_fpregset_t;

#else
#error "unsupported architecture"
#endif

#ifdef __cplusplus
}
#endif
#endif
