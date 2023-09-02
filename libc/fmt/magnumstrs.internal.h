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

extern const struct MagnumStr kClockNames[];
extern const struct MagnumStr kErrnoDocs[];
extern const struct MagnumStr kErrnoNames[];
extern const struct MagnumStr kFcntlCmds[];
extern const struct MagnumStr kIpOptnames[];
extern const struct MagnumStr kOpenFlags[];
extern const struct MagnumStr kRlimitNames[];
extern const struct MagnumStr kSignalNames[];
extern const struct MagnumStr kSockOptnames[];
extern const struct MagnumStr kTcpOptnames[];
extern const struct MagnumStr kPollNames[];

const char *DescribeMagnum(char *, const struct MagnumStr *, const char *, int);

__funline const char *GetMagnumStr(const struct MagnumStr *ms, int x) {
  int i;
  for (i = 0; ms[i].x != MAGNUM_TERMINATOR; ++i) {
    if (x == MAGNUM_NUMBER(ms, i)) {
      return MAGNUM_STRING(ms, i);
    }
  }
  return 0;
}

/**
 * Converts errno value to descriptive sentence.
 * @return non-null rodata string or null if not found
 */
__funline const char *_strerdoc(int x) {
  if (x) {
    return GetMagnumStr(kErrnoDocs, x);
  } else {
    return 0;
  }
}

/**
 * Converts errno value to symbolic name.
 * @return non-null rodata string or null if not found
 */
__funline const char *_strerrno(int x) {
  if (x) {
    return GetMagnumStr(kErrnoNames, x);
  } else {
    return 0;
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_MAGNUMSTRS_H_ */
