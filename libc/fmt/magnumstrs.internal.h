#ifndef COSMOPOLITAN_LIBC_FMT_MAGNUMSTRS_H_
#define COSMOPOLITAN_LIBC_FMT_MAGNUMSTRS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct MagnumStr {
  int x, s;
};

extern const struct MagnumStr kErrnoDocs[];
extern const struct MagnumStr kErrnoNames[];
extern const struct MagnumStr kIpOptnames[];
extern const struct MagnumStr kSignalNames[];
extern const struct MagnumStr kSockOptnames[];
extern const struct MagnumStr kTcpOptnames[];

const char *DescribeSockLevel(int);
const char *DescribeSockOptname(int, int);
const char *GetMagnumStr(const struct MagnumStr *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_MAGNUMSTRS_H_ */
