#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_IFCONF_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_IFCONF_H_
#include "libc/sock/struct/ifreq.h"
COSMOPOLITAN_C_START_

/*
 * Structure used in SIOCGIFCONF request.
 * Used to retrieve interface configuration
 * for machine (useful for programs which
 * must know all networks accessible).
 */
struct ifconf {
  int32_t ifc_len; /* size of buffer */
  int32_t padding;
  union {
    char *ifcu_buf;
    struct ifreq *ifcu_req;
  } ifc_ifcu;
};

/* Shortcuts to the ifconf buffer or ifreq array */
#define ifc_buf ifc_ifcu.ifcu_buf /* buffer address   */
#define ifc_req ifc_ifcu.ifcu_req /* array of structures  */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_IFCONF_H_ */
