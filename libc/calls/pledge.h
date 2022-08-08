#ifndef COSMOPOLITAN_LIBC_CALLS_PLEDGE_H_
#define COSMOPOLITAN_LIBC_CALLS_PLEDGE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

enum PledgeMode {
  kPledgeModeKillThread,
  kPledgeModeKillProcess,
  kPledgeModeErrno,
};

extern enum PledgeMode __pledge_mode;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_PLEDGE_H_ */
