#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SO_H_
#include "libc/runtime/symbolic.h"

#define LOCAL_PEERCRED           SYMBOLIC(LOCAL_PEERCRED)
#define SO_ACCEPTCONN            SYMBOLIC(SO_ACCEPTCONN)
#define SO_ATTACH_BPF            SYMBOLIC(SO_ATTACH_BPF)
#define SO_ATTACH_FILTER         SYMBOLIC(SO_ATTACH_FILTER)
#define SO_ATTACH_REUSEPORT_CBPF SYMBOLIC(SO_ATTACH_REUSEPORT_CBPF)
#define SO_ATTACH_REUSEPORT_EBPF SYMBOLIC(SO_ATTACH_REUSEPORT_EBPF)
#define SO_BINDTODEVICE          SYMBOLIC(SO_BINDTODEVICE)
#define SO_BPF_EXTENSIONS        SYMBOLIC(SO_BPF_EXTENSIONS)
#define SO_BROADCAST             SYMBOLIC(SO_BROADCAST)
#define SO_BSDCOMPAT             SYMBOLIC(SO_BSDCOMPAT)
#define SO_BUSY_POLL             SYMBOLIC(SO_BUSY_POLL)
#define SO_CNX_ADVICE            SYMBOLIC(SO_CNX_ADVICE)
#define SO_DEBUG                 SYMBOLIC(SO_DEBUG)
#define SO_DETACH_BPF            SYMBOLIC(SO_DETACH_BPF)
#define SO_DETACH_FILTER         SYMBOLIC(SO_DETACH_FILTER)
#define SO_DOMAIN                SYMBOLIC(SO_DOMAIN)
#define SO_DONTROUTE             SYMBOLIC(SO_DONTROUTE)
#define SO_ERROR                 SYMBOLIC(SO_ERROR)
#define SO_EXCLUSIVEADDRUSE      SYMBOLIC(SO_EXCLUSIVEADDRUSE)
#define SO_GET_FILTER            SYMBOLIC(SO_GET_FILTER)
#define SO_INCOMING_CPU          SYMBOLIC(SO_INCOMING_CPU)
#define SO_KEEPALIVE             SYMBOLIC(SO_KEEPALIVE)
#define SO_LINGER                SYMBOLIC(SO_LINGER)
#define SO_LOCK_FILTER           SYMBOLIC(SO_LOCK_FILTER)
#define SO_MARK                  SYMBOLIC(SO_MARK)
#define SO_MAX_PACING_RATE       SYMBOLIC(SO_MAX_PACING_RATE)
#define SO_NOFCS                 SYMBOLIC(SO_NOFCS)
#define SO_NO_CHECK              SYMBOLIC(SO_NO_CHECK)
#define SO_OOBINLINE             SYMBOLIC(SO_OOBINLINE)
#define SO_PASSCRED              SYMBOLIC(SO_PASSCRED)
#define SO_PASSSEC               SYMBOLIC(SO_PASSSEC)
#define SO_PEEK_OFF              SYMBOLIC(SO_PEEK_OFF)
#define SO_PEERCRED              SYMBOLIC(SO_PEERCRED)
#define SO_PEERNAME              SYMBOLIC(SO_PEERNAME)
#define SO_PEERSEC               SYMBOLIC(SO_PEERSEC)
#define SO_PRIORITY              SYMBOLIC(SO_PRIORITY)
#define SO_PROTOCOL              SYMBOLIC(SO_PROTOCOL)
#define SO_RCVBUF                SYMBOLIC(SO_RCVBUF)
#define SO_RCVBUFFORCE           SYMBOLIC(SO_RCVBUFFORCE)
#define SO_RCVLOWAT              SYMBOLIC(SO_RCVLOWAT)
#define SO_RCVTIMEO              SYMBOLIC(SO_RCVTIMEO)
#define SO_REUSEADDR             SYMBOLIC(SO_REUSEADDR)
#define SO_REUSEPORT             SYMBOLIC(SO_REUSEPORT)
#define SO_RXQ_OVFL              SYMBOLIC(SO_RXQ_OVFL)
#define SO_SELECT_ERR_QUEUE      SYMBOLIC(SO_SELECT_ERR_QUEUE)
#define SO_SETFIB                SYMBOLIC(SO_SETFIB)
#define SO_SNDBUF                SYMBOLIC(SO_SNDBUF)
#define SO_SNDBUFFORCE           SYMBOLIC(SO_SNDBUFFORCE)
#define SO_SNDLOWAT              SYMBOLIC(SO_SNDLOWAT)
#define SO_SNDTIMEO              SYMBOLIC(SO_SNDTIMEO)
#define SO_TIMESTAMP             SYMBOLIC(SO_TIMESTAMP)
#define SO_TIMESTAMPING          SYMBOLIC(SO_TIMESTAMPING)
#define SO_TIMESTAMPNS           SYMBOLIC(SO_TIMESTAMPNS)
#define SO_TYPE                  SYMBOLIC(SO_TYPE)
#define SO_USELOOPBACK           SYMBOLIC(SO_USELOOPBACK)
#define SO_WIFI_STATUS           SYMBOLIC(SO_WIFI_STATUS)

