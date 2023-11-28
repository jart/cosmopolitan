#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_IPPORT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_IPPORT_H_

/**
 * ARPA network ports.
 */
#define IPPORT_ECHO         7
#define IPPORT_DISCARD      9
#define IPPORT_SYSTAT       11
#define IPPORT_DAYTIME      13
#define IPPORT_NETSTAT      15
#define IPPORT_FTP          21
#define IPPORT_TELNET       23
#define IPPORT_SMTP         25
#define IPPORT_TIMESERVER   37
#define IPPORT_NAMESERVER   42
#define IPPORT_WHOIS        43
#define IPPORT_MTP          57
#define IPPORT_TFTP         69
#define IPPORT_RJE          77
#define IPPORT_FINGER       79
#define IPPORT_TTYLINK      87
#define IPPORT_SUPDUP       95
#define IPPORT_EXECSERVER   512
#define IPPORT_LOGINSERVER  513
#define IPPORT_CMDSERVER    514
#define IPPORT_EFSSERVER    520
#define IPPORT_BIFFUDP      512
#define IPPORT_WHOSERVER    513
#define IPPORT_ROUTESERVER  520
#define IPPORT_RESERVED     1024
#define IPPORT_USERRESERVED IPPORT_USERRESERVED

/**
 * Modern network ports.
 */
#define IPPORT_SSH         22
#define IPPORT_DOMAIN      53
#define IPPORT_HTTP        80
#define IPPORT_POP3        110
#define IPPORT_SFTP        115
#define IPPORT_NTP         123
#define IPPORT_IMAP2       143
#define IPPORT_NETBIOS_NS  137
#define IPPORT_NETBIOS_DGM 138
#define IPPORT_NETBIOS_SSN 139
#define IPPORT_BGP         179
#define IPPORT_IRC         194
#define IPPORT_HTTPS       443
#define IPPORT_PRINTER     515
#define IPPORT_NFS         2049
#define IPPORT_DISTCC      3632
#define IPPORT_SIP         5060
#define IPPORT_POSTGRESQL  5432
#define IPPORT_X11         6000
#define IPPORT_GIT         9418
#define IPPORT_IRCD        6667
#define IPPORT_IMAPS       993
#define IPPORT_POP3S       995

COSMOPOLITAN_C_START_

extern const int IPPORT_USERRESERVED;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_IPPORT_H_ */
