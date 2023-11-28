#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
COSMOPOLITAN_C_START_

#define EXTA B19200
#define EXTB B38400

extern const int _POSIX_VDISABLE;
extern const uint32_t BRKINT;
extern const uint32_t BS1;
extern const uint32_t BS2;
extern const uint32_t BSDLY;
extern const uint32_t CLOCAL;
extern const uint32_t CMSPAR;
extern const uint32_t CR1;
extern const uint32_t CR2;
extern const uint32_t CR3;
extern const uint32_t CRDLY;
extern const uint32_t CREAD;
extern const uint32_t CS5;
extern const uint32_t CS6;
extern const uint32_t CS7;
extern const uint32_t CS8;
extern const uint32_t CSIZE;
extern const uint32_t CSTOPB;
extern const uint32_t ECHOCTL;
extern const uint32_t ECHOE;
extern const uint32_t ECHOK;
extern const uint32_t ECHOKE;
extern const uint32_t ECHONL;
extern const uint32_t ECHOPRT;
extern const long EXTPROC;
extern const uint32_t FF1;
extern const uint32_t FF2;
extern const uint32_t FFDLY;
extern const uint32_t FLUSHO;
extern const uint32_t HUPCL;
extern const uint32_t ICANON;
extern const uint32_t ICRNL;
extern const uint32_t IEXTEN;
extern const uint32_t IGNBRK;
extern const uint32_t IGNCR;
extern const uint32_t IGNPAR;
extern const uint32_t IMAXBEL;
extern const uint32_t INLCR;
extern const uint32_t INPCK;
extern const uint32_t ISIG;
extern const uint32_t ISTRIP;
extern const uint32_t IUCLC;
extern const uint32_t IUTF8;
extern const uint32_t IXANY;
extern const uint32_t IXOFF;
extern const uint32_t IXON;
extern const uint32_t NL1;
extern const uint32_t NL2;
extern const uint32_t NL3;
extern const uint32_t NLDLY;
extern const uint32_t NOFLSH;
extern const uint32_t OCRNL;
extern const uint32_t OFDEL;
extern const uint32_t OFILL;
extern const uint32_t OLCUC;
extern const uint32_t ONLCR;
extern const uint32_t ONLRET;
extern const uint32_t ONOCR;
extern const uint32_t OPOST;
extern const uint32_t PARENB;
extern const uint32_t PARMRK;
extern const uint32_t PARODD;
extern const uint32_t PENDIN;
extern const uint32_t TAB1;
extern const uint32_t TAB2;
extern const uint32_t TAB3;
extern const uint32_t TABDLY;
extern const uint64_t TIOCCONS;
extern const uint64_t TIOCGETD;
extern const uint64_t TIOCGWINSZ;
extern const uint64_t TIOCNOTTY;
extern const uint64_t TIOCNXCL;
extern const uint64_t TIOCOUTQ;
extern const uint64_t TIOCSCTTY;
extern const uint64_t TIOCSETD;
extern const uint64_t TIOCSIG;
extern const uint64_t TIOCSPGRP;
extern const uint64_t TIOCSTI;
extern const uint64_t TIOCSWINSZ;
extern const long TOSTOP;
extern const uint8_t VDISCARD;
extern const uint8_t VEOF;
extern const uint8_t VEOL2;
extern const uint8_t VEOL;
extern const uint8_t VERASE;
extern const uint8_t VINTR;
extern const uint8_t VKILL;
extern const uint8_t VLNEXT;
extern const uint8_t VMIN;
extern const uint8_t VQUIT;
extern const uint8_t VREPRINT;
extern const uint8_t VSTART;
extern const uint8_t VSTOP;
extern const uint8_t VSUSP;
extern const uint8_t VSWTC;
extern const uint32_t VT1;
extern const uint32_t VT2;
extern const uint32_t VTDLY;
extern const uint8_t VTIME;
extern const uint8_t VWERASE;
extern const uint32_t XCASE;
extern const uint32_t XTABS;
extern const uint32_t CRTSCTS;

#define BRKINT  0x02
#define ICRNL   0x0100
#define IGNBRK  0x01
#define IGNCR   0x80
#define IGNPAR  0x04
#define IMAXBEL 0x2000
#define INLCR   0x40
#define INPCK   0x10
#define ISTRIP  0x20
#define IXANY   0x0800
#define OPOST   0x01
#define PARMRK  0x08