#define SO_SECURITY_AUTHENTICATION     SYMBOLIC(SO_SECURITY_AUTHENTICATION)
#define SO_SECURITY_ENCRYPTION_NETWORK SYMBOLIC(SO_SECURITY_ENCRYPTION_NETWORK)
#define SO_SECURITY_ENCRYPTION_TRANSPORT \
  SYMBOLIC(SO_SECURITY_ENCRYPTION_TRANSPORT)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long LOCAL_PEERCRED;
extern const long SO_ACCEPTCONN;
extern const long SO_ATTACH_BPF;
extern const long SO_ATTACH_FILTER;
extern const long SO_ATTACH_REUSEPORT_CBPF;
extern const long SO_ATTACH_REUSEPORT_EBPF;
extern const long SO_BINDTODEVICE;
extern const long SO_BPF_EXTENSIONS;
extern const long SO_BROADCAST;
extern const long SO_BSDCOMPAT;
extern const long SO_BUSY_POLL;
extern const long SO_CNX_ADVICE;
extern const long SO_DEBUG;
extern const long SO_DETACH_BPF;
extern const long SO_DETACH_FILTER;
extern const long SO_DOMAIN;
extern const long SO_DONTROUTE;
extern const long SO_ERROR;
extern const long SO_EXCLUSIVEADDRUSE;
extern const long SO_GET_FILTER;
extern const long SO_INCOMING_CPU;
extern const long SO_KEEPALIVE;
extern const long SO_LINGER;
extern const long SO_LOCK_FILTER;
extern const long SO_MARK;
extern const long SO_MAX_PACING_RATE;
extern const long SO_NOFCS;
extern const long SO_NO_CHECK;
extern const long SO_OOBINLINE;
extern const long SO_PASSCRED;
extern const long SO_PASSSEC;
extern const long SO_PEEK_OFF;
extern const long SO_PEERCRED;
extern const long SO_PEERNAME;
extern const long SO_PEERSEC;
extern const long SO_PRIORITY;
extern const long SO_PROTOCOL;
extern const long SO_RCVBUF;
extern const long SO_RCVBUFFORCE;
extern const long SO_RCVLOWAT;
extern const long SO_RCVTIMEO;
extern const long SO_REUSEADDR;
extern const long SO_REUSEPORT;
extern const long SO_RXQ_OVFL;
extern const long SO_SECURITY_AUTHENTICATION;
extern const long SO_SECURITY_ENCRYPTION_NETWORK;
extern const long SO_SECURITY_ENCRYPTION_TRANSPORT;
extern const long SO_SELECT_ERR_QUEUE;
extern const long SO_SETFIB;
extern const long SO_SNDBUF;
extern const long SO_SNDBUFFORCE;
extern const long SO_SNDLOWAT;
extern const long SO_SNDTIMEO;
extern const long SO_TIMESTAMP;
extern const long SO_TIMESTAMPING;
extern const long SO_TIMESTAMPNS;
extern const long SO_TYPE;
extern const long SO_USELOOPBACK;
extern const long SO_WIFI_STATUS;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SO_H_ */
