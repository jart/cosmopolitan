#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_
COSMOPOLITAN_C_START_

extern const uint64_t TIOCMGET;
extern const uint64_t TIOCMSET;
extern const uint64_t TIOCMBIC;
extern const uint64_t TIOCMBIS;

extern const int TIOCM_CAR;
extern const int TIOCM_CD;
extern const int TIOCM_CTS;
extern const int TIOCM_DSR;
extern const int TIOCM_DTR;
extern const int TIOCM_LE;
extern const int TIOCM_RI;
extern const int TIOCM_RNG;
extern const int TIOCM_RTS;
extern const int TIOCM_SR;
extern const int TIOCM_ST;

#define TIOCM_LE  0x01
#define TIOCM_DTR 0x02
#define TIOCM_RTS 0x04
#define TIOCM_ST  0x08
#define TIOCM_SR  0x10
#define TIOCM_CTS 0x20
#define TIOCM_CAR 0x40
#define TIOCM_CD  0x40
#define TIOCM_RI  0x80
#define TIOCM_RNG 0x80
#define TIOCM_DSR 0x0100

#define TIOCMGET TIOCMGET
#define TIOCMSET TIOCMSET
#define TIOCMBIC TIOCMBIC
#define TIOCMBIS TIOCMBIS


COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MODEM_H_ */
