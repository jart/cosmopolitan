#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
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

#define BRKINT   0x02
#define ICRNL    0x0100
#define IGNBRK   0x01
#define IGNCR    0x80
#define IGNPAR   0x04
#define IMAXBEL  0x2000
#define INLCR    0x40
#define INPCK    0x10
#define ISTRIP   0x20
#define IXANY    0x0800
#define OPOST    0x01
#define PARMRK   0x08
#define TIOCSTOP TIOCSTOP

#define NLDLY  NLDLY
#define NL0    0
#define NL1    NL1
#define NL2    NL2
#define NL3    NL3
#define CRDLY  CRDLY
#define CR0    0
#define CR1    CR1
#define CR2    CR2
#define CR3    CR3
#define TABDLY TABDLY
#define TAB0   0
#define TAB1   TAB1
#define TAB2   TAB2
#define TAB3   TAB3
#define XTABS  XTABS
#define BSDLY  BSDLY
#define BS0    0
#define BS1    BS1
#define BS2    BS2
#define VTDLY  VTDLY
#define VT0    0
#define VT1    VT1
#define VT2    VT2
#define FFDLY  FFDLY
#define FF0    0
#define FF1    FF1
#define FF2    FF2

#define BUSY             BUSY
#define CBAUD            CBAUD
#define CBAUDEX          CBAUDEX
#define CIBAUD           CIBAUD
#define CLOCAL           CLOCAL
#define CMSPAR           CMSPAR
#define CREAD            CREAD
#define CS5              CS5
#define CS6              CS6
#define CS7              CS7
#define CS8              CS8
#define CSIZE            CSIZE
#define CSTOPB           CSTOPB
#define ECHO             8
#define ECHOCTL          ECHOCTL
#define ECHOE            ECHOE
#define ECHOK            ECHOK
#define ECHOKE           ECHOKE
#define ECHONL           ECHONL
#define ECHOPRT          ECHOPRT
#define ENDRUNDISC       ENDRUNDISC
#define EXTA             EXTA
#define EXTB             EXTB
#define EXTPROC          EXTPROC
#define FLUSHO           FLUSHO
#define H4DISC           H4DISC
#define HUPCL            HUPCL
#define ICANON           ICANON
#define IEXTEN           IEXTEN
#define ISIG             ISIG
#define IUCLC            IUCLC
#define IUTF8            IUTF8
#define IXOFF            IXOFF
#define IXON             IXON
#define NCCS             20
#define NETGRAPHDISC     NETGRAPHDISC
#define NMEADISC         NMEADISC
#define NOFLSH           NOFLSH
#define OCRNL            OCRNL
#define OFDEL            OFDEL
#define OFILL            OFILL
#define OLCUC            OLCUC
#define ONLCR            ONLCR
#define ONLRET           ONLRET
#define ONOCR            ONOCR
#define PARENB           PARENB
#define PARODD           PARODD
#define PENDIN           PENDIN
#define PPPDISC          PPPDISC
#define SLIPDISC         SLIPDISC
#define STRIPDISC        STRIPDISC
#define TABLDISC         TABLDISC
#define TCFLSH           TCFLSH
#define TCGETS           TCGETS
#define TCIFLUSH         TCIFLUSH
#define TCIOFF           TCIOFF
#define TCIOFLUSH        TCIOFLUSH
#define TCION            TCION
#define TCOFLUSH         TCOFLUSH
#define TCOOFF           TCOOFF
#define TCOON            TCOON
#define TCSADRAIN        1
#define TCSAFLUSH        2
#define TCSANOW          0
#define TCSBRK           TCSBRK
#define TCSETS           TCSETS
#define TCSETSF          TCSETSF
#define TCSETSW          TCSETSW
#define TCXONC           TCXONC
#define TIOCCBRK         TIOCCBRK
#define TIOCCDTR         TIOCCDTR
#define TIOCCHKVERAUTH   TIOCCHKVERAUTH
#define TIOCCONS         TIOCCONS
#define TIOCEXT          TIOCEXT
#define TIOCFLAG_CLOCAL  TIOCFLAG_CLOCAL
#define TIOCFLAG_MDMBUF  TIOCFLAG_MDMBUF
#define TIOCFLAG_PPS     TIOCFLAG_PPS
#define TIOCFLAG_SOFTCAR TIOCFLAG_SOFTCAR
#define TIOCFLUSH        TIOCFLUSH
#define TIOCGDRAINWAIT   TIOCGDRAINWAIT
#define TIOCGETD         TIOCGETD
#define TIOCGFLAGS       TIOCGFLAGS
#define TIOCGPGRP        TIOCGPGRP
#define TIOCGPTN         TIOCGPTN
#define TIOCGSID         TIOCGSID
#define TIOCGTSTAMP      TIOCGTSTAMP
#define TIOCGWINSZ       TIOCGWINSZ
#define TIOCNOTTY        TIOCNOTTY
#define TIOCNXCL         TIOCNXCL
#define TIOCOUTQ         TIOCOUTQ
#define TIOCPTMASTER     TIOCPTMASTER
#define TIOCREMOTE       TIOCREMOTE
#define TIOCSBRK         TIOCSBRK
#define TIOCSCTTY        TIOCSCTTY
#define TIOCSDRAINWAIT   TIOCSDRAINWAIT
#define TIOCSDTR         TIOCSDTR
#define TIOCSERGETLSR    TIOCSERGETLSR
#define TIOCSERGETMULTI  TIOCSERGETMULTI
#define TIOCSERSETMULTI  TIOCSERSETMULTI
#define TIOCSER_TEMT     TIOCSER_TEMT
#define TIOCSETD         TIOCSETD
#define TIOCSETVERAUTH   TIOCSETVERAUTH
#define TIOCSFLAGS       TIOCSFLAGS
#define TIOCSIG          TIOCSIG
#define TIOCSPGRP        TIOCSPGRP
#define TIOCSTART        TIOCSTART
#define TIOCSTAT         TIOCSTAT
#define TIOCSTI          TIOCSTI
#define TIOCSTSTAMP      TIOCSTSTAMP
#define TIOCSWINSZ       TIOCSWINSZ
#define TIOCTIMESTAMP    TIOCTIMESTAMP
#define TIOCUCNTL_CBRK   TIOCUCNTL_CBRK
#define TOSTOP           TOSTOP
#define TTYDISC          TTYDISC
#define VDISCARD         VDISCARD
#define VEOF             VEOF
#define VEOL             VEOL
#define VEOL2            VEOL2
#define VERASE           VERASE
#define VERIFY           VERIFY
#define VINTR            VINTR
#define VKILL            VKILL
#define VLNEXT           VLNEXT
#define VMIN             VMIN
#define VQUIT            VQUIT
#define VREPRINT         VREPRINT
#define VSTART           VSTART
#define VSTOP            VSTOP
#define VSUSP            VSUSP
#define VSWTC            VSWTC
#define VTIME            VTIME
#define VWERASE          VWERASE
#define XCASE            XCASE