#define _POSIX_VDISABLE _POSIX_VDISABLE

#define NLDLY   NLDLY
#define NL0     0
#define NL1     NL1
#define NL2     NL2
#define NL3     NL3
#define CRDLY   CRDLY
#define CR0     0
#define CR1     CR1
#define CR2     CR2
#define CR3     CR3
#define TABDLY  TABDLY
#define TAB0    0
#define TAB1    TAB1
#define TAB2    TAB2
#define TAB3    TAB3
#define XTABS   XTABS
#define CRTSCTS CRTSCTS
#define BSDLY   BSDLY
#define BS0     0
#define BS1     BS1
#define BS2     BS2
#define VTDLY   VTDLY
#define VT0     0
#define VT1     VT1
#define VT2     VT2
#define FFDLY   FFDLY
#define FF0     0
#define FF1     FF1
#define FF2     FF2

#define CLOCAL   CLOCAL
#define CREAD    CREAD
#define CS5      CS5
#define CS6      CS6
#define CS7      CS7
#define CS8      CS8
#define CSIZE    CSIZE
#define CSTOPB   CSTOPB
#define ECHO     8
#define ECHOCTL  ECHOCTL
#define ECHOE    ECHOE
#define ECHOK    ECHOK
#define ECHOKE   ECHOKE
#define ECHONL   ECHONL
#define ECHOPRT  ECHOPRT
#define EXTPROC  EXTPROC
#define FLUSHO   FLUSHO
#define HUPCL    HUPCL
#define ICANON   ICANON
#define IEXTEN   IEXTEN
#define ISIG     ISIG
#define IUCLC    IUCLC
#define IUTF8    IUTF8
#define IXOFF    IXOFF
#define IXON     IXON
#define NOFLSH   NOFLSH
#define OCRNL    OCRNL
#define OFDEL    OFDEL
#define OFILL    OFILL
#define OLCUC    OLCUC
#define ONLCR    ONLCR
#define ONLRET   ONLRET
#define ONOCR    ONOCR
#define PARENB   PARENB
#define PARODD   PARODD
#define PENDIN   PENDIN
#define TOSTOP   TOSTOP
#define VDISCARD VDISCARD
#define VEOF     VEOF
#define VEOL     VEOL
#define VEOL2    VEOL2
#define VERASE   VERASE
#define VINTR    VINTR
#define VKILL    VKILL
#define VLNEXT   VLNEXT
#define VMIN     VMIN
#define VQUIT    VQUIT
#define VREPRINT VREPRINT
#define VSTART   VSTART
#define VSTOP    VSTOP
#define VSUSP    VSUSP
#define VSWTC    VSWTC
#define VTIME    VTIME
#define VWERASE  VWERASE
#define XCASE    XCASE

/* terminal ioctls */
#define TIOCGWINSZ TIOCGWINSZ /* get tty dimensions */
#define TIOCSWINSZ TIOCSWINSZ /* set tty dimensions */
#define TIOCCONS   TIOCCONS   /* redirect terminal */
#define TIOCGETD   TIOCGETD   /* get line discipline */
#define TIOCSETD   TIOCSETD   /* set line discipline */
#define TIOCNOTTY  TIOCNOTTY  /* give up terminal */
#define TIOCNXCL   TIOCNXCL   /* disable exclusive mode */
#define TIOCOUTQ   TIOCOUTQ   /* bytes in output buffer */
#define TIOCSCTTY  TIOCSCTTY  /* make controlling terminal */
#define TIOCSIG    TIOCSIG    /* generate pty signal */
#define TIOCSTI    TIOCSTI    /* insert fake tty input */

/* tcsetattr() */
#define TCSANOW   0
#define TCSAFLUSH 2
#define TCSADRAIN 1
extern const unsigned long TCGETS; /* use tcgetattr() */
extern const unsigned long TCSETS; /* use tcsetattr() */

/* tcflush() */
extern const int TCIFLUSH;
extern const int TCOFLUSH;
extern const int TCIOFLUSH;
#define TCIFLUSH  TCIFLUSH
#define TCOFLUSH  TCOFLUSH
#define TCIOFLUSH TCIOFLUSH

/* tcflow() */
#define TCOOFF 0
#define TCOON  1
#define TCIOFF 2
#define TCION  3

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_ */
