#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_BITS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_BITS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct iovec_bits {
  uint8_t iov_base[8];
  uint8_t iov_len[8];
};

struct pollfd_bits {
  uint8_t fd[4];
  uint8_t events[2];
  uint8_t revents[2];
};

struct timeval_bits {
  uint8_t tv_sec[8];
  uint8_t tv_usec[8];
};

struct timespec_bits {
  uint8_t tv_sec[8];
  uint8_t tv_nsec[8];
};

struct timezone_bits {
  uint8_t tz_minuteswest[4];
  uint8_t tz_dsttime[4];
};

struct sigaction_bits {
  uint8_t handler[8];
  uint8_t flags[8];
  uint8_t restorer[8];
  uint8_t mask[8];
};

struct winsize_bits {
  uint8_t ws_row[2];
  uint8_t ws_col[2];
  uint8_t ws_xpixel[2];
  uint8_t ws_ypixel[2];
};

struct termios_bits {
  uint8_t c_iflag[4];
  uint8_t c_oflag[4];
  uint8_t c_cflag[4];
  uint8_t c_lflag[4];
  uint8_t c_cc[32];
  uint8_t c_ispeed[4];
  uint8_t c_ospeed[4];
};

struct sockaddr_in_bits {
  uint8_t sin_family[2];
  uint16_t sin_port;
  uint32_t sin_addr;
  uint8_t sin_zero[8];
};

struct stat_bits {
  uint8_t st_dev[8];
  uint8_t st_ino[8];
  uint8_t st_nlink[8];
  uint8_t st_mode[4];
  uint8_t st_uid[4];
  uint8_t st_gid[4];
  uint8_t __pad[4];
  uint8_t st_rdev[8];
  uint8_t st_size[8];
  uint8_t st_blksize[8];
  uint8_t st_blocks[8];
  struct timespec_bits st_atim;
  struct timespec_bits st_mtim;
  struct timespec_bits st_ctim;
};

struct itimerval_bits {
  struct timeval_bits it_interval;
  struct timeval_bits it_value;
};

struct rusage_bits {
  struct timeval_bits ru_utime;
  struct timeval_bits ru_stime;
  uint8_t ru_maxrss[8];
  uint8_t ru_ixrss[8];
  uint8_t ru_idrss[8];
  uint8_t ru_isrss[8];
  uint8_t ru_minflt[8];
  uint8_t ru_majflt[8];
  uint8_t ru_nswap[8];
  uint8_t ru_inblock[8];
  uint8_t ru_oublock[8];
  uint8_t ru_msgsnd[8];
  uint8_t ru_msgrcv[8];
  uint8_t ru_nsignals[8];
  uint8_t ru_nvcsw[8];
  uint8_t ru_nivcsw[8];
};

struct siginfo_bits {
  uint8_t si_signo[4];
  uint8_t si_errno[4];
  uint8_t si_code[4];
  uint8_t __pad[4];
  uint8_t payload[112];
};

struct fpstate_bits {
  uint8_t cwd[2];
  uint8_t swd[2];
  uint8_t ftw[2];
  uint8_t fop[2];
  uint8_t rip[8];
  uint8_t rdp[8];
  uint8_t mxcsr[4];
  uint8_t mxcr_mask[4];
  uint8_t st[8][16];
  uint8_t xmm[16][16];
  uint8_t __padding[96];
};

struct ucontext_bits {
  uint8_t uc_flags[8];
  uint8_t uc_link[8];
  uint8_t ss_sp[8];
  uint8_t ss_flags[4];
  uint8_t __pad0[4];
  uint8_t ss_size[8];
  uint8_t r8[8];
  uint8_t r9[8];
  uint8_t r10[8];
  uint8_t r11[8];
  uint8_t r12[8];
  uint8_t r13[8];
  uint8_t r14[8];
  uint8_t r15[8];
  uint8_t rdi[8];
  uint8_t rsi[8];
  uint8_t rbp[8];
  uint8_t rbx[8];
  uint8_t rdx[8];
  uint8_t rax[8];
  uint8_t rcx[8];
  uint8_t rsp[8];
  uint8_t rip[8];
  uint8_t eflags[8];
  uint8_t cs[2];
  uint8_t gs[2];
  uint8_t fs[2];
  uint8_t ss[2];
  uint8_t err[8];
  uint8_t trapno[8];
  uint8_t oldmask[8];
  uint8_t cr2[8];
  uint8_t fpstate[8];
  uint8_t __pad1[64];
  uint8_t uc_sigmask[8];
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_BITS_H_ */
