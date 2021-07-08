#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_TCP_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_TCP_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long TCP_CC_INFO;
extern const long TCP_CONGESTION;
extern const long TCP_COOKIE_TRANSACTIONS;
extern const long TCP_CORK;
extern const long TCP_DEFER_ACCEPT;
extern const long TCP_FASTOPEN;
extern const long TCP_FASTOPEN_CONNECT;
extern const long TCP_INFO;
extern const long TCP_KEEPCNT;
extern const long TCP_KEEPIDLE;
extern const long TCP_KEEPINTVL;
extern const long TCP_LINGER2;
extern const long TCP_MAXSEG;
extern const long TCP_MD5SIG;
extern const long TCP_MD5SIG_MAXKEYLEN;
extern const long TCP_NODELAY;
extern const long TCP_NOTSENT_LOWAT;
extern const long TCP_QUEUE_SEQ;
extern const long TCP_QUICKACK;
extern const long TCP_REPAIR;
extern const long TCP_REPAIR_OPTIONS;
extern const long TCP_REPAIR_QUEUE;
extern const long TCP_SAVED_SYN;
extern const long TCP_SAVE_SYN;
extern const long TCP_SYNCNT;
extern const long TCP_THIN_DUPACK;
extern const long TCP_THIN_LINEAR_TIMEOUTS;
extern const long TCP_TIMESTAMP;
extern const long TCP_ULP;
extern const long TCP_USER_TIMEOUT;
extern const long TCP_WINDOW_CLAMP;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define TCP_CC_INFO              SYMBOLIC(TCP_CC_INFO)
#define TCP_CONGESTION           SYMBOLIC(TCP_CONGESTION)
#define TCP_COOKIE_TRANSACTIONS  SYMBOLIC(TCP_COOKIE_TRANSACTIONS)
#define TCP_CORK                 SYMBOLIC(TCP_CORK)
#define TCP_DEFER_ACCEPT         SYMBOLIC(TCP_DEFER_ACCEPT)
#define TCP_FASTOPEN             SYMBOLIC(TCP_FASTOPEN)
#define TCP_FASTOPEN_CONNECT     SYMBOLIC(TCP_FASTOPEN_CONNECT)
#define TCP_INFO                 SYMBOLIC(TCP_INFO)
#define TCP_KEEPCNT              SYMBOLIC(TCP_KEEPCNT)
#define TCP_KEEPIDLE             SYMBOLIC(TCP_KEEPIDLE)
#define TCP_KEEPINTVL            SYMBOLIC(TCP_KEEPINTVL)
#define TCP_LINGER2              SYMBOLIC(TCP_LINGER2)
#define TCP_MAXSEG               SYMBOLIC(TCP_MAXSEG)
#define TCP_MD5SIG               SYMBOLIC(TCP_MD5SIG)
#define TCP_MD5SIG_MAXKEYLEN     SYMBOLIC(TCP_MD5SIG_MAXKEYLEN)
#define TCP_NODELAY              LITERALLY(1)
#define TCP_NOTSENT_LOWAT        SYMBOLIC(TCP_NOTSENT_LOWAT)
#define TCP_QUEUE_SEQ            SYMBOLIC(TCP_QUEUE_SEQ)
#define TCP_QUICKACK             SYMBOLIC(TCP_QUICKACK)
#define TCP_REPAIR               SYMBOLIC(TCP_REPAIR)
#define TCP_REPAIR_OPTIONS       SYMBOLIC(TCP_REPAIR_OPTIONS)
#define TCP_REPAIR_QUEUE         SYMBOLIC(TCP_REPAIR_QUEUE)
#define TCP_SAVED_SYN            SYMBOLIC(TCP_SAVED_SYN)
#define TCP_SAVE_SYN             SYMBOLIC(TCP_SAVE_SYN)
#define TCP_SYNCNT               SYMBOLIC(TCP_SYNCNT)
#define TCP_THIN_DUPACK          SYMBOLIC(TCP_THIN_DUPACK)
#define TCP_THIN_LINEAR_TIMEOUTS SYMBOLIC(TCP_THIN_LINEAR_TIMEOUTS)
#define TCP_TIMESTAMP            SYMBOLIC(TCP_TIMESTAMP)
#define TCP_ULP                  SYMBOLIC(TCP_ULP)
#define TCP_USER_TIMEOUT         SYMBOLIC(TCP_USER_TIMEOUT)
#define TCP_WINDOW_CLAMP         SYMBOLIC(TCP_WINDOW_CLAMP)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_TCP_H_ */
