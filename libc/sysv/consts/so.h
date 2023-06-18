#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SO_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int LOCAL_PEERCRED;
extern const int SO_ACCEPTCONN;
extern const int SO_ATTACH_BPF;
extern const int SO_ATTACH_FILTER;
extern const int SO_ATTACH_REUSEPORT_CBPF;
extern const int SO_ATTACH_REUSEPORT_EBPF;
extern const int SO_BINDTODEVICE;
extern const int SO_BPF_EXTENSIONS;
extern const int SO_BROADCAST;
extern const int SO_BSDCOMPAT;
extern const int SO_BUSY_POLL;
extern const int SO_CNX_ADVICE;
extern const int SO_DEBUG;
extern const int SO_DETACH_BPF;
extern const int SO_DETACH_FILTER;
extern const int SO_DOMAIN;
extern const int SO_DONTROUTE;
extern const int SO_ERROR;
extern const int SO_EXCLUSIVEADDRUSE;
extern const int SO_GET_FILTER;
extern const int SO_INCOMING_CPU;
extern const int SO_KEEPALIVE;
extern const int SO_LINGER;
extern const int SO_LOCK_FILTER;
extern const int SO_MARK;
extern const int SO_MAX_PACING_RATE;
extern const int SO_NOFCS;
extern const int SO_NO_CHECK;
extern const int SO_OOBINLINE;
extern const int SO_PASSCRED;
extern const int SO_PASSSEC;
extern const int SO_PEEK_OFF;
extern const int SO_PEERCRED;
extern const int SO_PEERNAME;
extern const int SO_PEERSEC;
extern const int SO_PRIORITY;
extern const int SO_PROTOCOL;
extern const int SO_RCVBUF;
extern const int SO_RCVBUFFORCE;
extern const int SO_RCVLOWAT;
extern const int SO_RCVTIMEO;
extern const int SO_REUSEADDR;
extern const int SO_REUSEPORT;
extern const int SO_RXQ_OVFL;
extern const int SO_SECURITY_AUTHENTICATION;
extern const int SO_SECURITY_ENCRYPTION_NETWORK;
extern const int SO_SECURITY_ENCRYPTION_TRANSPORT;
extern const int SO_SELECT_ERR_QUEUE;
extern const int SO_SETFIB;
extern const int SO_SNDBUF;
extern const int SO_SNDBUFFORCE;
extern const int SO_SNDLOWAT;
extern const int SO_SNDTIMEO;
extern const int SO_TIMESTAMP;
extern const int SO_TIMESTAMPING;
extern const int SO_TIMESTAMPNS;
extern const int SO_TYPE;
extern const int SO_USELOOPBACK;
extern const int SO_WIFI_STATUS;

#define SO_DEBUG 1

#define LOCAL_PEERCRED           LOCAL_PEERCRED
#define SO_ACCEPTCONN            SO_ACCEPTCONN
#define SO_ATTACH_BPF            SO_ATTACH_BPF
#define SO_ATTACH_FILTER         SO_ATTACH_FILTER
#define SO_ATTACH_REUSEPORT_CBPF SO_ATTACH_REUSEPORT_CBPF
#define SO_ATTACH_REUSEPORT_EBPF SO_ATTACH_REUSEPORT_EBPF
#define SO_BINDTODEVICE          SO_BINDTODEVICE
#define SO_BPF_EXTENSIONS        SO_BPF_EXTENSIONS
#define SO_BROADCAST             SO_BROADCAST
#define SO_BSDCOMPAT             SO_BSDCOMPAT
#define SO_BUSY_POLL             SO_BUSY_POLL
#define SO_CNX_ADVICE            SO_CNX_ADVICE
#define SO_DETACH_BPF            SO_DETACH_BPF
#define SO_DETACH_FILTER         SO_DETACH_FILTER
#define SO_DOMAIN                SO_DOMAIN
#define SO_DONTROUTE             SO_DONTROUTE
#define SO_ERROR                 SO_ERROR
#define SO_EXCLUSIVEADDRUSE      SO_EXCLUSIVEADDRUSE
#define SO_GET_FILTER            SO_GET_FILTER
#define SO_INCOMING_CPU          SO_INCOMING_CPU
#define SO_KEEPALIVE             SO_KEEPALIVE
#define SO_LINGER                SO_LINGER
#define SO_LOCK_FILTER           SO_LOCK_FILTER
#define SO_MARK                  SO_MARK
#define SO_MAX_PACING_RATE       SO_MAX_PACING_RATE
#define SO_NOFCS                 SO_NOFCS
#define SO_NO_CHECK              SO_NO_CHECK
#define SO_OOBINLINE             SO_OOBINLINE
#define SO_PASSCRED              SO_PASSCRED
#define SO_PASSSEC               SO_PASSSEC
#define SO_PEEK_OFF              SO_PEEK_OFF
#define SO_PEERCRED              SO_PEERCRED
#define SO_PEERNAME              SO_PEERNAME
#define SO_PEERSEC               SO_PEERSEC
#define SO_PRIORITY              SO_PRIORITY
#define SO_PROTOCOL              SO_PROTOCOL
#define SO_RCVBUF                SO_RCVBUF
#define SO_RCVBUFFORCE           SO_RCVBUFFORCE
#define SO_RCVLOWAT              SO_RCVLOWAT
#define SO_RCVTIMEO              SO_RCVTIMEO
#define SO_REUSEADDR             SO_REUSEADDR
#define SO_REUSEPORT             SO_REUSEPORT
#define SO_RXQ_OVFL              SO_RXQ_OVFL
#define SO_SELECT_ERR_QUEUE      SO_SELECT_ERR_QUEUE
#define SO_SETFIB                SO_SETFIB
#define SO_SNDBUF                SO_SNDBUF
#define SO_SNDBUFFORCE           SO_SNDBUFFORCE
#define SO_SNDLOWAT              SO_SNDLOWAT
#define SO_SNDTIMEO              SO_SNDTIMEO
#define SO_TIMESTAMP             SO_TIMESTAMP
#define SO_TIMESTAMPING          SO_TIMESTAMPING
#define SO_TIMESTAMPNS           SO_TIMESTAMPNS
#define SO_TYPE                  SO_TYPE
#define SO_USELOOPBACK           SO_USELOOPBACK
#define SO_WIFI_STATUS           SO_WIFI_STATUS

#define SO_SECURITY_AUTHENTICATION       SO_SECURITY_AUTHENTICATION
#define SO_SECURITY_ENCRYPTION_NETWORK   SO_SECURITY_ENCRYPTION_NETWORK
#define SO_SECURITY_ENCRYPTION_TRANSPORT SO_SECURITY_ENCRYPTION_TRANSPORT


COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SO_H_ */
