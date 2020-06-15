#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long MSG_BATCH;
hidden extern const long MSG_CMSG_CLOEXEC;
hidden extern const long MSG_CONFIRM;
hidden extern const long MSG_CTRUNC;
hidden extern const long MSG_DONTROUTE;
hidden extern const long MSG_DONTWAIT;
hidden extern const long MSG_EOR;
hidden extern const long MSG_ERRQUEUE;
hidden extern const long MSG_EXCEPT;
hidden extern const long MSG_FASTOPEN;
hidden extern const long MSG_FIN;
hidden extern const long MSG_INFO;
hidden extern const long MSG_MORE;
hidden extern const long MSG_NOERROR;
hidden extern const long MSG_NOSIGNAL;
hidden extern const long MSG_OOB;
hidden extern const long MSG_PARITY_ERROR;
hidden extern const long MSG_PEEK;
hidden extern const long MSG_PROXY;
hidden extern const long MSG_RST;
hidden extern const long MSG_STAT;
hidden extern const long MSG_SYN;
hidden extern const long MSG_TRUNC;
hidden extern const long MSG_WAITALL;
hidden extern const long MSG_WAITFORONE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define MSG_BATCH SYMBOLIC(MSG_BATCH)
#define MSG_CMSG_CLOEXEC SYMBOLIC(MSG_CMSG_CLOEXEC)
#define MSG_CONFIRM SYMBOLIC(MSG_CONFIRM)
#define MSG_CTRUNC SYMBOLIC(MSG_CTRUNC)
#define MSG_DONTROUTE LITERALLY(4)
#define MSG_DONTWAIT SYMBOLIC(MSG_DONTWAIT)
#define MSG_EOR SYMBOLIC(MSG_EOR)
#define MSG_ERRQUEUE SYMBOLIC(MSG_ERRQUEUE)
#define MSG_EXCEPT SYMBOLIC(MSG_EXCEPT)
#define MSG_FASTOPEN SYMBOLIC(MSG_FASTOPEN)
#define MSG_FIN SYMBOLIC(MSG_FIN)
#define MSG_INFO SYMBOLIC(MSG_INFO)
#define MSG_MORE SYMBOLIC(MSG_MORE)
#define MSG_NOERROR SYMBOLIC(MSG_NOERROR)
#define MSG_NOSIGNAL SYMBOLIC(MSG_NOSIGNAL)
#define MSG_OOB LITERALLY(1)
#define MSG_PARITY_ERROR SYMBOLIC(MSG_PARITY_ERROR)
#define MSG_PEEK LITERALLY(2)
#define MSG_PROXY SYMBOLIC(MSG_PROXY)
#define MSG_RST SYMBOLIC(MSG_RST)
#define MSG_STAT SYMBOLIC(MSG_STAT)
#define MSG_SYN SYMBOLIC(MSG_SYN)
#define MSG_TRUNC SYMBOLIC(MSG_TRUNC)
#define MSG_WAITALL SYMBOLIC(MSG_WAITALL)
#define MSG_WAITFORONE SYMBOLIC(MSG_WAITFORONE)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_ */
