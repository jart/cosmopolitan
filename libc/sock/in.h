#ifndef COSMOPOLITAN_LIBC_SOCK_IN_H_
#define COSMOPOLITAN_LIBC_SOCK_IN_H_
#include "libc/sock/sock.h"

#define IN_CLASSA(a)       ((((in_addr_t)(a)) & 0x80000000) == 0)
#define IN_CLASSA_NET      0xff000000
#define IN_CLASSA_NSHIFT   24
#define IN_CLASSA_HOST     (0xffffffff & ~IN_CLASSA_NET)
#define IN_CLASSA_MAX      128
#define IN_CLASSB(a)       ((((in_addr_t)(a)) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET      0xffff0000
#define IN_CLASSB_NSHIFT   16
#define IN_CLASSB_HOST     (0xffffffff & ~IN_CLASSB_NET)
#define IN_CLASSB_MAX      65536
#define IN_CLASSC(a)       ((((in_addr_t)(a)) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET      0xffffff00
#define IN_CLASSC_NSHIFT   8
#define IN_CLASSC_HOST     (0xffffffff & ~IN_CLASSC_NET)
#define IN_CLASSD(a)       ((((in_addr_t)(a)) & 0xf0000000) == 0xe0000000)
#define IN_MULTICAST(a)    IN_CLASSD(a)
#define IN_EXPERIMENTAL(a) ((((in_addr_t)(a)) & 0xe0000000) == 0xe0000000)
#define IN_BADCLASS(a)     ((((in_addr_t)(a)) & 0xf0000000) == 0xf0000000)

#define IN6_IS_ADDR_UNSPECIFIED(a)                           \
  (((uint32_t *)(a))[0] == 0 && ((uint32_t *)(a))[1] == 0 && \
   ((uint32_t *)(a))[2] == 0 && ((uint32_t *)(a))[3] == 0)

#define IN6_IS_ADDR_LOOPBACK(a)                              \
  (((uint32_t *)(a))[0] == 0 && ((uint32_t *)(a))[1] == 0 && \
   ((uint32_t *)(a))[2] == 0 && ((uint8_t *)(a))[12] == 0 && \
   ((uint8_t *)(a))[13] == 0 && ((uint8_t *)(a))[14] == 0 && \
   ((uint8_t *)(a))[15] == 1)

#define IN6_IS_ADDR_MULTICAST(a) (((uint8_t *)(a))[0] == 0xff)

#define IN6_IS_ADDR_LINKLOCAL(a) \
  ((((uint8_t *)(a))[0]) == 0xfe && (((uint8_t *)(a))[1] & 0xc0) == 0x80)

#define IN6_IS_ADDR_SITELOCAL(a) \
  ((((uint8_t *)(a))[0]) == 0xfe && (((uint8_t *)(a))[1] & 0xc0) == 0xc0)

#define IN6_IS_ADDR_V4MAPPED(a)                              \
  (((uint32_t *)(a))[0] == 0 && ((uint32_t *)(a))[1] == 0 && \
   ((uint8_t *)(a))[8] == 0 && ((uint8_t *)(a))[9] == 0 &&   \
   ((uint8_t *)(a))[10] == 0xff && ((uint8_t *)(a))[11] == 0xff)

#define IN6_IS_ADDR_V4COMPAT(a)                              \
  (((uint32_t *)(a))[0] == 0 && ((uint32_t *)(a))[1] == 0 && \
   ((uint32_t *)(a))[2] == 0 && ((uint8_t *)(a))[15] > 1)

#define IN6_IS_ADDR_MC_NODELOCAL(a) \
  (IN6_IS_ADDR_MULTICAST(a) && ((((uint8_t *)(a))[1] & 0xf) == 0x1))

#define IN6_IS_ADDR_MC_LINKLOCAL(a) \
  (IN6_IS_ADDR_MULTICAST(a) && ((((uint8_t *)(a))[1] & 0xf) == 0x2))

#define IN6_IS_ADDR_MC_SITELOCAL(a) \
  (IN6_IS_ADDR_MULTICAST(a) && ((((uint8_t *)(a))[1] & 0xf) == 0x5))

#define IN6_IS_ADDR_MC_ORGLOCAL(a) \
  (IN6_IS_ADDR_MULTICAST(a) && ((((uint8_t *)(a))[1] & 0xf) == 0x8))

#define IN6_IS_ADDR_MC_GLOBAL(a) \
  (IN6_IS_ADDR_MULTICAST(a) && ((((uint8_t *)(a))[1] & 0xf) == 0xe))

#define __ARE_4_EQUAL(a, b) \
  (!((0 [a] - 0 [b]) | (1 [a] - 1 [b]) | (2 [a] - 2 [b]) | (3 [a] - 3 [b])))
#define IN6_ARE_ADDR_EQUAL(a, b) \
  __ARE_4_EQUAL((const uint32_t *)(a), (const uint32_t *)(b))

#endif /* COSMOPOLITAN_LIBC_SOCK_IN_H_ */
