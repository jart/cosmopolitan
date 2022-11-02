#ifndef COSMOPOLITAN_LIBC_X_H_
#define COSMOPOLITAN_LIBC_X_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int xwrite(int, const void *, uint64_t);
void xdie(void) wontreturn;
char *xdtoa(double)
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
char *xdtoaf(float)
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
char *xdtoal(long double)
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
void *xmalloc(size_t) attributeallocsize((1))
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
void *xrealloc(void *, size_t)
    attributeallocsize((2)) dontthrow nocallback dontdiscard;
void *xcalloc(size_t, size_t) attributeallocsize((1, 2))
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
void *xvalloc(size_t) attributeallocsize((1)) returnsaligned((FRAMESIZE))
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
void *xmemalign(size_t, size_t) attributeallocalign((1)) attributeallocsize((2))
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
void *xmemalignzero(size_t, size_t) attributeallocalign((1))
    attributeallocsize((2)) returnspointerwithnoaliases dontthrow nocallback
    dontdiscard returnsnonnull;
char *xstrdup(const char *) paramsnonnull()
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
char *xstrndup(const char *, size_t) paramsnonnull()
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
char *xstrcat(const char *, ...) paramsnonnull((1)) nullterminated()
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
#define xstrcat(...) (xstrcat)(__VA_ARGS__, 0)
char *xstrmul(const char *, size_t) paramsnonnull((1))
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
char *xinet_ntop(int, const void *) paramsnonnull()
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
void *xunbinga(size_t, const char16_t *)
    attributeallocalign((1)) returnspointerwithnoaliases dontthrow nocallback
    dontdiscard returnsnonnull dontthrow nocallback dontdiscard returnsnonnull;
void *xunbing(const char16_t *) returnspointerwithnoaliases dontthrow nocallback
    dontdiscard returnsnonnull dontthrow nocallback dontdiscard returnsnonnull;
char16_t *utf8to16(const char *, size_t, size_t *) dontdiscard;
char *utf16to8(const char16_t *, size_t, size_t *) dontdiscard;
wchar_t *utf8to32(const char *, size_t, size_t *) dontdiscard;
wchar_t *utf16to32(const char16_t *, size_t, size_t *) dontdiscard;
char *utf32to8(const wchar_t *, size_t, size_t *) dontdiscard;
char *xhomedir(void) dontdiscard;
char *xstripext(const char *) dontdiscard;
char *xstripexts(const char *) dontdiscard;
void *xload(_Atomic(void *) *, const void *, size_t, size_t);
void *xloadzd(_Atomic(void *) *, const void *, size_t, size_t, size_t, size_t,
              uint32_t);
int rmrf(const char *);
char *xbasename(const char *) paramsnonnull()
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
char *xdirname(const char *) paramsnonnull()
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
char *xjoinpaths(const char *, const char *) paramsnonnull()
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
char *xreadlink(const char *) paramsnonnull()
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
char *xreadlinkat(int, const char *) paramsnonnull()
    returnspointerwithnoaliases dontthrow nocallback dontdiscard returnsnonnull;
void xfixpath(void);
void *xslurp(const char *, size_t *)
    paramsnonnull((1)) returnspointerwithnoaliases
    returnsaligned((PAGESIZE)) dontdiscard;
int xbarf(const char *, const void *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_X_H_ */
