#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_SYSCALL_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_SYSCALL_H_
#include "tool/build/lib/fds.h"
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const struct MachineFdCb kMachineFdCbHost;

void OpSyscall(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_SYSCALL_H_ */
