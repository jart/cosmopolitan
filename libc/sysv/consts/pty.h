#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_PTY_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_PTY_H_
COSMOPOLITAN_C_START_

extern const int TIOCPKT;
extern const int TIOCPKT_DATA;
extern const int TIOCPKT_DOSTOP;
extern const int TIOCPKT_FLUSHREAD;
extern const int TIOCPKT_FLUSHWRITE;
extern const int TIOCPKT_IOCTL;
extern const int TIOCPKT_NOSTOP;
extern const int TIOCPKT_START;
extern const int TIOCPKT_STOP;

#define TIOCPKT_DATA       0x00
#define TIOCPKT_DOSTOP     0x01
#define TIOCPKT_FLUSHREAD  0x02
#define TIOCPKT_FLUSHWRITE 0x04
#define TIOCPKT_IOCTL      0x08
#define TIOCPKT_NOSTOP     0x10
#define TIOCPKT_START      0x20
#define TIOCPKT_STOP       0x40

#define TIOCPKT TIOCPKT


COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_PTY_H_ */