#define __tmpcosmo_TIOCSTOP -1017191481

#define __tmpcosmo_NLDLY  1319994273
#define __tmpcosmo_NL1    -70287396
#define __tmpcosmo_NL2    652191906
#define __tmpcosmo_NL3    1513394829
#define __tmpcosmo_CRDLY  2044628767
#define __tmpcosmo_CR1    1115673592
#define __tmpcosmo_CR2    1871515224
#define __tmpcosmo_CR3    -1010651809
#define __tmpcosmo_TABDLY -886090656
#define __tmpcosmo_TAB1   778986470
#define __tmpcosmo_TAB2   -382588940
#define __tmpcosmo_TAB3   -238740052
#define __tmpcosmo_XTABS  3851498
#define __tmpcosmo_BSDLY  1798324922
#define __tmpcosmo_BS1    -2123797836
#define __tmpcosmo_BS2    -354341079
#define __tmpcosmo_VTDLY  -1916199946
#define __tmpcosmo_VT1    159615710
#define __tmpcosmo_VT2    -2135726260
#define __tmpcosmo_FFDLY  1018442558
#define __tmpcosmo_FF1    366192762
#define __tmpcosmo_FF2    1064173224

#define __tmpcosmo_BUSY             1091122196
#define __tmpcosmo_CBAUD            621840898
#define __tmpcosmo_CBAUDEX          1684865431
#define __tmpcosmo_CIBAUD           232332978
#define __tmpcosmo_CLOCAL           819516838
#define __tmpcosmo_CMSPAR           -1110572378
#define __tmpcosmo_CREAD            -1620802775
#define __tmpcosmo_CS5              -909937503
#define __tmpcosmo_CS6              1486108550
#define __tmpcosmo_CS7              2070161332
#define __tmpcosmo_CS8              -894417336
#define __tmpcosmo_CSIZE            -2142503038
#define __tmpcosmo_CSTOPB           -1893740371
#define __tmpcosmo_ECHOCTL          1839844219
#define __tmpcosmo_ECHOE            -462490559
#define __tmpcosmo_ECHOK            2582183
#define __tmpcosmo_ECHOKE           1807548346
#define __tmpcosmo_ECHONL           -1627629800
#define __tmpcosmo_ECHOPRT          789779244
#define __tmpcosmo_ENDRUNDISC       890493373
#define __tmpcosmo_EXTA             -1952310979
#define __tmpcosmo_EXTB             -1924899818
#define __tmpcosmo_EXTPROC          1954282515
#define __tmpcosmo_FLUSHO           -1439777831
#define __tmpcosmo_H4DISC           -383446731
#define __tmpcosmo_HUPCL            -1762956935
#define __tmpcosmo_ICANON           1391597225
#define __tmpcosmo_IEXTEN           -2017256995
#define __tmpcosmo_ISIG             1460108174
#define __tmpcosmo_IUCLC            1013808696
#define __tmpcosmo_IUTF8            1988433768
#define __tmpcosmo_IXOFF            -206825490
#define __tmpcosmo_IXON             -1157774462
#define __tmpcosmo_NETGRAPHDISC     -99256515
#define __tmpcosmo_NMEADISC         -74984305
#define __tmpcosmo_NOFLSH           -1818319598
#define __tmpcosmo_OCRNL            -1553778506
#define __tmpcosmo_OFDEL            424931285
#define __tmpcosmo_OFILL            -414167674
#define __tmpcosmo_OLCUC            2114383293
#define __tmpcosmo_ONLCR            -1154324374
#define __tmpcosmo_ONLRET           -639724866
#define __tmpcosmo_ONOCR            -490882729
#define __tmpcosmo_PARENB           -1774689151
#define __tmpcosmo_PARODD           -404286749
#define __tmpcosmo_PENDIN           -619915085
#define __tmpcosmo_PPPDISC          -1392399884
#define __tmpcosmo_SLIPDISC         -183947027
#define __tmpcosmo_STRIPDISC        2025122243
#define __tmpcosmo_TABLDISC         -1858378918
#define __tmpcosmo_TCFLSH           -243453721
#define __tmpcosmo_TCGETS           -916893397
#define __tmpcosmo_TCIFLUSH         1056759293
#define __tmpcosmo_TCIOFF           -737116690
#define __tmpcosmo_TCIOFLUSH        -342999896
#define __tmpcosmo_TCION            520326151
#define __tmpcosmo_TCOFLUSH         659539281
#define __tmpcosmo_TCOOFF           478400994
#define __tmpcosmo_TCOON            1521058272
#define __tmpcosmo_TCSBRK           1276777613
#define __tmpcosmo_TCSETS           535983615
#define __tmpcosmo_TCSETSF          659034961
#define __tmpcosmo_TCSETSW          1654950622
#define __tmpcosmo_TCXONC           -647714748
#define __tmpcosmo_TIOCCBRK         1839061556
#define __tmpcosmo_TIOCCDTR         -752068369
#define __tmpcosmo_TIOCCHKVERAUTH   -1183922440
#define __tmpcosmo_TIOCCONS         1455144588
#define __tmpcosmo_TIOCEXT          186515040
#define __tmpcosmo_TIOCFLAG_CLOCAL  698005770
#define __tmpcosmo_TIOCFLAG_MDMBUF  1422045049
#define __tmpcosmo_TIOCFLAG_PPS     -620931778
#define __tmpcosmo_TIOCFLAG_SOFTCAR -140438205
#define __tmpcosmo_TIOCFLUSH        516222339
#define __tmpcosmo_TIOCGDRAINWAIT   -842950172
#define __tmpcosmo_TIOCGETD         470897144
#define __tmpcosmo_TIOCGFLAGS       713359628
#define __tmpcosmo_TIOCGPGRP        1020495911
#define __tmpcosmo_TIOCGPTN         67701595
#define __tmpcosmo_TIOCGSID         -1924582947
#define __tmpcosmo_TIOCGTSTAMP      1116108087
#define __tmpcosmo_TIOCGWINSZ       965491756
#define __tmpcosmo_TIOCNOTTY        1073131930
#define __tmpcosmo_TIOCNXCL         1210582499
#define __tmpcosmo_TIOCOUTQ         1063967309
#define __tmpcosmo_TIOCPTMASTER     -1722245149
#define __tmpcosmo_TIOCREMOTE       -478898903
#define __tmpcosmo_TIOCSBRK         -1827728950
#define __tmpcosmo_TIOCSCTTY        -1994241678
#define __tmpcosmo_TIOCSDRAINWAIT   2088849044
#define __tmpcosmo_TIOCSDTR         -1218831027
#define __tmpcosmo_TIOCSERGETLSR    -196763909
#define __tmpcosmo_TIOCSERGETMULTI  -1904826952
#define __tmpcosmo_TIOCSERSETMULTI  -2096909859
#define __tmpcosmo_TIOCSER_TEMT     762771473
#define __tmpcosmo_TIOCSETD         -860412918
#define __tmpcosmo_TIOCSETVERAUTH   -1550886356
#define __tmpcosmo_TIOCSFLAGS       1899558086
#define __tmpcosmo_TIOCSIG          975589334
#define __tmpcosmo_TIOCSPGRP        -1362234385
#define __tmpcosmo_TIOCSTART        -1106632595
#define __tmpcosmo_TIOCSTAT         -1240783913
#define __tmpcosmo_TIOCSTI          1704247727
#define __tmpcosmo_TIOCSTSTAMP      290377566
#define __tmpcosmo_TIOCSWINSZ       -273178782
#define __tmpcosmo_TIOCTIMESTAMP    -167408548
#define __tmpcosmo_TIOCUCNTL_CBRK   -1260429107
#define __tmpcosmo_TOSTOP           1907739786
#define __tmpcosmo_TTYDISC          -1450700552
#define __tmpcosmo_VDISCARD         1773966586
#define __tmpcosmo_VEOF             1992016613
#define __tmpcosmo_VEOL             1926565027
#define __tmpcosmo_VEOL2            -1297272191
#define __tmpcosmo_VERASE           864859287
#define __tmpcosmo_VERIFY           1512705907
#define __tmpcosmo_VINTR            -973181253
#define __tmpcosmo_VKILL            1764217477
#define __tmpcosmo_VLNEXT           -1045308606
#define __tmpcosmo_VMIN             1479455781
#define __tmpcosmo_VQUIT            853995221
#define __tmpcosmo_VREPRINT         868062415
#define __tmpcosmo_VSTART           -245321985
#define __tmpcosmo_VSTOP            -1379951528
#define __tmpcosmo_VSUSP            -1054959499
#define __tmpcosmo_VSWTC            1631975394
#define __tmpcosmo_VTIME            -942478376
#define __tmpcosmo_VWERASE          -908507167
#define __tmpcosmo_XCASE            -34176757

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_ */
