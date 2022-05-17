#ifndef COSMOPOLITAN_LIBC_INTRIN_THREADED_H_
#define COSMOPOLITAN_LIBC_INTRIN_THREADED_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern bool __threaded;
extern bool __tls_enabled;
extern unsigned __tls_index;

void *__initialize_tls(char[hasatleast 64]);
void __install_tls(char[hasatleast 64]);
char *__get_tls(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_THREADED_H_ */
