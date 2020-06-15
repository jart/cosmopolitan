#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LOG_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LOG_H_
#include "libc/runtime/symbolic.h"

#define LOG_ALERT SYMBOLIC(LOG_ALERT)
#define LOG_AUTH SYMBOLIC(LOG_AUTH)
#define LOG_CONS SYMBOLIC(LOG_CONS)
#define LOG_CRIT SYMBOLIC(LOG_CRIT)
#define LOG_CRON SYMBOLIC(LOG_CRON)
#define LOG_DAEMON SYMBOLIC(LOG_DAEMON)
#define LOG_DEBUG SYMBOLIC(LOG_DEBUG)
#define LOG_EMERG SYMBOLIC(LOG_EMERG)
#define LOG_ERR SYMBOLIC(LOG_ERR)
#define LOG_FACMASK SYMBOLIC(LOG_FACMASK)
#define LOG_INFO SYMBOLIC(LOG_INFO)
#define LOG_KERN SYMBOLIC(LOG_KERN)
#define LOG_LOCAL0 SYMBOLIC(LOG_LOCAL0)
#define LOG_LOCAL1 SYMBOLIC(LOG_LOCAL1)
#define LOG_LOCAL2 SYMBOLIC(LOG_LOCAL2)
#define LOG_LOCAL3 SYMBOLIC(LOG_LOCAL3)
#define LOG_LOCAL4 SYMBOLIC(LOG_LOCAL4)
#define LOG_LOCAL5 SYMBOLIC(LOG_LOCAL5)
#define LOG_LOCAL6 SYMBOLIC(LOG_LOCAL6)
#define LOG_LOCAL7 SYMBOLIC(LOG_LOCAL7)
#define LOG_LPR SYMBOLIC(LOG_LPR)
#define LOG_MAIL SYMBOLIC(LOG_MAIL)
#define LOG_NDELAY SYMBOLIC(LOG_NDELAY)
#define LOG_NEWS SYMBOLIC(LOG_NEWS)
#define LOG_NFACILITIES SYMBOLIC(LOG_NFACILITIES)
#define LOG_NOTICE SYMBOLIC(LOG_NOTICE)
#define LOG_NOWAIT SYMBOLIC(LOG_NOWAIT)
#define LOG_ODELAY SYMBOLIC(LOG_ODELAY)
#define LOG_PERROR SYMBOLIC(LOG_PERROR)
#define LOG_PID SYMBOLIC(LOG_PID)
#define LOG_PRIMASK SYMBOLIC(LOG_PRIMASK)
#define LOG_SELECT SYMBOLIC(LOG_SELECT)
#define LOG_SENSE SYMBOLIC(LOG_SENSE)
#define LOG_SYSLOG SYMBOLIC(LOG_SYSLOG)
#define LOG_USER SYMBOLIC(LOG_USER)
#define LOG_UUCP SYMBOLIC(LOG_UUCP)
#define LOG_WARNING SYMBOLIC(LOG_WARNING)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long LOG_ALERT;
hidden extern const long LOG_AUTH;
hidden extern const long LOG_CONS;
hidden extern const long LOG_CRIT;
hidden extern const long LOG_CRON;
hidden extern const long LOG_DAEMON;
hidden extern const long LOG_DEBUG;
hidden extern const long LOG_EMERG;
hidden extern const long LOG_ERR;
hidden extern const long LOG_FACMASK;
hidden extern const long LOG_INFO;
hidden extern const long LOG_KERN;
hidden extern const long LOG_LOCAL0;
hidden extern const long LOG_LOCAL1;
hidden extern const long LOG_LOCAL2;
hidden extern const long LOG_LOCAL3;
hidden extern const long LOG_LOCAL4;
hidden extern const long LOG_LOCAL5;
hidden extern const long LOG_LOCAL6;
hidden extern const long LOG_LOCAL7;
hidden extern const long LOG_LPR;
hidden extern const long LOG_MAIL;
hidden extern const long LOG_NDELAY;
hidden extern const long LOG_NEWS;
hidden extern const long LOG_NFACILITIES;
hidden extern const long LOG_NOTICE;
hidden extern const long LOG_NOWAIT;
hidden extern const long LOG_ODELAY;
hidden extern const long LOG_PERROR;
hidden extern const long LOG_PID;
hidden extern const long LOG_PRIMASK;
hidden extern const long LOG_SELECT;
hidden extern const long LOG_SENSE;
hidden extern const long LOG_SYSLOG;
hidden extern const long LOG_USER;
hidden extern const long LOG_UUCP;
hidden extern const long LOG_WARNING;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LOG_H_ */
