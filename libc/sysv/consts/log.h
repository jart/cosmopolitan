#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LOG_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LOG_H_

#define LOG_MASK(pri) (1 << (pri))
#define LOG_UPTO(pri) ((1 << ((pri) + 1)) - 1)

#define LOG_EMERG   0
#define LOG_ALERT   1
#define LOG_CRIT    2
#define LOG_ERR     3
#define LOG_WARNING 4
#define LOG_NOTICE  5
#define LOG_INFO    6
#define LOG_DEBUG   7

#define LOG_KERN     0000
#define LOG_USER     0010
#define LOG_MAIL     0020
#define LOG_DAEMON   0030
#define LOG_AUTH     0040
#define LOG_SYSLOG   0050
#define LOG_LPR      0060
#define LOG_NEWS     0070
#define LOG_UUCP     0100
#define LOG_CRON     0110
#define LOG_AUTHPRIV 0120
#define LOG_FTP      0130

#define LOG_PID    1
#define LOG_CONS   2
#define LOG_ODELAY 4
#define LOG_NDELAY 8
#define LOG_NOWAIT 16
#define LOG_PERROR 32

#define LOG_LOCAL0 128
#define LOG_LOCAL1 136
#define LOG_LOCAL2 144
#define LOG_LOCAL3 152
#define LOG_LOCAL4 160
#define LOG_LOCAL5 168
#define LOG_LOCAL6 176
#define LOG_LOCAL7 184

#define LOG_NFACILITIES 24
#define LOG_FACMASK     0x03f8
#define LOG_FAC(p)      ((LOG_FACMASK & (p)) >> 3)

#define LOG_PRIMASK       7
#define LOG_PRI(p)        (LOG_PRIMASK & (p))
#define LOG_MAKEPRI(f, p) (((f) << 3) | (p))

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LOG_H_ */
