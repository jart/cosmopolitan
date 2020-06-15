#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_ETH_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_ETH_H_
#include "libc/runtime/symbolic.h"

#define ETH_P_CUST SYMBOLIC(ETH_P_CUST)
#define ETH_P_DDCMP SYMBOLIC(ETH_P_DDCMP)
#define ETH_P_DEC SYMBOLIC(ETH_P_DEC)
#define ETH_P_DIAG SYMBOLIC(ETH_P_DIAG)
#define ETH_P_DNA_DL SYMBOLIC(ETH_P_DNA_DL)
#define ETH_P_DNA_RC SYMBOLIC(ETH_P_DNA_RC)
#define ETH_P_DNA_RT SYMBOLIC(ETH_P_DNA_RT)
#define ETH_P_IEEE802154 SYMBOLIC(ETH_P_IEEE802154)
#define ETH_P_LAT SYMBOLIC(ETH_P_LAT)
#define ETH_P_LOCALTALK SYMBOLIC(ETH_P_LOCALTALK)
#define ETH_P_PPP_MP SYMBOLIC(ETH_P_PPP_MP)
#define ETH_P_RARP SYMBOLIC(ETH_P_RARP)
#define ETH_P_SCA SYMBOLIC(ETH_P_SCA)
#define ETH_P_WAN_PPP SYMBOLIC(ETH_P_WAN_PPP)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long ETH_P_CUST;
hidden extern const long ETH_P_DDCMP;
hidden extern const long ETH_P_DEC;
hidden extern const long ETH_P_DIAG;
hidden extern const long ETH_P_DNA_DL;
hidden extern const long ETH_P_DNA_RC;
hidden extern const long ETH_P_DNA_RT;
hidden extern const long ETH_P_IEEE802154;
hidden extern const long ETH_P_LAT;
hidden extern const long ETH_P_LOCALTALK;
hidden extern const long ETH_P_PPP_MP;
hidden extern const long ETH_P_RARP;
hidden extern const long ETH_P_SCA;
hidden extern const long ETH_P_WAN_PPP;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_ETH_H_ */
