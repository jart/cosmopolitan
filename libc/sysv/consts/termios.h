#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long BRKINT;
extern const long BS1;
extern const long BS2;
extern const long BSDLY;
extern const long BUSY;
extern const long CANBSIZ;
extern const long CBAUD;
extern const long CBAUDEX;
extern const long CIBAUD;
extern const long CLOCAL;
extern const long CMSPAR;
extern const long CR1;
extern const long CR2;
extern const long CR3;
extern const long CRDLY;
extern const long CREAD;
extern const long CS5;
extern const long CS6;
extern const long CS7;
extern const long CS8;
extern const long CSIZE;
extern const long CSTOPB;
extern const long ECHO;
extern const long ECHOCTL;
extern const long ECHOE;
extern const long ECHOK;
extern const long ECHOKE;
extern const long ECHONL;
extern const long ECHOPRT;
extern const long ENDRUNDISC;
extern const long EXTA;
extern const long EXTB;
extern const long EXTPROC;
extern const long FF1;
extern const long FF2;
extern const long FFDLY;
extern const long FLUSHO;
extern const long H4DISC;
extern const long HUPCL;
extern const long ICANON;
extern const long ICRNL;
extern const long IEXTEN;
extern const long IGNBRK;
extern const long IGNCR;
extern const long IGNPAR;
extern const long IMAXBEL;
extern const long INLCR;
extern const long INPCK;
extern const long ISIG;
extern const long ISTRIP;
extern const long IUCLC;
extern const long IUTF8;
extern const long IXANY;
extern const long IXOFF;
extern const long IXON;
extern const long NETGRAPHDISC;
extern const long NL1;
extern const long NL2;
extern const long NL3;
extern const long NLDLY;
extern const long NMEADISC;
extern const long NOFLSH;
extern const long OCRNL;
extern const long OFDEL;
extern const long OFILL;
extern const long OLCUC;
extern const long ONLCR;
extern const long ONLRET;
extern const long ONOCR;
extern const long OPOST;
extern const long PARENB;
extern const long PARMRK;
extern const long PARODD;
extern const long PENDIN;
extern const long PPPDISC;
extern const long SLIPDISC;
extern const long STRIPDISC;
extern const long TAB1;
extern const long TAB2;
extern const long TAB3;
extern const long TABDLY;
extern const long TABLDISC;
extern const long TCFLSH;
extern const long TCGETS;
extern const long TCIFLUSH;
extern const long TCIOFF;
extern const long TCIOFLUSH;
extern const long TCION;
extern const long TCOFLUSH;
extern const long TCOOFF;
extern const long TCOON;
extern const long TCSADRAIN;
extern const long TCSAFLUSH;
extern const long TCSANOW;
extern const long TCSBRK;
extern const long TCSETS;
extern const long TCSETSF;
extern const long TCSETSW;
extern const long TCXONC;
extern const long TIOCCBRK;
extern const long TIOCCDTR;
extern const long TIOCCHKVERAUTH;
extern const long TIOCCONS;
extern const long TIOCDRAIN;
extern const long TIOCEXT;
extern const long TIOCFLAG_CLOCAL;
extern const long TIOCFLAG_MDMBUF;
extern const long TIOCFLAG_PPS;
extern const long TIOCFLAG_SOFTCAR;
extern const long TIOCFLUSH;
extern const long TIOCGDRAINWAIT;
extern const long TIOCGETD;
extern const long TIOCGFLAGS;
extern const long TIOCGPGRP;
extern const long TIOCGPTN;
extern const long TIOCGSID;
extern const long TIOCGTSTAMP;
extern const long TIOCGWINSZ;
extern const long TIOCNOTTY;
extern const long TIOCNXCL;
extern const long TIOCOUTQ;
extern const long TIOCPTMASTER;
extern const long TIOCREMOTE;
extern const long TIOCSBRK;
extern const long TIOCSCTTY;
extern const long TIOCSDRAINWAIT;
extern const long TIOCSDTR;
extern const long TIOCSERGETLSR;
extern const long TIOCSERGETMULTI;
extern const long TIOCSERSETMULTI;
extern const long TIOCSER_TEMT;
extern const long TIOCSETD;
extern const long TIOCSETVERAUTH;
extern const long TIOCSFLAGS;
extern const long TIOCSIG;
extern const long TIOCSPGRP;
extern const long TIOCSTART;
extern const long TIOCSTAT;
extern const long TIOCSTI;
extern const long TIOCSTOP;
extern const long TIOCSTSTAMP;
extern const long TIOCSWINSZ;
extern const long TIOCTIMESTAMP;
extern const long TIOCUCNTL_CBRK;
extern const long TOSTOP;
extern const long TTYDISC;
extern const long VDISCARD;
extern const long VEOF;
extern const long VEOL2;
extern const long VEOL;
extern const long VERASE;
extern const long VERIFY;
extern const long VINTR;
extern const long VKILL;
extern const long VLNEXT;
extern const long VMIN;
extern const long VQUIT;
extern const long VREPRINT;
extern const long VSTART;
extern const long VSTOP;
extern const long VSUSP;
extern const long VSWTC;
extern const long VT1;
extern const long VT2;
extern const long VTDLY;
extern const long VTIME;
extern const long VWERASE;
extern const long XCASE;
extern const long XTABS;

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
#define CANBSIZ          SYMBOLIC(CANBSIZ)
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
#define TIOCDRAIN        SYMBOLIC(TIOCDRAIN)
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
