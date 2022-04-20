#ifndef COSMOPOLITAN_LIBC_INTRIN_TLS_H_
#define COSMOPOLITAN_LIBC_INTRIN_TLS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

uint32_t TlsAlloc(void);
bool32 TlsFree(uint32_t);
bool32 TlsSetValue(uint32_t, void *);
void *TlsGetValue(uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_TLS_H_ */
