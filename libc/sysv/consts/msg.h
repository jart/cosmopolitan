#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_
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

#define MSG_OOB       1
#define MSG_PEEK      2
#define MSG_DONTROUTE 4
#define MSG_DONTWAIT  MSG_DONTWAIT
#define MSG_FASTOPEN  MSG_FASTOPEN
#define MSG_WAITALL   MSG_WAITALL
#define MSG_TRUNC     MSG_TRUNC
#define MSG_CTRUNC    MSG_CTRUNC

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MSG_H_ */
