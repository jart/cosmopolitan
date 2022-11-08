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

_Hide extern const struct MagnumStr kClockNames[];
_Hide extern const struct MagnumStr kErrnoDocs[];
_Hide extern const struct MagnumStr kErrnoNames[];
_Hide extern const struct MagnumStr kFcntlCmds[];
_Hide extern const struct MagnumStr kIpOptnames[];
_Hide extern const struct MagnumStr kOpenFlags[];
_Hide extern const struct MagnumStr kRlimitNames[];
_Hide extern const struct MagnumStr kSignalNames[];
_Hide extern const struct MagnumStr kSockOptnames[];
_Hide extern const struct MagnumStr kTcpOptnames[];

char *GetMagnumStr(const struct MagnumStr *, int);
char *DescribeMagnum(char *, const struct MagnumStr *, const char *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_MAGNUMSTRS_H_ */
