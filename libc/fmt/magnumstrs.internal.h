#ifndef COSMOPOLITAN_LIBC_FMT_MAGNUMSTRS_H_
#define COSMOPOLITAN_LIBC_FMT_MAGNUMSTRS_H_

#define MAGNUM_TERMINATOR -123

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define MAGNUM_NUMBER(TABLE, INDEX) \
  *(const int *)((uintptr_t)TABLE + TABLE[INDEX].x)

#define MAGNUM_STRING(TABLE, INDEX) \
  (const char *)((uintptr_t)TABLE + TABLE[INDEX].s)

struct MagnumStr {
  int x, s;
};

hidden extern const struct MagnumStr kClockNames[];
hidden extern const struct MagnumStr kErrnoDocs[];
hidden extern const struct MagnumStr kErrnoNames[];
hidden extern const struct MagnumStr kFcntlCmds[];
hidden extern const struct MagnumStr kIpOptnames[];
hidden extern const struct MagnumStr kOpenFlags[];
hidden extern const struct MagnumStr kRlimitNames[];
hidden extern const struct MagnumStr kSignalNames[];
hidden extern const struct MagnumStr kSockOptnames[];
hidden extern const struct MagnumStr kTcpOptnames[];

char *GetMagnumStr(const struct MagnumStr *, int);
char *DescribeMagnum(char *, const struct MagnumStr *, const char *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_MAGNUMSTRS_H_ */
