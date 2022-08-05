#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SF_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SF_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct sf_hdtr {
  struct iovec *headers;
  int hdr_cnt;
  struct iovec *trailers;
  int trl_cnt;
};

extern const int SF_MNOWAIT;
extern const int SF_NODISKIO;
extern const int SF_SYNC;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define SF_MNOWAIT  SYMBOLIC(SF_MNOWAIT)
#define SF_NODISKIO SYMBOLIC(SF_NODISKIO)
#define SF_SYNC     SYMBOLIC(SF_SYNC)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SF_H_ */
