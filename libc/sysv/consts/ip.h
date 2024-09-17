#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_IP_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_IP_H_
COSMOPOLITAN_C_START_

extern const int IP_TOS;
extern const int IP_TTL;
extern const int IP_MTU;
extern const int IP_HDRINCL;
extern const int IP_OPTIONS;
extern const int IP_RECVTTL;
extern const int IP_ADD_MEMBERSHIP;
extern const int IP_DROP_MEMBERSHIP;
extern const int IP_MULTICAST_IF;
extern const int IP_MULTICAST_LOOP;
extern const int IP_MULTICAST_TTL;
extern const int IP_PKTINFO;
extern const int IP_RECVTOS;

#define IP_TOS             IP_TOS
#define IP_TTL             IP_TTL
#define IP_MTU             IP_MTU
#define IP_HDRINCL         IP_HDRINCL
#define IP_OPTIONS         IP_OPTIONS
#define IP_RECVTTL         IP_RECVTTL
#define IP_ADD_MEMBERSHIP  IP_ADD_MEMBERSHIP
#define IP_DROP_MEMBERSHIP IP_DROP_MEMBERSHIP
#define IP_MULTICAST_IF    IP_MULTICAST_IF
#define IP_MULTICAST_LOOP  IP_MULTICAST_LOOP
#define IP_MULTICAST_TTL   IP_MULTICAST_TTL

#define IP_DEFAULT_MULTICAST_TTL  1
#define IP_DEFAULT_MULTICAST_LOOP 1

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_IP_H_ */
