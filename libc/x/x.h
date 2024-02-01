#ifndef COSMOPOLITAN_LIBC_X_H_
#define COSMOPOLITAN_LIBC_X_H_

#ifdef _COSMO_SOURCE
#define xwrite        __xwrite
#define xdie          __xdie
#define xmalloc       __xmalloc
#define xrealloc      __xrealloc
#define xcalloc       __xcalloc
#define xvalloc       __xvalloc
#define xmemalign     __xmemalign
#define xmemalignzero __xmemalignzero
#define xstrdup       __xstrdup
#define xstrndup      __xstrndup
#define xstrmul       __xstrmul
#define xinet_ntop    __xinet_ntop
#define xunbinga      __xunbinga
#define xunbing       __xunbing
#define utf8to16      __utf8to16
#define utf16to8      __utf16to8
#define utf8to32      __utf8to32
#define utf16to32     __utf16to32
#define utf32to8      __utf32to8
#define xhomedir      __xhomedir
#define xstripext     __xstripext
#define xstripexts    __xstripexts
#define xload         __xload
#define rmrf          __rmrf
#define xbasename     __xbasename
#define xdirname      __xdirname
#define xjoinpaths    __xjoinpaths
#define xfixpath      __xfixpath
#define xslurp        __xslurp
#define xbarf         __xbarf
#endif /* _COSMO_SOURCE */

#ifdef _COSMO_SOURCE
COSMOPOLITAN_C_START_

int xwrite(int, const void *, uint64_t);
void xdie(void) wontreturn;
char *xdtoa(double)
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
char *xdtoaf(float)
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
char *xdtoal(long double)
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
void *xmalloc(size_t) attributeallocsize((1))
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
void *xrealloc(void *, size_t)
    attributeallocsize((2)) dontthrow dontcallback __wur;
void *xcalloc(size_t, size_t) attributeallocsize((1, 2))
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
void *xvalloc(size_t) attributeallocsize((1)) returnsaligned((65536))
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
void *xmemalign(size_t, size_t) attributeallocalign((1)) attributeallocsize((2))
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
void *xmemalignzero(size_t, size_t) attributeallocalign((1))
    attributeallocsize((2))
        returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
char *xstrdup(const char *) paramsnonnull()
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
char *xstrndup(const char *, size_t) paramsnonnull()
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
char *xstrcat(const char *, ...) paramsnonnull((1)) nullterminated()
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
#define xstrcat(...) (xstrcat)(__VA_ARGS__, NULL)
char *xstrmul(const char *, size_t) paramsnonnull((1))
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
char *xinet_ntop(int, const void *) paramsnonnull()
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
void *xunbinga(size_t, const char16_t *)
    attributeallocalign((1)) returnspointerwithnoaliases dontthrow dontcallback
    __wur returnsnonnull dontthrow dontcallback __wur returnsnonnull;
void *xunbing(const char16_t *)
    returnspointerwithnoaliases dontthrow dontcallback __wur
    returnsnonnull dontthrow dontcallback __wur returnsnonnull;
char16_t *utf8to16(const char *, size_t, size_t *) __wur;
char *utf16to8(const char16_t *, size_t, size_t *) __wur;
wchar_t *utf8to32(const char *, size_t, size_t *) __wur;
wchar_t *utf16to32(const char16_t *, size_t, size_t *) __wur;
char *utf32to8(const wchar_t *, size_t, size_t *) __wur;
char *xhomedir(void) __wur;
char *xstripext(const char *) __wur;
char *xstripexts(const char *) __wur;
void *xload(_Atomic(void *) *, const void *, size_t, size_t);
int rmrf(const char *);
char *xbasename(const char *) paramsnonnull()
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
char *xdirname(const char *) paramsnonnull()
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
char *xjoinpaths(const char *, const char *) paramsnonnull()
    returnspointerwithnoaliases dontthrow dontcallback __wur returnsnonnull;
void xfixpath(void);
void *xslurp(const char *, size_t *)
    paramsnonnull((1)) returnspointerwithnoaliases returnsaligned((4096)) __wur;
int xbarf(const char *, const void *, size_t);

COSMOPOLITAN_C_END_
#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_X_H_ */
