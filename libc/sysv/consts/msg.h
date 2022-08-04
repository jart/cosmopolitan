#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int MSG_BATCH;
extern const int MSG_BCAST;
extern const int MSG_CMSG_CLOEXEC;
extern const int MSG_CONFIRM;
extern const int MSG_CTRUNC;
extern const int MSG_DONTROUTE;
extern const int MSG_DONTWAIT;
extern const int MSG_EOF;
extern const int MSG_EOR;
extern const int MSG_ERRQUEUE;
extern const int MSG_EXCEPT;
extern const int MSG_FASTOPEN;
extern const int MSG_FIN;
extern const int MSG_INFO;
extern const int MSG_MCAST;
extern const int MSG_MORE;
extern const int MSG_NOERROR;
extern const int MSG_NOSIGNAL;
extern const int MSG_NOTIFICATION;
extern const int MSG_OOB;
extern const int MSG_PARITY_ERROR;
extern const int MSG_PEEK;
extern const int MSG_PROXY;
extern const int MSG_RST;
extern const int MSG_STAT;
extern const int MSG_SYN;
extern const int MSG_TRUNC;
extern const int MSG_WAITALL;
extern const int MSG_WAITFORONE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define MSG_BATCH        SYMBOLIC(MSG_BATCH)
#define MSG_BCAST        SYMBOLIC(MSG_BCAST)
#define MSG_CMSG_CLOEXEC SYMBOLIC(MSG_CMSG_CLOEXEC)
#define MSG_CONFIRM      SYMBOLIC(MSG_CONFIRM)
#define MSG_CTRUNC       SYMBOLIC(MSG_CTRUNC)
#define MSG_DONTROUTE    LITERALLY(4)
#define MSG_DONTWAIT     SYMBOLIC(MSG_DONTWAIT)
#define MSG_EOF          SYMBOLIC(MSG_EOF)
#define MSG_EOR          SYMBOLIC(MSG_EOR)
#define MSG_ERRQUEUE     SYMBOLIC(MSG_ERRQUEUE)
#define MSG_EXCEPT       SYMBOLIC(MSG_EXCEPT)
#define MSG_FASTOPEN     SYMBOLIC(MSG_FASTOPEN)
#define MSG_FIN          SYMBOLIC(MSG_FIN)
#define MSG_INFO         SYMBOLIC(MSG_INFO)
#define MSG_MCAST        SYMBOLIC(MSG_MCAST)
#define MSG_MORE         SYMBOLIC(MSG_MORE)
#define MSG_NOERROR      SYMBOLIC(MSG_NOERROR)
#define MSG_NOSIGNAL     SYMBOLIC(MSG_NOSIGNAL)
#define MSG_NOTIFICATION SYMBOLIC(MSG_NOTIFICATION)
#define MSG_OOB          LITERALLY(1)
#define MSG_PARITY_ERROR SYMBOLIC(MSG_PARITY_ERROR)
#define MSG_PEEK         LITERALLY(2)
#define MSG_PROXY        SYMBOLIC(MSG_PROXY)
#define MSG_RST          SYMBOLIC(MSG_RST)
#define MSG_STAT         SYMBOLIC(MSG_STAT)
#define MSG_SYN          SYMBOLIC(MSG_SYN)
#define MSG_TRUNC        SYMBOLIC(MSG_TRUNC)
#define MSG_WAITALL      SYMBOLIC(MSG_WAITALL)
#define MSG_WAITFORONE   SYMBOLIC(MSG_WAITFORONE)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_ */
