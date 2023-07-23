#ifndef COSMOPOLITAN_LIBC_ISYSTEM_NETINET_IN_H_
#define COSMOPOLITAN_LIBC_ISYSTEM_NETINET_IN_H_
#include "libc/calls/weirdtypes.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/in6_pktinfo.h"
#include "libc/sock/struct/in_pktinfo.h"
#include "libc/sock/struct/ip_mreq.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr6.h"
#include "libc/sysv/consts/in.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/inet6.h"
#include "libc/sysv/consts/ip.h"
#include "libc/sysv/consts/ipport.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/ipv6.h"
#include "libc/sysv/consts/mcast.h"
#include "libc/sysv/consts/pf.h"
#include "libc/sysv/consts/sock.h"

# define IN6_ARE_ADDR_EQUAL(a,b) \
	((((const uint32_t *) (a))[0] == ((const uint32_t *) (b))[0])	      \
	 && (((const uint32_t *) (a))[1] == ((const uint32_t *) (b))[1])      \
	 && (((const uint32_t *) (a))[2] == ((const uint32_t *) (b))[2])      \
	 && (((const uint32_t *) (a))[3] == ((const uint32_t *) (b))[3]))

#endif /* COSMOPOLITAN_LIBC_ISYSTEM_NETINET_IN_H_ */
