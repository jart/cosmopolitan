#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const uint32_t BRKINT;
extern const uint32_t BS0;
extern const uint32_t BS1;
extern const uint32_t BS2;
extern const uint32_t BSDLY;
extern const long BUSY;
extern const uint32_t CBAUD;
extern const uint32_t CBAUDEX;
extern const uint32_t CIBAUD;
extern const uint32_t CLOCAL;
extern const uint32_t CMSPAR;
extern const uint32_t CR0;
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
extern const uint32_t ECHO;
extern const uint32_t ECHOCTL;
extern const uint32_t ECHOE;
extern const uint32_t ECHOK;
extern const uint32_t ECHOKE;
extern const uint32_t ECHONL;
extern const uint32_t ECHOPRT;
extern const long ENDRUNDISC;
extern const long EXTA;
extern const long EXTB;
extern const long EXTPROC;
extern const uint32_t FF0;
extern const uint32_t FF1;
extern const uint32_t FF2;
extern const uint32_t FFDLY;
extern const uint32_t FLUSHO;
extern const long H4DISC;
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
extern const long NETGRAPHDISC;
extern const uint32_t NL0;
extern const uint32_t NL1;
extern const uint32_t NL2;
extern const uint32_t NL3;
extern const uint32_t NLDLY;
extern const long NMEADISC;
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
extern const long PPPDISC;
extern const long SLIPDISC;
extern const long STRIPDISC;
extern const uint32_t TAB0;
extern const uint32_t TAB1;
extern const uint32_t TAB2;
extern const uint32_t TAB3;
extern const uint32_t TABDLY;
extern const long TABLDISC;
extern const uint64_t TCFLSH;
extern const uint64_t TCGETS;
extern const int TCIFLUSH;
extern const int TCIOFF;
extern const int TCIOFLUSH;
extern const int TCION;
extern const int TCOFLUSH;
extern const int TCOOFF;
extern const int TCOON;
extern const int TCSADRAIN;
extern const int TCSAFLUSH;
extern const int TCSANOW;
extern const uint64_t TCSBRK;
extern const uint64_t TCSETS;
extern const uint64_t TCSETSF;
extern const uint64_t TCSETSW;
extern const uint64_t TCXONC;
extern const uint64_t TIOCCBRK;
extern const uint64_t TIOCCDTR;
extern const uint64_t TIOCCHKVERAUTH;
extern const uint64_t TIOCCONS;
extern const uint64_t TIOCEXT;
extern const uint64_t TIOCFLAG_CLOCAL;
extern const uint64_t TIOCFLAG_MDMBUF;
extern const uint64_t TIOCFLAG_PPS;
extern const uint64_t TIOCFLAG_SOFTCAR;
extern const uint64_t TIOCFLUSH;
extern const uint64_t TIOCGDRAINWAIT;
extern const uint64_t TIOCGETD;
extern const uint64_t TIOCGFLAGS;
extern const uint64_t TIOCGPGRP;
extern const uint64_t TIOCGPTN;
extern const uint64_t TIOCGSID;
extern const uint64_t TIOCGTSTAMP;
extern const uint64_t TIOCGWINSZ;
extern const uint64_t TIOCNOTTY;
extern const uint64_t TIOCNXCL;
extern const uint64_t TIOCOUTQ;
extern const uint64_t TIOCPTMASTER;
extern const uint64_t TIOCREMOTE;
extern const uint64_t TIOCSBRK;
extern const uint64_t TIOCSCTTY;
extern const uint64_t TIOCSDRAINWAIT;
extern const uint64_t TIOCSDTR;
extern const uint64_t TIOCSERGETLSR;
extern const uint64_t TIOCSERGETMULTI;
extern const uint64_t TIOCSERSETMULTI;
extern const uint64_t TIOCSER_TEMT;
extern const uint64_t TIOCSETD;
extern const uint64_t TIOCSETVERAUTH;
extern const uint64_t TIOCSFLAGS;
extern const uint64_t TIOCSIG;
extern const uint64_t TIOCSPGRP;
extern const uint64_t TIOCSTART;
extern const uint64_t TIOCSTAT;
extern const uint64_t TIOCSTI;
extern const uint64_t TIOCSTOP;
extern const uint64_t TIOCSTSTAMP;
extern const uint64_t TIOCSWINSZ;
extern const uint64_t TIOCTIMESTAMP;
extern const uint64_t TIOCUCNTL_CBRK;
extern const long TOSTOP;
extern const long TTYDISC;
extern const uint8_t VDISCARD;
extern const uint8_t VEOF;
extern const uint8_t VEOL2;
extern const uint8_t VEOL;
extern const uint8_t VERASE;
extern const long VERIFY;
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
extern const uint32_t VT0;
extern const uint32_t VT1;
extern const uint32_t VT2;
extern const uint32_t VTDLY;
extern const uint8_t VTIME;
extern const uint8_t VWERASE;
extern const uint32_t XCASE;
extern const uint32_t XTABS;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define BRKINT    LITERALLY(0x02)
#define ICRNL     LITERALLY(0x0100)
#define IGNBRK    LITERALLY(0x01)
#define IGNCR     LITERALLY(0x80)
#define IGNPAR    LITERALLY(0x04)
#define IMAXBEL   LITERALLY(0x2000)
#define INLCR     LITERALLY(0x40)
#define INPCK     LITERALLY(0x10)
#define ISTRIP    LITERALLY(0x20)
#define IXANY     LITERALLY(0x0800)
#define OPOST     LITERALLY(0x01)
#define PARMRK    LITERALLY(0x08)
#define TIOCSTART SYMBOLIC(TIOCSTART)
#define TIOCSTOP  SYMBOLIC(TIOCSTOP)

