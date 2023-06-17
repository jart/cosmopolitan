#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
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
extern const int TCIFLUSH;
extern const int TCIOFLUSH;
extern const int TCOFLUSH;
extern const int TCSADRAIN;
extern const int TCSAFLUSH;
extern const int TCSANOW;
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

#define TCOOFF 0
#define TCOON  1
#define TCIOFF 2
#define TCION  3

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

#define CLOCAL     CLOCAL
#define CREAD      CREAD
#define CS5        CS5
#define CS6        CS6
#define CS7        CS7
#define CS8        CS8
#define CSIZE      CSIZE
#define CSTOPB     CSTOPB
#define ECHO       8
#define ECHOCTL    ECHOCTL
#define ECHOE      ECHOE
#define ECHOK      ECHOK
#define ECHOKE     ECHOKE
#define ECHONL     ECHONL
#define ECHOPRT    ECHOPRT
#define EXTPROC    EXTPROC
#define FLUSHO     FLUSHO
#define HUPCL      HUPCL
#define ICANON     ICANON
#define IEXTEN     IEXTEN
#define ISIG       ISIG
#define IUCLC      IUCLC
#define IUTF8      IUTF8
#define IXOFF      IXOFF
#define IXON       IXON
#define NOFLSH     NOFLSH
#define OCRNL      OCRNL
#define OFDEL      OFDEL
#define OFILL      OFILL
#define OLCUC      OLCUC
#define ONLCR      ONLCR
#define ONLRET     ONLRET
#define ONOCR      ONOCR
#define PARENB     PARENB
#define PARODD     PARODD
#define PENDIN     PENDIN
#define TCIFLUSH   TCIFLUSH
#define TCIOFLUSH  TCIOFLUSH
#define TCOFLUSH   TCOFLUSH
#define TCSADRAIN  1
#define TCSAFLUSH  2
#define TCSANOW    0
#define TIOCCONS   TIOCCONS
#define TIOCGETD   TIOCGETD
#define TIOCGWINSZ TIOCGWINSZ
#define TIOCNOTTY  TIOCNOTTY
#define TIOCNXCL   TIOCNXCL
#define TIOCOUTQ   TIOCOUTQ
#define TIOCSCTTY  TIOCSCTTY
#define TIOCSETD   TIOCSETD
#define TIOCSIG    TIOCSIG
#define TIOCSPGRP  TIOCSPGRP
#define TIOCSTI    TIOCSTI
#define TIOCSWINSZ TIOCSWINSZ
#define TOSTOP     TOSTOP
#define VDISCARD   VDISCARD
#define VEOF       VEOF
#define VEOL       VEOL
#define VEOL2      VEOL2
#define VERASE     VERASE
#define VINTR      VINTR
#define VKILL      VKILL
#define VLNEXT     VLNEXT
#define VMIN       VMIN
#define VQUIT      VQUIT
#define VREPRINT   VREPRINT
#define VSTART     VSTART
#define VSTOP      VSTOP
#define VSUSP      VSUSP
#define VSWTC      VSWTC
#define VTIME      VTIME
#define VWERASE    VWERASE
#define XCASE      XCASE

#define __tmpcosmo__POSIX_VDISABLE 169854768

#define __tmpcosmo_NLDLY   1319994273
#define __tmpcosmo_NL1     -70287396
#define __tmpcosmo_NL2     652191906
#define __tmpcosmo_NL3     1513394829
#define __tmpcosmo_CRDLY   2044628767
#define __tmpcosmo_CR1     1115673592
#define __tmpcosmo_CR2     1871515224
#define __tmpcosmo_CR3     -1010651809
#define __tmpcosmo_TABDLY  -886090656
#define __tmpcosmo_TAB1    778986470
#define __tmpcosmo_TAB2    -382588940
#define __tmpcosmo_TAB3    -238740052
#define __tmpcosmo_XTABS   3851498
#define __tmpcosmo_CRTSCTS -1928718178
#define __tmpcosmo_BSDLY   1798324922
#define __tmpcosmo_BS1     -2123797836
#define __tmpcosmo_BS2     -354341079
#define __tmpcosmo_VTDLY   -1916199946
#define __tmpcosmo_VT1     159615710
#define __tmpcosmo_VT2     -2135726260
#define __tmpcosmo_FFDLY   1018442558
#define __tmpcosmo_FF1     366192762
#define __tmpcosmo_FF2     1064173224

