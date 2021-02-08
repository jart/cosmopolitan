#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_TYPE_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_TYPE_H_
#include "libc/runtime/symbolic.h"

#define TYPE_A SYMBOLIC(TYPE_A)
#define TYPE_DISK SYMBOLIC(TYPE_DISK)
#define TYPE_E SYMBOLIC(TYPE_E)
#define TYPE_ENCLOSURE SYMBOLIC(TYPE_ENCLOSURE)
#define TYPE_I SYMBOLIC(TYPE_I)
#define TYPE_L SYMBOLIC(TYPE_L)
#define TYPE_MEDIUM_CHANGER SYMBOLIC(TYPE_MEDIUM_CHANGER)
#define TYPE_MOD SYMBOLIC(TYPE_MOD)
#define TYPE_NO_LUN SYMBOLIC(TYPE_NO_LUN)
#define TYPE_PROCESSOR SYMBOLIC(TYPE_PROCESSOR)
#define TYPE_ROM SYMBOLIC(TYPE_ROM)
#define TYPE_SCANNER SYMBOLIC(TYPE_SCANNER)
#define TYPE_TAPE SYMBOLIC(TYPE_TAPE)
#define TYPE_WORM SYMBOLIC(TYPE_WORM)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long TYPE_A;
extern const long TYPE_DISK;
extern const long TYPE_E;
extern const long TYPE_ENCLOSURE;
extern const long TYPE_I;
extern const long TYPE_L;
extern const long TYPE_MEDIUM_CHANGER;
extern const long TYPE_MOD;
extern const long TYPE_NO_LUN;
extern const long TYPE_PROCESSOR;
extern const long TYPE_ROM;
extern const long TYPE_SCANNER;
extern const long TYPE_TAPE;
extern const long TYPE_WORM;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_TYPE_H_ */
