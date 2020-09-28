#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_SSE_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_SSE_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void Op171(struct Machine *, uint32_t);
void Op172(struct Machine *, uint32_t);
void Op173(struct Machine *, uint32_t);
void OpSsePabsb(struct Machine *, uint32_t);
void OpSsePabsd(struct Machine *, uint32_t);
void OpSsePabsw(struct Machine *, uint32_t);
void OpSsePackssdw(struct Machine *, uint32_t);
void OpSsePacksswb(struct Machine *, uint32_t);
void OpSsePackuswb(struct Machine *, uint32_t);
void OpSsePaddb(struct Machine *, uint32_t);
void OpSsePaddd(struct Machine *, uint32_t);
void OpSsePaddq(struct Machine *, uint32_t);
void OpSsePaddsb(struct Machine *, uint32_t);
void OpSsePaddsw(struct Machine *, uint32_t);
void OpSsePaddusb(struct Machine *, uint32_t);
void OpSsePaddusw(struct Machine *, uint32_t);
void OpSsePaddw(struct Machine *, uint32_t);
void OpSsePalignr(struct Machine *, uint32_t);
void OpSsePand(struct Machine *, uint32_t);
void OpSsePandn(struct Machine *, uint32_t);
void OpSsePavgb(struct Machine *, uint32_t);
void OpSsePavgw(struct Machine *, uint32_t);
void OpSsePcmpeqb(struct Machine *, uint32_t);
void OpSsePcmpeqd(struct Machine *, uint32_t);
void OpSsePcmpeqw(struct Machine *, uint32_t);
void OpSsePcmpgtb(struct Machine *, uint32_t);
void OpSsePcmpgtd(struct Machine *, uint32_t);
void OpSsePcmpgtw(struct Machine *, uint32_t);
void OpSsePhaddd(struct Machine *, uint32_t);
void OpSsePhaddsw(struct Machine *, uint32_t);
void OpSsePhaddw(struct Machine *, uint32_t);
void OpSsePhsubd(struct Machine *, uint32_t);
void OpSsePhsubsw(struct Machine *, uint32_t);
void OpSsePhsubw(struct Machine *, uint32_t);
void OpSsePmaddubsw(struct Machine *, uint32_t);
void OpSsePmaddwd(struct Machine *, uint32_t);
void OpSsePmaxsw(struct Machine *, uint32_t);
void OpSsePmaxub(struct Machine *, uint32_t);
void OpSsePminsw(struct Machine *, uint32_t);
void OpSsePminub(struct Machine *, uint32_t);
void OpSsePmulhrsw(struct Machine *, uint32_t);
void OpSsePmulhuw(struct Machine *, uint32_t);
void OpSsePmulhw(struct Machine *, uint32_t);
void OpSsePmulld(struct Machine *, uint32_t);
void OpSsePmullw(struct Machine *, uint32_t);
void OpSsePmuludq(struct Machine *, uint32_t);
void OpSsePor(struct Machine *, uint32_t);
void OpSsePsadbw(struct Machine *, uint32_t);
void OpSsePshufb(struct Machine *, uint32_t);
void OpSsePsignb(struct Machine *, uint32_t);
void OpSsePsignd(struct Machine *, uint32_t);
void OpSsePsignw(struct Machine *, uint32_t);
void OpSsePslldv(struct Machine *, uint32_t);
void OpSsePsllqv(struct Machine *, uint32_t);
void OpSsePsllwv(struct Machine *, uint32_t);
void OpSsePsradv(struct Machine *, uint32_t);
void OpSsePsrawv(struct Machine *, uint32_t);
void OpSsePsrldv(struct Machine *, uint32_t);
void OpSsePsrlqv(struct Machine *, uint32_t);
void OpSsePsrlwv(struct Machine *, uint32_t);
void OpSsePsubb(struct Machine *, uint32_t);
void OpSsePsubd(struct Machine *, uint32_t);
void OpSsePsubq(struct Machine *, uint32_t);
void OpSsePsubsb(struct Machine *, uint32_t);
void OpSsePsubsw(struct Machine *, uint32_t);
void OpSsePsubusb(struct Machine *, uint32_t);
void OpSsePsubusw(struct Machine *, uint32_t);
void OpSsePsubw(struct Machine *, uint32_t);
void OpSsePunpckhbw(struct Machine *, uint32_t);
void OpSsePunpckhdq(struct Machine *, uint32_t);
void OpSsePunpckhqdq(struct Machine *, uint32_t);
void OpSsePunpckhwd(struct Machine *, uint32_t);
void OpSsePunpcklbw(struct Machine *, uint32_t);
void OpSsePunpckldq(struct Machine *, uint32_t);
void OpSsePunpcklqdq(struct Machine *, uint32_t);
void OpSsePunpcklwd(struct Machine *, uint32_t);
void OpSsePxor(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_SSE_H_ */
