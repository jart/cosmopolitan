#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LC_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LC_H_
#include "libc/runtime/symbolic.h"

#define LC_ALL SYMBOLIC(LC_ALL)
#define LC_ALL_MASK SYMBOLIC(LC_ALL_MASK)
#define LC_COLLATE SYMBOLIC(LC_COLLATE)
#define LC_COLLATE_MASK SYMBOLIC(LC_COLLATE_MASK)
#define LC_CTYPE SYMBOLIC(LC_CTYPE)
#define LC_CTYPE_MASK SYMBOLIC(LC_CTYPE_MASK)
#define LC_MESSAGES SYMBOLIC(LC_MESSAGES)
#define LC_MESSAGES_MASK SYMBOLIC(LC_MESSAGES_MASK)
#define LC_MONETARY SYMBOLIC(LC_MONETARY)
#define LC_MONETARY_MASK SYMBOLIC(LC_MONETARY_MASK)
#define LC_NUMERIC SYMBOLIC(LC_NUMERIC)
#define LC_NUMERIC_MASK SYMBOLIC(LC_NUMERIC_MASK)
#define LC_TIME SYMBOLIC(LC_TIME)
#define LC_TIME_MASK SYMBOLIC(LC_TIME_MASK)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long LC_ALL;
hidden extern const long LC_ALL_MASK;
hidden extern const long LC_COLLATE;
hidden extern const long LC_COLLATE_MASK;
hidden extern const long LC_CTYPE;
hidden extern const long LC_CTYPE_MASK;
hidden extern const long LC_MESSAGES;
hidden extern const long LC_MESSAGES_MASK;
hidden extern const long LC_MONETARY;
hidden extern const long LC_MONETARY_MASK;
hidden extern const long LC_NUMERIC;
hidden extern const long LC_NUMERIC_MASK;
hidden extern const long LC_TIME;
hidden extern const long LC_TIME_MASK;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LC_H_ */