#define NLDLY  SYMBOLIC(NLDLY)
#define NL0    LITERALLY(0)
#define NL1    SYMBOLIC(NL1)
#define NL2    SYMBOLIC(NL2)
#define NL3    SYMBOLIC(NL3)
#define CRDLY  SYMBOLIC(CRDLY)
#define CR0    LITERALLY(0)
#define CR1    SYMBOLIC(CR1)
#define CR2    SYMBOLIC(CR2)
#define CR3    SYMBOLIC(CR3)
#define TABDLY SYMBOLIC(TABDLY)
#define TAB0   LITERALLY(0)
#define TAB1   SYMBOLIC(TAB1)
#define TAB2   SYMBOLIC(TAB2)
#define TAB3   SYMBOLIC(TAB3)
#define XTABS  SYMBOLIC(XTABS)
#define BSDLY  SYMBOLIC(BSDLY)
#define BS0    LITERALLY(0)
#define BS1    SYMBOLIC(BS1)
#define BS2    SYMBOLIC(BS2)
#define VTDLY  SYMBOLIC(VTDLY)
#define VT0    LITERALLY(0)
#define VT1    SYMBOLIC(VT1)
#define VT2    SYMBOLIC(VT2)
#define FFDLY  SYMBOLIC(FFDLY)
#define FF0    LITERALLY(0)
#define FF1    SYMBOLIC(FF1)
#define FF2    SYMBOLIC(FF2)

