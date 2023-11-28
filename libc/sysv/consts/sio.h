#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SIO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SIO_H_
COSMOPOLITAN_C_START_

extern const unsigned long SIOCADDDLCI;
extern const unsigned long SIOCADDMULTI;
extern const unsigned long SIOCADDRT;
extern const unsigned long SIOCDARP;
extern const unsigned long SIOCDELDLCI;
extern const unsigned long SIOCDELMULTI;
extern const unsigned long SIOCDELRT;
extern const unsigned long SIOCDEVPRIVATE;
extern const unsigned long SIOCDIFADDR;
extern const unsigned long SIOCDRARP;
extern const unsigned long SIOCGARP;
extern const unsigned long SIOCGIFADDR;
extern const unsigned long SIOCGIFBR;
extern const unsigned long SIOCGIFBRDADDR;
extern const unsigned long SIOCGIFCONF;
extern const unsigned long SIOCGIFCOUNT;
extern const unsigned long SIOCGIFDSTADDR;
extern const unsigned long SIOCGIFENCAP;
extern const unsigned long SIOCGIFFLAGS;
extern const unsigned long SIOCGIFHWADDR;
extern const unsigned long SIOCGIFINDEX;
extern const unsigned long SIOCGIFMAP;
extern const unsigned long SIOCGIFMEM;
extern const unsigned long SIOCGIFMETRIC;
extern const unsigned long SIOCGIFMTU;
extern const unsigned long SIOCGIFNAME;
extern const unsigned long SIOCGIFNETMASK;
extern const unsigned long SIOCGIFPFLAGS;
extern const unsigned long SIOCGIFSLAVE;
extern const unsigned long SIOCGIFTXQLEN;
extern const unsigned long SIOCGPGRP;
extern const unsigned long SIOCGRARP;
extern const unsigned long SIOCGSTAMP;
extern const unsigned long SIOCGSTAMPNS;
extern const unsigned long SIOCPROTOPRIVATE;
extern const unsigned long SIOCRTMSG;
extern const unsigned long SIOCSARP;
extern const unsigned long SIOCSIFADDR;
extern const unsigned long SIOCSIFBR;
extern const unsigned long SIOCSIFBRDADDR;
extern const unsigned long SIOCSIFDSTADDR;
extern const unsigned long SIOCSIFENCAP;
extern const unsigned long SIOCSIFFLAGS;
extern const unsigned long SIOCSIFHWADDR;
extern const unsigned long SIOCSIFHWBROADCAST;
extern const unsigned long SIOCSIFLINK;
extern const unsigned long SIOCSIFMAP;
extern const unsigned long SIOCSIFMEM;
extern const unsigned long SIOCSIFMETRIC;
extern const unsigned long SIOCSIFMTU;
extern const unsigned long SIOCSIFNAME;
extern const unsigned long SIOCSIFNETMASK;
extern const unsigned long SIOCSIFPFLAGS;
extern const unsigned long SIOCSIFSLAVE;
extern const unsigned long SIOCSIFTXQLEN;
extern const unsigned long SIOCSPGRP;
extern const unsigned long SIOCSRARP;
extern const unsigned long SIOGIFINDEX;

#define SIOCGIFCONF    SIOCGIFCONF
#define SIOCGIFADDR    SIOCGIFADDR
#define SIOCSIFADDR    SIOCSIFADDR
#define SIOCDIFADDR    SIOCDIFADDR
#define SIOCGIFBRDADDR SIOCGIFBRDADDR
#define SIOCGIFNETMASK SIOCGIFNETMASK
#define SIOCGIFFLAGS   SIOCGIFFLAGS
#define SIOCSIFFLAGS   SIOCSIFFLAGS
#define SIOCGIFMETRIC  SIOCGIFMETRIC
#define SIOCSIFMETRIC  SIOCSIFMETRIC
#define SIOCSIFBRDADDR SIOCSIFBRDADDR
#define SIOCSIFNETMASK SIOCSIFNETMASK
#define SIOCGIFDSTADDR SIOCGIFDSTADDR
#define SIOCSIFDSTADDR SIOCSIFDSTADDR
#define SIOCGIFMTU     SIOCGIFMTU
#define SIOCSIFMTU     SIOCSIFMTU

#define SIOCGPGRP SIOCGPGRP
#define SIOCSPGRP SIOCSPGRP

#define SIOCADDMULTI SIOCADDMULTI
#define SIOCDELMULTI SIOCDELMULTI

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SIO_H_ */
