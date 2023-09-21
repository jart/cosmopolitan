#ifndef COSMOPOLITAN_LIBC_INTRIN_NOMULTICS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_NOMULTICS_INTERNAL_H_
#include "libc/calls/struct/timespec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern unsigned char __replmode;
extern unsigned char __replstderr;
extern unsigned char __ttymagic;
extern unsigned char __veof;
extern unsigned char __vintr;
extern unsigned char __vquit;
extern unsigned char __vtime;
extern unsigned char __mousebuttons;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_NOMULTICS_INTERNAL_H_ */
