#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SF_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SF_H_
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

#define SF_MNOWAIT  SF_MNOWAIT
#define SF_NODISKIO SF_NODISKIO
#define SF_SYNC     SF_SYNC


COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SF_H_ */
