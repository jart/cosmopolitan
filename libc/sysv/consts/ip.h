#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_IP_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_IP_H_
#include "libc/runtime/symbolic.h"

#define IP_ADD_MEMBERSHIP         SYMBOLIC(IP_ADD_MEMBERSHIP)
#define IP_ADD_SOURCE_MEMBERSHIP  SYMBOLIC(IP_ADD_SOURCE_MEMBERSHIP)
#define IP_BIND_ADDRESS_NO_PORT   SYMBOLIC(IP_BIND_ADDRESS_NO_PORT)
#define IP_BLOCK_SOURCE           SYMBOLIC(IP_BLOCK_SOURCE)
#define IP_CHECKSUM               SYMBOLIC(IP_CHECKSUM)
#define IP_DEFAULT_MULTICAST_LOOP SYMBOLIC(IP_DEFAULT_MULTICAST_LOOP)
#define IP_DEFAULT_MULTICAST_TTL  SYMBOLIC(IP_DEFAULT_MULTICAST_TTL)
#define IP_DROP_MEMBERSHIP        SYMBOLIC(IP_DROP_MEMBERSHIP)
#define IP_DROP_SOURCE_MEMBERSHIP SYMBOLIC(IP_DROP_SOURCE_MEMBERSHIP)
#define IP_FREEBIND               SYMBOLIC(IP_FREEBIND)
#define IP_HDRINCL                SYMBOLIC(IP_HDRINCL)
#define IP_IPSEC_POLICY           SYMBOLIC(IP_IPSEC_POLICY)
#define IP_MAX_MEMBERSHIPS        SYMBOLIC(IP_MAX_MEMBERSHIPS)
#define IP_MINTTL                 SYMBOLIC(IP_MINTTL)
#define IP_MSFILTER               SYMBOLIC(IP_MSFILTER)
#define IP_MTU                    SYMBOLIC(IP_MTU)
#define IP_MTU_DISCOVER           SYMBOLIC(IP_MTU_DISCOVER)
#define IP_MULTICAST_ALL          SYMBOLIC(IP_MULTICAST_ALL)
#define IP_MULTICAST_IF           SYMBOLIC(IP_MULTICAST_IF)
#define IP_MULTICAST_LOOP         SYMBOLIC(IP_MULTICAST_LOOP)
#define IP_MULTICAST_TTL          SYMBOLIC(IP_MULTICAST_TTL)
#define IP_NODEFRAG               SYMBOLIC(IP_NODEFRAG)
#define IP_OPTIONS                SYMBOLIC(IP_OPTIONS)
#define IP_ORIGDSTADDR            SYMBOLIC(IP_ORIGDSTADDR)
#define IP_PASSSEC                SYMBOLIC(IP_PASSSEC)
#define IP_PKTINFO                SYMBOLIC(IP_PKTINFO)
#define IP_PKTOPTIONS             SYMBOLIC(IP_PKTOPTIONS)
#define IP_PMTUDISC               SYMBOLIC(IP_PMTUDISC)
#define IP_PMTUDISC_DO            SYMBOLIC(IP_PMTUDISC_DO)
#define IP_PMTUDISC_DONT          SYMBOLIC(IP_PMTUDISC_DONT)
#define IP_PMTUDISC_INTERFACE     SYMBOLIC(IP_PMTUDISC_INTERFACE)
#define IP_PMTUDISC_OMIT          SYMBOLIC(IP_PMTUDISC_OMIT)
#define IP_PMTUDISC_PROBE         SYMBOLIC(IP_PMTUDISC_PROBE)
#define IP_PMTUDISC_WANT          SYMBOLIC(IP_PMTUDISC_WANT)
#define IP_RECVDSTADDR            SYMBOLIC(IP_RECVDSTADDR)
#define IP_RECVERR                SYMBOLIC(IP_RECVERR)
#define IP_RECVOPTS               SYMBOLIC(IP_RECVOPTS)
#define IP_RECVORIGDSTADDR        SYMBOLIC(IP_RECVORIGDSTADDR)
#define IP_RECVRETOPTS            SYMBOLIC(IP_RECVRETOPTS)
#define IP_RECVTOS                SYMBOLIC(IP_RECVTOS)
#define IP_RECVTTL                SYMBOLIC(IP_RECVTTL)
#define IP_RETOPTS                SYMBOLIC(IP_RETOPTS)
#define IP_ROUTER_ALERT           SYMBOLIC(IP_ROUTER_ALERT)
#define IP_TOS                    SYMBOLIC(IP_TOS)
#define IP_TRANSPARENT            SYMBOLIC(IP_TRANSPARENT)
#define IP_TTL                    SYMBOLIC(IP_TTL)
#define IP_UNBLOCK_SOURCE         SYMBOLIC(IP_UNBLOCK_SOURCE)
#define IP_UNICAST_IF             SYMBOLIC(IP_UNICAST_IF)
#define IP_XFRM_POLICY            SYMBOLIC(IP_XFRM_POLICY)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int IP_ADD_MEMBERSHIP;
extern const int IP_ADD_SOURCE_MEMBERSHIP;
extern const int IP_BIND_ADDRESS_NO_PORT;
extern const int IP_BLOCK_SOURCE;
extern const int IP_CHECKSUM;
extern const int IP_DEFAULT_MULTICAST_LOOP;
extern const int IP_DEFAULT_MULTICAST_TTL;
extern const int IP_DROP_MEMBERSHIP;
extern const int IP_DROP_SOURCE_MEMBERSHIP;
extern const int IP_FREEBIND;
extern const int IP_HDRINCL;
extern const int IP_IPSEC_POLICY;
extern const int IP_MAX_MEMBERSHIPS;
extern const int IP_MINTTL;
extern const int IP_MSFILTER;
extern const int IP_MTU;
extern const int IP_MTU_DISCOVER;
extern const int IP_MULTICAST_ALL;
extern const int IP_MULTICAST_IF;
extern const int IP_MULTICAST_LOOP;
extern const int IP_MULTICAST_TTL;
extern const int IP_NODEFRAG;
extern const int IP_OPTIONS;
extern const int IP_ORIGDSTADDR;
extern const int IP_PASSSEC;
extern const int IP_PKTINFO;
extern const int IP_PKTOPTIONS;
extern const int IP_PMTUDISC;
extern const int IP_PMTUDISC_DO;
extern const int IP_PMTUDISC_DONT;
extern const int IP_PMTUDISC_INTERFACE;
extern const int IP_PMTUDISC_OMIT;
extern const int IP_PMTUDISC_PROBE;
extern const int IP_PMTUDISC_WANT;
extern const int IP_RECVDSTADDR;
extern const int IP_RECVERR;
extern const int IP_RECVOPTS;
extern const int IP_RECVORIGDSTADDR;
extern const int IP_RECVRETOPTS;
extern const int IP_RECVTOS;
extern const int IP_RECVTTL;
extern const int IP_RETOPTS;
extern const int IP_ROUTER_ALERT;
extern const int IP_TOS;
extern const int IP_TRANSPARENT;
extern const int IP_TTL;
extern const int IP_UNBLOCK_SOURCE;
extern const int IP_UNICAST_IF;
extern const int IP_XFRM_POLICY;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_IP_H_ */
