#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long BRKINT;
hidden extern const long BSDLY;
hidden extern const long BUSY;
hidden extern const long CANBSIZ;
hidden extern const long CBAUD;
hidden extern const long CBAUDEX;
hidden extern const long CBRK;
hidden extern const long CEOL;
hidden extern const long CIBAUD;
hidden extern const long CLOCAL;
hidden extern const long CMSPAR;
hidden extern const long CRDLY;
hidden extern const long CS6;
hidden extern const long CS7;
hidden extern const long CS8;
hidden extern const long CSIZE;
hidden extern const long ECHO;
hidden extern const long ECHOCTL;
hidden extern const long ECHOE;
hidden extern const long ECHOK;
hidden extern const long ECHOKE;
hidden extern const long ECHONL;
hidden extern const long ECHOPRT;
hidden extern const long ENDRUNDISC;
hidden extern const long EXTPROC;
hidden extern const long FFDLY;
hidden extern const long FLUSHO;
hidden extern const long H4DISC;
hidden extern const long ICANON;
hidden extern const long ICRNL;
hidden extern const long IEXTEN;
hidden extern const long IGNBRK;
hidden extern const long IGNCR;
hidden extern const long IGNPAR;
hidden extern const long IMAXBEL;
hidden extern const long INLCR;
hidden extern const long INPCK;
hidden extern const long ISIG;
hidden extern const long ISTRIP;
hidden extern const long IUCLC;
hidden extern const long IUTF8;
hidden extern const long IXANY;
hidden extern const long IXOFF;
hidden extern const long IXON;
hidden extern const long NCCS;
hidden extern const long NETGRAPHDISC;
hidden extern const long NLDLY;
hidden extern const long NMEADISC;
hidden extern const long NOFLSH;
hidden extern const long OCRNL;
hidden extern const long OFDEL;
hidden extern const long OFILL;
hidden extern const long OLCUC;
hidden extern const long ONLCR;
hidden extern const long ONLRET;
hidden extern const long ONOCR;
hidden extern const long OPOST;
hidden extern const long PARENB;
hidden extern const long PARMRK;
hidden extern const long PARODD;
hidden extern const long PENDIN;
hidden extern const long PPPDISC;
hidden extern const long SLIPDISC;
hidden extern const long STRIPDISC;
hidden extern const long TABDLY;
hidden extern const long TABLDISC;
hidden extern const long TCGETS;
hidden extern const long TCSADRAIN;
hidden extern const long TCSAFLUSH;
hidden extern const long TCSANOW;
hidden extern const long TCSETS;
hidden extern const long TCSETSF;
hidden extern const long TCSETSW;
hidden extern const long TIOCCBRK;
hidden extern const long TIOCCDTR;
hidden extern const long TIOCCHKVERAUTH;
hidden extern const long TIOCCONS;
hidden extern const long TIOCDRAIN;
hidden extern const long TIOCEXT;
hidden extern const long TIOCFLAG_CLOCAL;
hidden extern const long TIOCFLAG_MDMBUF;
hidden extern const long TIOCFLAG_PPS;
hidden extern const long TIOCFLAG_SOFTCAR;
hidden extern const long TIOCFLUSH;
hidden extern const long TIOCGDRAINWAIT;
hidden extern const long TIOCGETD;
hidden extern const long TIOCGFLAGS;
hidden extern const long TIOCGPGRP;
hidden extern const long TIOCGPTN;
hidden extern const long TIOCGSID;
hidden extern const long TIOCGTSTAMP;
hidden extern const long TIOCGWINSZ;
hidden extern const long TIOCNOTTY;
hidden extern const long TIOCNXCL;
hidden extern const long TIOCOUTQ;
hidden extern const long TIOCPTMASTER;
hidden extern const long TIOCREMOTE;
hidden extern const long TIOCSBRK;
hidden extern const long TIOCSCTTY;
hidden extern const long TIOCSDRAINWAIT;
hidden extern const long TIOCSDTR;
hidden extern const long TIOCSERGETLSR;
hidden extern const long TIOCSERGETMULTI;
hidden extern const long TIOCSERSETMULTI;
hidden extern const long TIOCSER_TEMT;
hidden extern const long TIOCSETD;
hidden extern const long TIOCSETVERAUTH;
hidden extern const long TIOCSFLAGS;
hidden extern const long TIOCSIG;
hidden extern const long TIOCSPGRP;
hidden extern const long TIOCSTART;
hidden extern const long TIOCSTAT;
hidden extern const long TIOCSTI;
hidden extern const long TIOCSTSTAMP;
hidden extern const long TIOCSWINSZ;
hidden extern const long TIOCTIMESTAMP;
hidden extern const long TIOCUCNTL_CBRK;
hidden extern const long TOSTOP;
hidden extern const long TTYDISC;
hidden extern const long VDISCARD;
hidden extern const long VEOF;
hidden extern const long VEOL;
hidden extern const long VEOL2;
hidden extern const long VERASE;
hidden extern const long VERIFY;
hidden extern const long VINTR;
hidden extern const long VKILL;
hidden extern const long VLNEXT;
hidden extern const long VMIN;
hidden extern const long VQUIT;
hidden extern const long VREPRINT;
hidden extern const long VSTART;
hidden extern const long VSTOP;
hidden extern const long VSUSP;
hidden extern const long VSWTC;
hidden extern const long VTDLY;
hidden extern const long VTIME;
hidden extern const long VWERASE;
hidden extern const long XCASE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define IGNBRK  LITERALLY(0b0000000000000001)
#define BRKINT  LITERALLY(0b0000000000000010)
#define IGNPAR  LITERALLY(0b0000000000000100)
#define PARMRK  LITERALLY(0b0000000000001000)
#define INPCK   LITERALLY(0b0000000000010000)
#define ISTRIP  LITERALLY(0b0000000000100000)
#define INLCR   LITERALLY(0b0000000001000000)
#define IGNCR   LITERALLY(0b0000000010000000)
#define ICRNL   LITERALLY(0b0000000100000000)
#define IXANY   LITERALLY(0b0000100000000000)
#define IMAXBEL LITERALLY(0b0010000000000000)
#define OPOST   LITERALLY(0b0000000000000001)

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
#define CBRK             SYMBOLIC(CBRK)
#define CEOL             SYMBOLIC(CEOL)
#define CIBAUD           SYMBOLIC(CIBAUD)
#define CLOCAL           SYMBOLIC(CLOCAL)
#define CMSPAR           SYMBOLIC(CMSPAR)
#define CS6              SYMBOLIC(CS6)
#define CS7              SYMBOLIC(CS7)
#define CS8              SYMBOLIC(CS8)
#define CSIZE            SYMBOLIC(CSIZE)
#define ECHO             LITERALLY(8)
#define ECHOCTL          SYMBOLIC(ECHOCTL)
#define ECHOE            SYMBOLIC(ECHOE)
#define ECHOK            SYMBOLIC(ECHOK)
#define ECHOKE           SYMBOLIC(ECHOKE)
#define ECHONL           SYMBOLIC(ECHONL)
#define ECHOPRT          SYMBOLIC(ECHOPRT)
#define ENDRUNDISC       SYMBOLIC(ENDRUNDISC)
#define EXTPROC          SYMBOLIC(EXTPROC)
#define FLUSHO           SYMBOLIC(FLUSHO)
#define H4DISC           SYMBOLIC(H4DISC)
#define ICANON           SYMBOLIC(ICANON)
#define IEXTEN           SYMBOLIC(IEXTEN)
#define ISIG             SYMBOLIC(ISIG)
#define IUCLC            SYMBOLIC(IUCLC)
#define IUTF8            SYMBOLIC(IUTF8)
#define IXOFF            SYMBOLIC(IXOFF)
#define IXON             SYMBOLIC(IXON)
#define NCCS             LITERALLY(32)
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
#define TCGETS           SYMBOLIC(TCGETS)
#define TCSANOW          LITERALLY(0)
#define TCSADRAIN        LITERALLY(1)
#define TCSAFLUSH        LITERALLY(2)
#define TCSETS           SYMBOLIC(TCSETS)
#define TCSETSF          SYMBOLIC(TCSETSF)
#define TCSETSW          SYMBOLIC(TCSETSW)
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