#define BUSY             SYMBOLIC(BUSY)
#define CBAUD            SYMBOLIC(CBAUD)
#define CBAUDEX          SYMBOLIC(CBAUDEX)
#define CIBAUD           SYMBOLIC(CIBAUD)
#define CLOCAL           SYMBOLIC(CLOCAL)
#define CMSPAR           SYMBOLIC(CMSPAR)
#define CREAD            SYMBOLIC(CREAD)
#define CS5              SYMBOLIC(CS5)
#define CS6              SYMBOLIC(CS6)
#define CS7              SYMBOLIC(CS7)
#define CS8              SYMBOLIC(CS8)
#define CSIZE            SYMBOLIC(CSIZE)
#define CSTOPB           SYMBOLIC(CSTOPB)
#define ECHO             LITERALLY(8)
#define ECHOCTL          SYMBOLIC(ECHOCTL)
#define ECHOE            SYMBOLIC(ECHOE)
#define ECHOK            SYMBOLIC(ECHOK)
#define ECHOKE           SYMBOLIC(ECHOKE)
#define ECHONL           SYMBOLIC(ECHONL)
#define ECHOPRT          SYMBOLIC(ECHOPRT)
#define ENDRUNDISC       SYMBOLIC(ENDRUNDISC)
#define EXTA             SYMBOLIC(EXTA)
#define EXTB             SYMBOLIC(EXTB)
#define EXTPROC          SYMBOLIC(EXTPROC)
#define FLUSHO           SYMBOLIC(FLUSHO)
#define H4DISC           SYMBOLIC(H4DISC)
#define HUPCL            SYMBOLIC(HUPCL)
#define ICANON           SYMBOLIC(ICANON)
#define IEXTEN           SYMBOLIC(IEXTEN)
#define ISIG             SYMBOLIC(ISIG)
#define IUCLC            SYMBOLIC(IUCLC)
#define IUTF8            SYMBOLIC(IUTF8)
#define IXOFF            SYMBOLIC(IXOFF)
#define IXON             SYMBOLIC(IXON)
#define NCCS             LITERALLY(20)
#define NETGRAPHDISC     SYMBOLIC(NETGRAPHDISC)
#define NMEADISC         SYMBOLIC(NMEADISC)
#define NOFLSH           SYMBOLIC(NOFLSH)
#define OCRNL            SYMBOLIC(OCRNL)
#define OFDEL            SYMBOLIC(OFDEL)
#define OFILL            SYMBOLIC(OFILL)
#define OLCUC            SYMBOLIC(OLCUC)
#define ONLCR            SYMBOLIC(ONLCR)
#define ONLRET           SYMBOLIC(ONLRET)
#define ONOCR            SYMBOLIC(ONOCR)
#define PARENB           SYMBOLIC(PARENB)
#define PARODD           SYMBOLIC(PARODD)
#define PENDIN           SYMBOLIC(PENDIN)
#define PPPDISC          SYMBOLIC(PPPDISC)
#define SLIPDISC         SYMBOLIC(SLIPDISC)
#define STRIPDISC        SYMBOLIC(STRIPDISC)
#define TABLDISC         SYMBOLIC(TABLDISC)
#define TCFLSH           SYMBOLIC(TCFLSH)
#define TCFLSH           SYMBOLIC(TCFLSH)
#define TCGETS           SYMBOLIC(TCGETS)
#define TCIFLUSH         SYMBOLIC(TCIFLUSH)
#define TCIOFF           SYMBOLIC(TCIOFF)
#define TCIOFLUSH        SYMBOLIC(TCIOFLUSH)
#define TCION            SYMBOLIC(TCION)
#define TCOFLUSH         SYMBOLIC(TCOFLUSH)
#define TCOOFF           SYMBOLIC(TCOOFF)
#define TCOON            SYMBOLIC(TCOON)
#define TCSADRAIN        LITERALLY(1)
#define TCSAFLUSH        LITERALLY(2)
#define TCSANOW          LITERALLY(0)
#define TCSBRK           SYMBOLIC(TCSBRK)
#define TCSETS           SYMBOLIC(TCSETS)
#define TCSETSF          SYMBOLIC(TCSETSF)
#define TCSETSW          SYMBOLIC(TCSETSW)
#define TCXONC           SYMBOLIC(TCXONC)
#define TIOCCBRK         SYMBOLIC(TIOCCBRK)
#define TIOCCDTR         SYMBOLIC(TIOCCDTR)
#define TIOCCHKVERAUTH   SYMBOLIC(TIOCCHKVERAUTH)
#define TIOCCONS         SYMBOLIC(TIOCCONS)
#define TIOCEXT          SYMBOLIC(TIOCEXT)
#define TIOCFLAG_CLOCAL  SYMBOLIC(TIOCFLAG_CLOCAL)
#define TIOCFLAG_MDMBUF  SYMBOLIC(TIOCFLAG_MDMBUF)
#define TIOCFLAG_PPS     SYMBOLIC(TIOCFLAG_PPS)
#define TIOCFLAG_SOFTCAR SYMBOLIC(TIOCFLAG_SOFTCAR)
#define TIOCFLUSH        SYMBOLIC(TIOCFLUSH)
#define TIOCGDRAINWAIT   SYMBOLIC(TIOCGDRAINWAIT)
#define TIOCGETD         SYMBOLIC(TIOCGETD)
#define TIOCGFLAGS       SYMBOLIC(TIOCGFLAGS)
#define TIOCGPGRP        SYMBOLIC(TIOCGPGRP)
#define TIOCGPTN         SYMBOLIC(TIOCGPTN)
#define TIOCGSID         SYMBOLIC(TIOCGSID)
#define TIOCGTSTAMP      SYMBOLIC(TIOCGTSTAMP)
#define TIOCGWINSZ       SYMBOLIC(TIOCGWINSZ)
#define TIOCNOTTY        SYMBOLIC(TIOCNOTTY)
#define TIOCNXCL         SYMBOLIC(TIOCNXCL)
#define TIOCOUTQ         SYMBOLIC(TIOCOUTQ)
#define TIOCPTMASTER     SYMBOLIC(TIOCPTMASTER)
#define TIOCREMOTE       SYMBOLIC(TIOCREMOTE)
#define TIOCSBRK         SYMBOLIC(TIOCSBRK)
#define TIOCSCTTY        SYMBOLIC(TIOCSCTTY)
#define TIOCSDRAINWAIT   SYMBOLIC(TIOCSDRAINWAIT)
#define TIOCSDTR         SYMBOLIC(TIOCSDTR)
#define TIOCSERGETLSR    SYMBOLIC(TIOCSERGETLSR)
#define TIOCSERGETMULTI  SYMBOLIC(TIOCSERGETMULTI)
#define TIOCSERSETMULTI  SYMBOLIC(TIOCSERSETMULTI)
#define TIOCSER_TEMT     SYMBOLIC(TIOCSER_TEMT)
#define TIOCSETD         SYMBOLIC(TIOCSETD)
#define TIOCSETVERAUTH   SYMBOLIC(TIOCSETVERAUTH)
#define TIOCSFLAGS       SYMBOLIC(TIOCSFLAGS)
#define TIOCSIG          SYMBOLIC(TIOCSIG)
#define TIOCSPGRP        SYMBOLIC(TIOCSPGRP)
#define TIOCSTART        SYMBOLIC(TIOCSTART)
#define TIOCSTAT         SYMBOLIC(TIOCSTAT)
#define TIOCSTI          SYMBOLIC(TIOCSTI)
#define TIOCSTSTAMP      SYMBOLIC(TIOCSTSTAMP)
#define TIOCSWINSZ       SYMBOLIC(TIOCSWINSZ)
#define TIOCTIMESTAMP    SYMBOLIC(TIOCTIMESTAMP)
#define TIOCUCNTL_CBRK   SYMBOLIC(TIOCUCNTL_CBRK)
#define TOSTOP           SYMBOLIC(TOSTOP)
#define TTYDISC          SYMBOLIC(TTYDISC)
#define VDISCARD         SYMBOLIC(VDISCARD)
#define VEOF             SYMBOLIC(VEOF)
#define VEOL             SYMBOLIC(VEOL)
#define VEOL2            SYMBOLIC(VEOL2)
#define VERASE           SYMBOLIC(VERASE)
#define VERIFY           SYMBOLIC(VERIFY)
#define VINTR            SYMBOLIC(VINTR)
#define VKILL            SYMBOLIC(VKILL)
#define VLNEXT           SYMBOLIC(VLNEXT)
#define VMIN             SYMBOLIC(VMIN)
#define VQUIT            SYMBOLIC(VQUIT)
#define VREPRINT         SYMBOLIC(VREPRINT)
#define VSTART           SYMBOLIC(VSTART)
#define VSTOP            SYMBOLIC(VSTOP)
#define VSUSP            SYMBOLIC(VSUSP)
#define VSWTC            SYMBOLIC(VSWTC)
#define VTIME            SYMBOLIC(VTIME)
#define VWERASE          SYMBOLIC(VWERASE)
#define XCASE            SYMBOLIC(XCASE)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_ */
