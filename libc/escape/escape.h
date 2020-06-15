#ifndef COSMOPOLITAN_LIBC_ESCAPE_ESCAPE_H_
#define COSMOPOLITAN_LIBC_ESCAPE_ESCAPE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § escaping                                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

unsigned cescapec(int c);
int escapec(char *buf, unsigned size, const char *unescaped, unsigned length)
    paramsnonnull((3)) nothrow nocallback;
int escapesh(char *buf, unsigned size, const char *unescaped, unsigned length)
    paramsnonnull((3));
bool escapedos(char16_t *buffer, unsigned buflen, const char16_t *unquoted,
               unsigned len);
int aescapec(char **escaped, const char *unescaped, unsigned length)
    paramsnonnull();
int aescapesh(char **escaped, const char *unescaped, unsigned length)
    paramsnonnull();
int aescape(char **escaped, size_t size, const char *unescaped, unsigned length,
            int impl(char *escaped, unsigned size, const char *unescaped,
                     unsigned length)) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ESCAPE_ESCAPE_H_ */
