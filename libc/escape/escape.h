#ifndef COSMOPOLITAN_LIBC_ESCAPE_ESCAPE_H_
#define COSMOPOLITAN_LIBC_ESCAPE_ESCAPE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § escaping                                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

unsigned cescapec(int);
int escapec(char *, unsigned, const char *, unsigned)
    paramsnonnull((3)) nothrow nocallback;
int escapesh(char *, unsigned, const char *, unsigned) paramsnonnull((3));
bool escapedos(char16_t *, unsigned, const char16_t *, unsigned);
int aescapec(char **, const char *, unsigned) paramsnonnull();
int aescapesh(char **, const char *, unsigned) paramsnonnull();
int aescape(char **, size_t, const char *, unsigned,
            int (*)(char *, unsigned, const char *, unsigned)) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ESCAPE_ESCAPE_H_ */
