#ifndef COSMOPOLITAN_NET_HTTP_BASE64_H_
#define COSMOPOLITAN_NET_HTTP_BASE64_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *EncodeBase64(const void *, size_t, size_t *);
void *DecodeBase64(const char *, size_t, size_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_NET_HTTP_BASE64_H_ */
