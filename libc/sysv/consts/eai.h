#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_EAI_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_EAI_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long EAI_ADDRFAMILY;
hidden extern const long EAI_AGAIN;
hidden extern const long EAI_ALLDONE;
hidden extern const long EAI_BADFLAGS;
hidden extern const long EAI_CANCELED;
hidden extern const long EAI_FAIL;
hidden extern const long EAI_FAMILY;
hidden extern const long EAI_IDN_ENCODE;
hidden extern const long EAI_INPROGRESS;
hidden extern const long EAI_INTR;
hidden extern const long EAI_MEMORY;
hidden extern const long EAI_NODATA;
hidden extern const long EAI_NONAME;
hidden extern const long EAI_NOTCANCELED;
hidden extern const long EAI_OVERFLOW;
hidden extern const long EAI_SERVICE;
hidden extern const long EAI_SOCKTYPE;
hidden extern const long EAI_SYSTEM;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define EAI_ADDRFAMILY SYMBOLIC(EAI_ADDRFAMILY)
#define EAI_AGAIN SYMBOLIC(EAI_AGAIN)
#define EAI_ALLDONE SYMBOLIC(EAI_ALLDONE)
#define EAI_BADFLAGS SYMBOLIC(EAI_BADFLAGS)
#define EAI_CANCELED SYMBOLIC(EAI_CANCELED)
#define EAI_FAIL SYMBOLIC(EAI_FAIL)
#define EAI_FAMILY SYMBOLIC(EAI_FAMILY)
#define EAI_IDN_ENCODE SYMBOLIC(EAI_IDN_ENCODE)
#define EAI_INPROGRESS SYMBOLIC(EAI_INPROGRESS)
#define EAI_INTR SYMBOLIC(EAI_INTR)
#define EAI_MEMORY SYMBOLIC(EAI_MEMORY)
#define EAI_NODATA SYMBOLIC(EAI_NODATA)
#define EAI_NONAME SYMBOLIC(EAI_NONAME)
#define EAI_NOTCANCELED SYMBOLIC(EAI_NOTCANCELED)
#define EAI_OVERFLOW SYMBOLIC(EAI_OVERFLOW)
#define EAI_SERVICE SYMBOLIC(EAI_SERVICE)
#define EAI_SOCKTYPE SYMBOLIC(EAI_SOCKTYPE)
#define EAI_SUCCESS LITERALLY(0)
#define EAI_SYSTEM SYMBOLIC(EAI_SYSTEM)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_EAI_H_ */
