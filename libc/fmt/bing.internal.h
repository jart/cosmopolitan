#ifndef COSMOPOLITAN_LIBC_FMT_BING_H_
#define COSMOPOLITAN_LIBC_FMT_BING_H_

int bing(int, int) nosideeffect;
int unbing(int) nosideeffect;
void *unbingbuf(void *, size_t, const char16_t *, int);
void *unbingstr(const char16_t *) paramsnonnull() mallocesque;

#endif /* COSMOPOLITAN_LIBC_FMT_BING_H_ */
