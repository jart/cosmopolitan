#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_NEXGEN32E_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_NEXGEN32E_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void imapxlatab(void *);
void insertionsort(int32_t *, size_t);

int64_t div10int64(int64_t) libcesque pureconst;
int64_t div100int64(int64_t) libcesque pureconst;
int64_t div1000int64(int64_t) libcesque pureconst;
int64_t div10000int64(int64_t) libcesque pureconst;
int64_t div1000000int64(int64_t) libcesque pureconst;
int64_t div1000000000int64(int64_t) libcesque pureconst;

int64_t rem10int64(int64_t) libcesque pureconst;
int64_t rem100int64(int64_t) libcesque pureconst;
int64_t rem1000int64(int64_t) libcesque pureconst;
int64_t rem10000int64(int64_t) libcesque pureconst;
int64_t rem1000000int64(int64_t) libcesque pureconst;
int64_t rem1000000000int64(int64_t) libcesque pureconst;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_NEXGEN32E_H_ */
