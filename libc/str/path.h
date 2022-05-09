#ifndef COSMOPOLITAN_LIBC_STR_PATH_H_
#define COSMOPOLITAN_LIBC_STR_PATH_H_

#define _PATH_ABS  1
#define _PATH_DEV  2
#define _PATH_ROOT 4
#define _PATH_DOS  8
#define _PATH_WIN  16
#define _PATH_NT   32

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int _classifypath(const char *) libcesque nosideeffect;
bool _isabspath(const char *) libcesque strlenesque;
bool _isdirsep(int) libcesque pureconst;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_PATH_H_ */
