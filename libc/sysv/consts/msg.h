#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_
COSMOPOLITAN_C_START_

extern const int MSG_DONTWAIT;
extern const int MSG_WAITALL;
extern const int MSG_NOSIGNAL;
extern const int MSG_TRUNC;
extern const int MSG_CTRUNC;
extern const int MSG_FASTOPEN; /* linux only */

#define MSG_OOB       1
#define MSG_PEEK      2
#define MSG_DONTROUTE 4
#define MSG_DONTWAIT  MSG_DONTWAIT
#define MSG_NOSIGNAL  MSG_NOSIGNAL
#define MSG_WAITALL   MSG_WAITALL
#define MSG_TRUNC     MSG_TRUNC
#define MSG_CTRUNC    MSG_CTRUNC

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_ */
