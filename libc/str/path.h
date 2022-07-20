#ifndef COSMOPOLITAN_LIBC_STR_PATH_H_
#define COSMOPOLITAN_LIBC_STR_PATH_H_

#define _kPathAbs  1
#define _kPathDev  2
#define _kPathRoot 4
#define _kPathDos  8
#define _kPathWin  16
#define _kPathNt   32

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int _classifypath(const char *) libcesque nosideeffect;
bool _isabspath(const char *) libcesque strlenesque;
bool _isdirsep(int) libcesque pureconst;
char *_joinpaths(char *, size_t, const char *, const char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_PATH_H_ */
