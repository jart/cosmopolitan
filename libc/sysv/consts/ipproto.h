#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_IPPROTO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_IPPROTO_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long IPPROTO_AH;
hidden extern const long IPPROTO_BEETPH;
hidden extern const long IPPROTO_COMP;
hidden extern const long IPPROTO_DCCP;
hidden extern const long IPPROTO_DSTOPTS;
hidden extern const long IPPROTO_EGP;
hidden extern const long IPPROTO_ENCAP;
hidden extern const long IPPROTO_ESP;
hidden extern const long IPPROTO_FRAGMENT;
hidden extern const long IPPROTO_GRE;
hidden extern const long IPPROTO_HOPOPTS;
hidden extern const long IPPROTO_ICMP;
hidden extern const long IPPROTO_ICMPV6;
hidden extern const long IPPROTO_IDP;
hidden extern const long IPPROTO_IGMP;
hidden extern const long IPPROTO_IP;
hidden extern const long IPPROTO_IPIP;
hidden extern const long IPPROTO_IPV6;
hidden extern const long IPPROTO_MAX;
hidden extern const long IPPROTO_MH;
hidden extern const long IPPROTO_MPLS;
hidden extern const long IPPROTO_MTP;
hidden extern const long IPPROTO_NONE;
hidden extern const long IPPROTO_PIM;
hidden extern const long IPPROTO_PUP;
hidden extern const long IPPROTO_RAW;
hidden extern const long IPPROTO_ROUTING;
hidden extern const long IPPROTO_RSVP;
hidden extern const long IPPROTO_SCTP;
hidden extern const long IPPROTO_TCP;
hidden extern const long IPPROTO_TP;
hidden extern const long IPPROTO_UDP;
hidden extern const long IPPROTO_UDPLITE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define IPPROTO_IP   LITERALLY(0)
#define IPPROTO_ICMP LITERALLY(1)
#define IPPROTO_TCP  LITERALLY(6)
#define IPPROTO_UDP  LITERALLY(17)
#define IPPROTO_RAW  LITERALLY(255)

#define IPPROTO_AH       SYMBOLIC(IPPROTO_AH)
#define IPPROTO_BEETPH   SYMBOLIC(IPPROTO_BEETPH)
#define IPPROTO_COMP     SYMBOLIC(IPPROTO_COMP)
#define IPPROTO_DCCP     SYMBOLIC(IPPROTO_DCCP)
#define IPPROTO_DSTOPTS  SYMBOLIC(IPPROTO_DSTOPTS)
#define IPPROTO_EGP      SYMBOLIC(IPPROTO_EGP)
#define IPPROTO_ENCAP    SYMBOLIC(IPPROTO_ENCAP)
#define IPPROTO_ESP      SYMBOLIC(IPPROTO_ESP)
#define IPPROTO_FRAGMENT SYMBOLIC(IPPROTO_FRAGMENT)
#define IPPROTO_GRE      SYMBOLIC(IPPROTO_GRE)
#define IPPROTO_HOPOPTS  SYMBOLIC(IPPROTO_HOPOPTS)
#define IPPROTO_ICMPV6   SYMBOLIC(IPPROTO_ICMPV6)
#define IPPROTO_IDP      SYMBOLIC(IPPROTO_IDP)
#define IPPROTO_IGMP     SYMBOLIC(IPPROTO_IGMP)
#define IPPROTO_IPIP     SYMBOLIC(IPPROTO_IPIP)
#define IPPROTO_IPV6     SYMBOLIC(IPPROTO_IPV6)
#define IPPROTO_MAX      SYMBOLIC(IPPROTO_MAX)
#define IPPROTO_MH       SYMBOLIC(IPPROTO_MH)
#define IPPROTO_MPLS     SYMBOLIC(IPPROTO_MPLS)
#define IPPROTO_MTP      SYMBOLIC(IPPROTO_MTP)
#define IPPROTO_NONE     SYMBOLIC(IPPROTO_NONE)
#define IPPROTO_PIM      SYMBOLIC(IPPROTO_PIM)
#define IPPROTO_PUP      SYMBOLIC(IPPROTO_PUP)
#define IPPROTO_ROUTING  SYMBOLIC(IPPROTO_ROUTING)
#define IPPROTO_RSVP     SYMBOLIC(IPPROTO_RSVP)
#define IPPROTO_SCTP     SYMBOLIC(IPPROTO_SCTP)
#define IPPROTO_TP       SYMBOLIC(IPPROTO_TP)
#define IPPROTO_UDPLITE  SYMBOLIC(IPPROTO_UDPLITE)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_IPPROTO_H_ */
