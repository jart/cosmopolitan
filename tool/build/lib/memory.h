#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_MEMORY_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_MEMORY_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int RegisterMemory(struct Machine *, int64_t, void *, size_t);
void *AccessRam(struct Machine *, int64_t, size_t, void *[2], uint8_t *, bool);
void *BeginLoadStore(struct Machine *, int64_t, size_t, void *[2], uint8_t *);
void *BeginStore(struct Machine *, int64_t, size_t, void *[2], uint8_t *);
void *BeginStoreNp(struct Machine *, int64_t, size_t, void *[2], uint8_t *);
void *FindReal(struct Machine *, int64_t);
void *Load(struct Machine *, int64_t, size_t, uint8_t *);
void *LoadBuf(struct Machine *, int64_t, size_t);
void *LoadStr(struct Machine *, int64_t);
void *MallocPage(void);
void *ReserveAddress(struct Machine *, int64_t, size_t);
void *ResolveAddress(struct Machine *, int64_t);
void EndStore(struct Machine *, int64_t, size_t, void *[2], uint8_t *);
void EndStoreNp(struct Machine *, int64_t, size_t, void *[2], uint8_t *);
void ResetRam(struct Machine *);
void SetReadAddr(struct Machine *, int64_t, uint32_t);
void SetWriteAddr(struct Machine *, int64_t, uint32_t);
void VirtualRecv(struct Machine *, int64_t, void *, uint64_t);
void *VirtualSend(struct Machine *, void *, int64_t, uint64_t);
void VirtualSet(struct Machine *, int64_t, char, uint64_t);
void *RealAddress(struct Machine *, int64_t);
void VirtualSendRead(struct Machine *, void *, int64_t, uint64_t);
void VirtualRecvWrite(struct Machine *, int64_t, void *, uint64_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_MEMORY_H_ */
