#ifndef COSMOPOLITAN_LIBC_TESTLIB_HYPERION_H_
#define COSMOPOLITAN_LIBC_TESTLIB_HYPERION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern size_t kHyperionSize;
extern char kHyperion[];
extern uint8_t kHyperionZip[];

extern size_t kMobySize;
extern char kMoby[];
extern uint8_t kMobyZip[];

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TESTLIB_HYPERION_H_ */
