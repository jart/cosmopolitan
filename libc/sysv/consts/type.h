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

hidden extern const long TYPE_A;
hidden extern const long TYPE_DISK;
hidden extern const long TYPE_E;
hidden extern const long TYPE_ENCLOSURE;
hidden extern const long TYPE_I;
hidden extern const long TYPE_L;
hidden extern const long TYPE_MEDIUM_CHANGER;
hidden extern const long TYPE_MOD;
hidden extern const long TYPE_NO_LUN;
hidden extern const long TYPE_PROCESSOR;
hidden extern const long TYPE_ROM;
hidden extern const long TYPE_SCANNER;
hidden extern const long TYPE_TAPE;
hidden extern const long TYPE_WORM;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_TYPE_H_ */
