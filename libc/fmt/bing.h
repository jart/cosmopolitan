#ifndef COSMOPOLITAN_LIBC_FMT_BING_H_
#define COSMOPOLITAN_LIBC_FMT_BING_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int bing(int, int) nosideeffect;
int unbing(int) nosideeffect;
void *unbingbuf(void *, size_t, const char16_t *, int);
void *unbingstr(const char16_t *) paramsnonnull() mallocesque;
void *unhexbuf(void *, size_t, const char *);
void *unhexstr(const char *) mallocesque;
short *bingblit(int ys, int xs, unsigned char[ys][xs], int, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_BING_H_ */
