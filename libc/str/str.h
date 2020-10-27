#ifndef COSMOPOLITAN_LIBC_STR_STR_H_
#define COSMOPOLITAN_LIBC_STR_STR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § characters » asa x3.4-1967                                ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  fourth age telecommunications */

extern const uint8_t gperf_downcase[256];
extern const uint8_t kToLower[256];
extern const uint8_t kToUpper[256];
extern const uint16_t kToLower16[256];
extern const uint8_t kBase36[256];
extern const char16_t kCp437[256];

int isascii(int);
int isspace(int);
int isalpha(int);
int isdigit(int);
int isalnum(int);
int isxdigit(int);
int isprint(int);
int islower(int);
int isupper(int);
int isblank(int);
int iscntrl(int);
int isgraph(int);
int tolower(int);
int ispunct(int);
int toupper(int);
int hextoint(int);
int cescapec(int);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § characters » thompson-pike encoding                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  fifth age telecommunications

      0123456789abcdef
    ┌0─ ☺☻♥♦♣♠•◘○◙♂♀♪♫☼┬───Control
    └1─►◄↕‼¶§▬↨↑↓→←∟↔▲▼┘
    ┌2─ !"#$%&'()*+,-./┐
    │3 0123456789:;<=>?│
    │4 @ABCDEFGHIJKLMNO├───ASA x3.4-1967
    │5 PQRSTUVWXYZ[\]^_│
    │6 `abcdefghijklmno│
    └7─pqrstuvwxyz{|}~⌂┘
    ┌8─ÇüéâäàåçêëèïîìÄÅ┐
    │9 ÉæÆôöòûùÿÖÜ¢£¥€ƒ├───Thompson-Pike Continuation
    │a á¡óúñÑªº¿⌐¬½¼¡«»│    (not really characters)
    └b─░▒▓│┤╡╢╖╕╣║╗╝╜╛┐┘
    ┌c─└┴┬├─┼╞╟╚╔╩╦╠═╬╧┬───1 Continuation will follow
    └d─╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀┘
    ─e─αßΓπΣσμτΦΘΩδ∞φε∩────2 Continuations will follow
     f─≡±≥≤⌠⌡÷≈°∙·√ⁿ²■λ
       │      ││  │││└┤
       │      ││  │└┤ └───5 Continuations follow (and is negative)
       │      │└─┬┘ └─────5 Continuations follow (note: -1=λ┐┐┐┐┐)
       └───┬──┘  └────────4 Continuations follow
           └──────────────3 Continuations follow */

#define INVALID_CODEPOINT 0xfffd

wint_t DecodeNtsUtf16(const char16_t **);
unsigned getutf16(const char16_t *, wint_t *);
int pututf16(char16_t *, size_t, wint_t, bool);
int iswalnum(wint_t);
int iswalpha(wint_t);
int iswblank(wint_t);
int iswcntrl(wint_t);
int iswdigit(wint_t);
int iswgraph(wint_t);
int iswlower(wint_t);
int iswspace(wint_t);
int iswupper(wint_t);
int iswxdigit(wint_t);
int iswpunct(wint_t);
int iswprint(wint_t);
wint_t towlower(wint_t);
wint_t towupper(wint_t);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § strings                                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

void *memset(void *, int, size_t) memcpyesque;
void *memcpy(void *restrict, const void *restrict, size_t) memcpyesque;
void *mempcpy(void *restrict, const void *restrict, size_t) memcpyesque;
void *memccpy(void *restrict, const void *restrict, int, size_t) memcpyesque;
void *memmove(void *, const void *, size_t) memcpyesque;
void *memeqmask(void *, const void *, const void *, size_t) memcpyesque;

size_t strlen(const char *) strlenesque;
size_t strnlen(const char *, size_t) strlenesque;
size_t strnlen_s(const char *, size_t);
char *strchr(const char *, int) strlenesque;
char *index(const char *, int) strlenesque;
void *memchr(const void *, int, size_t) strlenesque;
char *strchrnul(const char *, int) strlenesque returnsnonnull;
void *rawmemchr(const void *, int) strlenesque returnsnonnull;
void bzero(void *, size_t) paramsnonnull() libcesque;
void explicit_bzero(void *, size_t) paramsnonnull() libcesque;
size_t strlen16(const char16_t *) strlenesque;
size_t strnlen16(const char16_t *, size_t) strlenesque;
size_t strnlen16_s(const char16_t *, size_t);
char16_t *strchr16(const char16_t *, int) strlenesque;
void *memchr16(const void *, int, size_t) strlenesque;
char16_t *strchrnul16(const char16_t *, int) strlenesque returnsnonnull;
void *rawmemchr16(const void *, int) strlenesque returnsnonnull;
size_t wcslen(const wchar_t *) strlenesque;
size_t wcsnlen(const wchar_t *, size_t) strlenesque;
size_t wcsnlen_s(const wchar_t *, size_t);
wchar_t *wcschr(const wchar_t *, wchar_t) strlenesque;
wchar_t *wmemchr(const wchar_t *, wchar_t, size_t) strlenesque;
wchar_t *wcschrnul(const wchar_t *, wchar_t) strlenesque returnsnonnull;
char *strstr(const char *, const char *) strlenesque;
char16_t *strstr16(const char16_t *, const char16_t *) strlenesque;
wchar_t *wcsstr(const wchar_t *, const wchar_t *) strlenesque;
void *rawwmemchr(const void *, wchar_t) strlenesque returnsnonnull;
int memcmp(const void *, const void *, size_t) strlenesque;
int strcmp(const char *, const char *) strlenesque;
int strncmp(const char *, const char *, size_t) strlenesque;
int strcmp16(const char16_t *, const char16_t *) strlenesque;
int strncmp16(const char16_t *, const char16_t *, size_t) strlenesque;
int wcscmp(const wchar_t *, const wchar_t *) strlenesque;
int wcsncmp(const wchar_t *, const wchar_t *, size_t) strlenesque;
int wmemcmp(const wchar_t *, const wchar_t *, size_t) strlenesque;
int strcasecmp(const char *, const char *) strlenesque;
int strcasecmp16(const char16_t *, const char16_t *) strlenesque;
int wcscasecmp(const wchar_t *, const wchar_t *) strlenesque;
int strncasecmp(const char *, const char *, size_t) strlenesque;
int strncasecmp16(const char16_t *, const char16_t *, size_t) strlenesque;
int wcsncasecmp(const wchar_t *, const wchar_t *, size_t) strlenesque;
char *strrchr(const char *, int) strlenesque;
void *memrchr(const void *, int, size_t) strlenesque;
char16_t *strrchr16(const char16_t *, int) strlenesque;
void *memrchr16(const void *, int, size_t) strlenesque;
wchar_t *wcsrchr(const wchar_t *, int) strlenesque;
void *wmemrchr(const void *, wchar_t, size_t) strlenesque;
char *strpbrk(const char *, const char *) strlenesque;
char16_t *strpbrk16(const char16_t *, const char16_t *) strlenesque;
wchar_t *wcspbrk(const wchar_t *, const wchar_t *) strlenesque;
size_t strspn(const char *, const char *) strlenesque;
size_t strspn16(const char16_t *, const char16_t *) strlenesque;
size_t wcsspn(const wchar_t *, const wchar_t *) strlenesque;
size_t strcspn(const char *, const char *) strlenesque;
size_t strcspn16(const char16_t *, const char16_t *) strlenesque;
size_t wcscspn(const wchar_t *, const wchar_t *) strlenesque;
void *memfrob(void *, size_t) memcpyesque;
int strcoll(const char *, const char *) strlenesque;
char *strsep(char **, const char *) paramsnonnull();
int strcmpzbw(const uint16_t *, const char *) strlenesque;
int strcasecmpzbw(const uint16_t *, const char *) strlenesque;
char *stpcpy(char *, const char *) memcpyesque;
char *stpncpy(char *, const char *, size_t) memcpyesque;
char *strcat(char *, const char *) memcpyesque;
size_t strlcpy(char *, const char *, size_t);
size_t strlcat(char *, const char *, size_t);
char *strcpy(char *, const char *) memcpyesque;
char16_t *strcpy16(char16_t *, const char16_t *) memcpyesque;
char *strncat(char *, const char *, size_t) memcpyesque;
char *strncpy(char *, const char *, size_t) memcpyesque;
char *strtok(char *, const char *) paramsnonnull((2)) libcesque;
char *strtok_r(char *, const char *, char **) paramsnonnull((2, 3));
uint16_t *strcpyzbw(uint16_t *, const char *) memcpyesque;
char *wstrtrunc(uint16_t *) memcpyesque;
char *wstrntrunc(uint16_t *, size_t) memcpyesque;
bool startswith(const char *, const char *) strlenesque;
bool startswith16(const char16_t *, const char16_t *) strlenesque;
bool wcsstartswith(const wchar_t *, const wchar_t *) strlenesque;
bool endswith(const char *, const char *) strlenesque;
bool endswith16(const char16_t *, const char16_t *) strlenesque;
bool wcsendswith(const wchar_t *, const wchar_t *) strlenesque;
const char *IndexDoubleNulString(const char *, unsigned) strlenesque;
int getkvlin(const char *, const char *const[]);
wchar_t *wmemset(wchar_t *, wchar_t, size_t) memcpyesque;
char16_t *memset16(char16_t *, char16_t, size_t) memcpyesque;
compatfn wchar_t *wmemcpy(wchar_t *, const wchar_t *, size_t) memcpyesque;
compatfn wchar_t *wmempcpy(wchar_t *, const wchar_t *, size_t) memcpyesque;
compatfn wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t) memcpyesque;

char *tinystrstr(const char *, const char *) strlenesque;
char16_t *tinystrstr16(const char16_t *, const char16_t *) strlenesque;
void *tinymemmem(const void *, size_t, const void *, size_t) strlenesque;

void *memtolower(void *, size_t);
char *strntolower(char *, size_t);
char *strtolower(char *) paramsnonnull();
char *strntoupper(char *, size_t);
char *strtoupper(char *) paramsnonnull();
char *chomp(char *);
char16_t *chomp16(char16_t *);
wchar_t *wchomp(wchar_t *);

bool escapedos(char16_t *, unsigned, const char16_t *, unsigned);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § strings » multibyte                                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef unsigned mbstate_t;

size_t tprecode8to16(char16_t *, size_t, const char *);
size_t tprecode16to8(char *, size_t, const char16_t *);
int strcmp8to16(const char *, const char16_t *) strlenesque;
int strncmp8to16(const char *, const char16_t *, size_t) strlenesque;
int strcasecmp8to16(const char *, const char16_t *) strlenesque;
int strncasecmp8to16(const char *, const char16_t *, size_t) strlenesque;
int strcmp16to8(const char16_t *, const char *) strlenesque;
int strncmp16to8(const char16_t *, const char *, size_t) strlenesque;
int strcasecmp16to8(const char16_t *, const char *) strlenesque;
int strncasecmp16to8(const char16_t *, const char *, size_t) strlenesque;
wchar_t *wcsncpy(wchar_t *, const wchar_t *, size_t);
int mbtowc(wchar_t *, const char *, size_t);
size_t mbrtowc(wchar_t *, const char *, size_t, mbstate_t *);
size_t mbsrtowcs(wchar_t *, const char **, size_t, mbstate_t *);
size_t mbstowcs(wchar_t *, const char *, size_t);
size_t wcstombs(char *, const wchar_t *, size_t);
size_t wcsrtombs(char *, const wchar_t **, size_t, mbstate_t *);
size_t wcrtomb(char *, wchar_t, mbstate_t *);
int wctomb(char *, wchar_t);
int wctob(wint_t);

size_t strclen(const char *) nosideeffect;
size_t strnclen(const char *, size_t) nosideeffect;
size_t strclen16(const char16_t *) nosideeffect;
size_t strnclen16(const char16_t *, size_t) nosideeffect;

typedef unsigned wctype_t;
wctype_t wctype(const char *) strlenesque;
int iswctype(wint_t, wctype_t) pureconst;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § strings » hashing                                         ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define SHA256_BLOCK_SIZE 32

struct Sha256Ctx {
  uint8_t data[64];
  uint32_t datalen;
  uint64_t bitlen;
  uint32_t state[8];
};

void sha256_init(struct Sha256Ctx *);
void sha256_update(struct Sha256Ctx *, const uint8_t *, size_t);
void sha256_final(struct Sha256Ctx *, uint8_t *);

bool luhn(const char *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § strings » system                                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *strsignal(int) returnsnonnull libcesque;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § strings » hooks                                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
extern int (*const hook$strcmp16)(const char16_t *, const char16_t *);
extern int (*const hook$strncmp16)(const char16_t *, const char16_t *, size_t);
extern int (*const hook$wcscmp)(const wchar_t *, const wchar_t *);
extern int (*const hook$wcsncmp)(const wchar_t *, const wchar_t *, size_t);
#define __STR_HOOK(SYMBOL) hook$##SYMBOL
#else
#define __STR_HOOK(SYMBOL) SYMBOL
#endif /* GNUC && !ANSI */

/* TODO(jart): Use @gotpcrel. */
#undef __STR_HOOK
#define __STR_HOOK(SYMBOL) SYMBOL

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § strings » generic typing                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if __STDC_VERSION__ + 0 >= 201112

#define strnlen(s, n)           \
  _Generic(*(s), wchar_t        \
           : wcsnlen, char16_t  \
           : strnlen16, default \
           : strnlen)(s, n)

#define strnlen_s(s, n)           \
  _Generic(*(s), wchar_t          \
           : wcsnlen_s, char16_t  \
           : strnlen16_s, default \
           : strnlen_s)(s, n)

#define strpbrk(s, c)           \
  _Generic(*(s), wchar_t        \
           : wcspbrk, char16_t  \
           : strpbrk16, default \
           : strpbrk)(s, c)

#define strspn(s, c) \
  _Generic(*(s), wchar_t : wcsspn, char16_t : strspn16, default : strspn)(s, c)

#define strcspn(s, c)           \
  _Generic(*(s), wchar_t        \
           : wcscspn, char16_t  \
           : strcspn16, default \
           : strcspn)(s, c)

/* clang-format off */
#define strcmp(s1, s2)                \
  _Generic((s1)[0],                   \
    wchar_t: __STR_HOOK(wcscmp),      \
    char16_t: _Generic(*(s2),         \
      char: strcmp16to8,              \
      default: __STR_HOOK(strcmp16)), \
    default: _Generic(*(s2),          \
      char16_t: strcmp8to16,          \
      default: strcmp))(s1, s2)
/* clang-format on */

#define strncmp(s1, s2, n)                      \
  _Generic(*(s1), wchar_t                       \
           : __STR_HOOK(wcsncmp), char16_t      \
           : _Generic(*(s2), char               \
                      : strncmp16to8, default   \
                      : __STR_HOOK(strncmp16)), \
             default                            \
           : _Generic(*(s2), char16_t           \
                      : strncmp8to16, default   \
                      : strncmp))(s1, s2, n)

#define strcasecmp(s1, s2)         \
  _Generic(*(s1), wchar_t          \
           : wcscasecmp, char16_t  \
           : strcasecmp16, default \
           : strcasecmp)(s1, s2)

#define strncasecmp(s1, s2, n)      \
  _Generic(*(s1), wchar_t           \
           : wcsncasecmp, char16_t  \
           : strncasecmp16, default \
           : strncasecmp)(s1, s2, n)

#define chomp(s) \
  _Generic(*(s), wchar_t : wchomp, char16_t : chomp16, default : chomp)(s)

#endif /* C11 */

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § strings » optimizations                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

extern int (*const __memcmp)(const void *, const void *, size_t);
#define memcmp(a, b, n) __memcmp(a, b, n)

/* gcc -Werror=stringop-truncation misunderstands strncpy() api */
char *_strncpy(char *, const char *, size_t) asm("strncpy") memcpyesque;
#define strncpy(DEST, SRC, N) _strncpy(DEST, SRC, N)

#define explicit_bzero(STR, BYTES)                                          \
  do {                                                                      \
    void *Str;                                                              \
    size_t Bytes;                                                           \
    asm volatile("call\texplicit_bzero"                                     \
                 : "=D"(Str), "=S"(Bytes)                                   \
                 : "0"(STR), "1"(BYTES)                                     \
                 : "rax", "rcx", "rdx", "r8", "r9", "r10", "r11", "memory", \
                   "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");   \
  } while (0)

#ifdef UNBLOAT_STDARG
#define __STR_XMM_CLOBBER
#else
#define __STR_XMM_CLOBBER "xmm3", "xmm4",
#endif

#define __memcpy_isgoodsize(SIZE)                              \
  (isconstant(SIZE) && ((SIZE) <= __BIGGEST_ALIGNMENT__ * 2 && \
                        __builtin_popcount((unsigned)(SIZE)) == 1))

#define __memset_isgoodsize(SIZE)                                      \
  (isconstant(SIZE) && (((SIZE) <= __BIGGEST_ALIGNMENT__ &&            \
                         __builtin_popcount((unsigned)(SIZE)) == 1) || \
                        ((SIZE) % __BIGGEST_ALIGNMENT__ == 0 &&        \
                         (SIZE) / __BIGGEST_ALIGNMENT__ <= 3)))

#define memcpy(DEST, SRC, SIZE)                                  \
  (__memcpy_isgoodsize(SIZE) ? __builtin_memcpy(DEST, SRC, SIZE) \
                             : __memcpy("MemCpy", DEST, SRC, SIZE))

#define memset(DEST, BYTE, SIZE)                                  \
  (__memset_isgoodsize(SIZE) ? __builtin_memset(DEST, BYTE, SIZE) \
                             : __memset(DEST, BYTE, SIZE))

#if defined(__STDC_HOSTED__) && (defined(__SSE2__) || defined(UNBLOAT_STDARG))

#define memmove(DEST, SRC, SIZE) __memcpy("MemMove", (DEST), (SRC), (SIZE))

#define mempcpy(DEST, SRC, SIZE)                          \
  ({                                                      \
    size_t SIze = (SIZE);                                 \
    (void *)((char *)memcpy((DEST), (SRC), SIze) + SIze); \
  })

#define __memcpy(FN, DEST, SRC, SIZE)                                      \
  ({                                                                       \
    void *DeSt = (DEST);                                                   \
    const void *SrC = (SRC);                                               \
    size_t SiZe = (SIZE);                                                  \
    asm("call\t" FN                                                        \
        : "=m"(*(char(*)[SiZe])(DeSt))                                     \
        : "D"(DeSt), "S"(SrC), "d"(SiZe), "m"(*(const char(*)[SiZe])(SrC)) \
        : __STR_XMM_CLOBBER "rcx", "cc");                                  \
    DeSt;                                                                  \
  })

#define __memset(DEST, BYTE, SIZE)        \
  ({                                      \
    void *DeSt = (DEST);                  \
    size_t SiZe = (SIZE);                 \
    asm("call\tMemSet"                    \
        : "=m"(*(char(*)[SiZe])(DeSt))    \
        : "D"(DeSt), "S"(BYTE), "d"(SiZe) \
        : __STR_XMM_CLOBBER "rcx", "cc"); \
    DeSt;                                 \
  })

#else /* hosted/sse2/unbloat */

#define mempcpy(DEST, SRC, SIZE)                                           \
  ({                                                                       \
    void *Rdi, *Dest = (DEST);                                             \
    const void *Rsi, *Src = (SRC);                                         \
    size_t SiZe = (SIZE);                                                  \
    size_t Rcx;                                                            \
    asm("rep movsb"                                                        \
        : "=D"(Rdi), "=S"(Rsi), "=D"(Rcx), "=m"(*(char(*)[SiZe])(Dest))    \
        : "0"(Dest), "1"(Src), "2"(SiZe), "m"(*(const char(*)[SiZe])(Src)) \
        : "cc");                                                           \
    Rdi;                                                                   \
  })

#define __memcpy(FN, DEST, SRC, SIZE)                                      \
  ({                                                                       \
    void *Rdi, *Dest = (DEST);                                             \
    const void *Rsi, *Src = (SRC);                                         \
    size_t SiZe = (SIZE);                                                  \
    size_t Rcx;                                                            \
    asm("rep movsb"                                                        \
        : "=D"(Rdi), "=S"(Rsi), "=c"(Rcx), "=m"(*(char(*)[SiZe])(Dest))    \
        : "0"(Dest), "1"(Src), "2"(SiZe), "m"(*(const char(*)[SiZe])(Src)) \
        : "cc");                                                           \
    Dest;                                                                  \
  })

#define __memset(DEST, BYTE, SIZE)                           \
  ({                                                         \
    void *Rdi, *Dest = (DEST);                               \
    size_t SiZe = (SIZE);                                    \
    size_t Rcx;                                              \
    asm("rep stosb"                                          \
        : "=D"(Rdi), "=c"(Rcx), "=m"(*(char(*)[SiZe])(Dest)) \
        : "0"(Dest), "1"(SiZe), "a"(BYTE)                    \
        : "cc");                                             \
    Dest;                                                    \
  })

#endif /* hosted/sse2/unbloat */

#define pututf16(BUF, SIZE, CH, AWESOME) __pututf16(BUF, SIZE, CH, AWESOME)
#define getutf16(BUF, CHPTR)             __getutf16(BUF, CHPTR)
size_t _strlen(const char *s) asm("strlen") strlenesque;
void *_memchr(const void *, int, size_t) asm("memchr") strlenesque;

forceinline int __pututf16(char16_t *s, size_t size, wint_t wc,
                           bool32 awesome) {
  if (size >= 1 && (0x00 <= wc && wc <= 0xD7FF)) {
    if (wc >= 32 || !awesome) {
      s[0] = (char16_t)wc;
      return 1;
    } else if (size >= 2) {
      s[0] = 0xd800;
      s[1] = 0xdc00 | (char16_t)wc;
      return 2;
    }
  }
  int ax;
  asm("call\tpututf16"
      : "=a"(ax), "=m"(*(char(*)[size])s)
      : "D"(s), "S"(size), "d"(wc)
      : "cc");
  return ax;
}

forceinline unsigned __getutf16(const char16_t *s, wint_t *wc) {
  if ((0x00 <= s[0] && s[0] <= 0xD7FF)) {
    *wc = s[0];
    return 1;
  }
  unsigned ax;
  asm("call\tgetutf16"
      : "=a"(ax), "=m"(*wc)
      : "D"(s), "S"(wc), "m"(*s), "m"(*(char(*)[4])s)
      : "cc");
  return ax;
}

#endif /* __GNUC__ && !__STRICT_ANSI__ */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_STR_H_ */