#define __tmpcosmo_CLOCAL     819516838
#define __tmpcosmo_CREAD      -1620802775
#define __tmpcosmo_CS5        -909937503
#define __tmpcosmo_CS6        1486108550
#define __tmpcosmo_CS7        2070161332
#define __tmpcosmo_CS8        -894417336
#define __tmpcosmo_CSIZE      -2142503038
#define __tmpcosmo_CSTOPB     -1893740371
#define __tmpcosmo_ECHOCTL    1839844219
#define __tmpcosmo_ECHOE      -462490559
#define __tmpcosmo_ECHOK      2582183
#define __tmpcosmo_ECHOKE     1807548346
#define __tmpcosmo_ECHONL     -1627629800
#define __tmpcosmo_ECHOPRT    789779244
#define __tmpcosmo_EXTPROC    1954282515
#define __tmpcosmo_FLUSHO     -1439777831
#define __tmpcosmo_HUPCL      -1762956935
#define __tmpcosmo_ICANON     1391597225
#define __tmpcosmo_IEXTEN     -2017256995
#define __tmpcosmo_ISIG       1460108174
#define __tmpcosmo_IUCLC      1013808696
#define __tmpcosmo_IUTF8      1988433768
#define __tmpcosmo_IXOFF      -206825490
#define __tmpcosmo_IXON       -1157774462
#define __tmpcosmo_NOFLSH     -1818319598
#define __tmpcosmo_OCRNL      -1553778506
#define __tmpcosmo_OFDEL      424931285
#define __tmpcosmo_OFILL      -414167674
#define __tmpcosmo_OLCUC      2114383293
#define __tmpcosmo_ONLCR      -1154324374
#define __tmpcosmo_ONLRET     -639724866
#define __tmpcosmo_ONOCR      -490882729
#define __tmpcosmo_PARENB     -1774689151
#define __tmpcosmo_PARODD     -404286749
#define __tmpcosmo_PENDIN     -619915085
#define __tmpcosmo_TCIFLUSH   1056759293
#define __tmpcosmo_TCIOFLUSH  -342999896
#define __tmpcosmo_TCOFLUSH   659539281
#define __tmpcosmo_TIOCCONS   1455144588
#define __tmpcosmo_TIOCGETD   470897144
#define __tmpcosmo_TIOCGWINSZ 965491756
#define __tmpcosmo_TIOCNOTTY  1073131930
#define __tmpcosmo_TIOCNXCL   1210582499
#define __tmpcosmo_TIOCOUTQ   1063967309
#define __tmpcosmo_TIOCSCTTY  -1994241678
#define __tmpcosmo_TIOCSETD   -860412918
#define __tmpcosmo_TIOCSIG    975589334
#define __tmpcosmo_TIOCSPGRP  -1362234385
#define __tmpcosmo_TIOCSTI    1704247727
#define __tmpcosmo_TIOCSWINSZ -273178782
#define __tmpcosmo_TOSTOP     1907739786
#define __tmpcosmo_VDISCARD   1773966586
#define __tmpcosmo_VEOF       1992016613
#define __tmpcosmo_VEOL       1926565027
#define __tmpcosmo_VEOL2      -1297272191
#define __tmpcosmo_VERASE     864859287
#define __tmpcosmo_VINTR      -973181253
#define __tmpcosmo_VKILL      1764217477
#define __tmpcosmo_VLNEXT     -1045308606
#define __tmpcosmo_VMIN       1479455781
#define __tmpcosmo_VQUIT      853995221
#define __tmpcosmo_VREPRINT   868062415
#define __tmpcosmo_VSTART     -245321985
#define __tmpcosmo_VSTOP      -1379951528
#define __tmpcosmo_VSUSP      -1054959499
#define __tmpcosmo_VSWTC      1631975394
#define __tmpcosmo_VTIME      -942478376
#define __tmpcosmo_VWERASE    -908507167
#define __tmpcosmo_XCASE      -34176757

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_TERMIOS_H_ */
