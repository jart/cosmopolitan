#ifndef COSMOPOLITAN_LIBC_STR_STR_H_
#define COSMOPOLITAN_LIBC_STR_STR_H_

#define INVALID_CODEPOINT 0xfffd

#ifdef _COSMO_SOURCE
#define chomp         _chomp
#define chomp16       _chomp16
#define wchomp        _wchomp
#define tpenc         _tpenc
#define isutf8        _isutf8
#define istext        _istext
#define startswith    _startswith
#define startswithi   _startswithi
#define endswith      _endswith
#define wcsendswith   _wcsendswith
#define wcsstartswith _wcsstartswith
#endif /* _COSMO_SOURCE */

#ifndef WEOF
#define WEOF -1u
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void *memset(void *, int, size_t) memcpyesque;
void *memmove(void *, const void *, size_t) memcpyesque;
void *memcpy(void *, const void *, size_t) memcpyesque;
char *hexpcpy(char *, const void *, size_t) memcpyesque;

int memcmp(const void *, const void *, size_t) strlenesque;
int timingsafe_bcmp(const void *, const void *, size_t) libcesque;
int timingsafe_memcmp(const void *, const void *, size_t) libcesque;

size_t strlen(const char *) strlenesque;
size_t strnlen(const char *, size_t) strlenesque;
size_t strnlen_s(const char *, size_t) libcesque;
char *strchr(const char *, int) strlenesque;
void *memchr(const void *, int, size_t) strlenesque;
void *rawmemchr(const void *, int) strlenesque returnsnonnull;
size_t wcslen(const wchar_t *) strlenesque;
size_t wcsnlen(const wchar_t *, size_t) strlenesque;
size_t wcsnlen_s(const wchar_t *, size_t) libcesque;
wchar_t *wcschr(const wchar_t *, wchar_t) strlenesque;
wchar_t *wmemchr(const wchar_t *, wchar_t, size_t) strlenesque;
wchar_t *wcschrnul(const wchar_t *, wchar_t)
strlenesque returnsnonnull;
char *strstr(const char *, const char *) strlenesque;
wchar_t *wcsstr(const wchar_t *, const wchar_t *) strlenesque;
int strcmp(const char *, const char *) strlenesque;
int strncmp(const char *, const char *, size_t) strlenesque;
int wcscmp(const wchar_t *, const wchar_t *) strlenesque;
int wcsncmp(const wchar_t *, const wchar_t *, size_t) strlenesque;
int wmemcmp(const wchar_t *, const wchar_t *, size_t) strlenesque;
int strcasecmp(const char *, const char *) strlenesque;
int wcscasecmp(const wchar_t *, const wchar_t *) strlenesque;
int strncasecmp(const char *, const char *, size_t) strlenesque;
int wcsncasecmp(const wchar_t *, const wchar_t *, size_t) strlenesque;
char *strrchr(const char *, int) strlenesque;
wchar_t *wcsrchr(const wchar_t *, wchar_t) strlenesque;
char *strpbrk(const char *, const char *) strlenesque;
wchar_t *wcspbrk(const wchar_t *, const wchar_t *) strlenesque;
size_t strspn(const char *, const char *) strlenesque;
size_t wcsspn(const wchar_t *, const wchar_t *) strlenesque;
size_t strcspn(const char *, const char *) strlenesque;
size_t wcscspn(const wchar_t *, const wchar_t *) strlenesque;
void *memfrob(void *, size_t) memcpyesque;
int strcoll(const char *, const char *) strlenesque;
char *stpcpy(char *, const char *) memcpyesque;
char *stpncpy(char *, const char *, size_t) memcpyesque;
char *strcat(char *, const char *) memcpyesque;
wchar_t *wcscat(wchar_t *, const wchar_t *) memcpyesque;
size_t strxfrm(char *, const char *, size_t) libcesque;
char *strcpy(char *, const char *) memcpyesque;
wchar_t *wcscpy(wchar_t *, const wchar_t *) memcpyesque;
char *strncat(char *, const char *, size_t) memcpyesque;
wchar_t *wcsncat(wchar_t *, const wchar_t *, size_t) memcpyesque;
char *strncpy(char *, const char *, size_t) memcpyesque;
char *strtok(char *, const char *) paramsnonnull((2)) libcesque;
char *strtok_r(char *, const char *, char **) paramsnonnull((2, 3));
wchar_t *wcstok(wchar_t *, const wchar_t *, wchar_t **) paramsnonnull((2, 3));
wchar_t *wmemset(wchar_t *, wchar_t, size_t) memcpyesque;
wchar_t *wmemcpy(wchar_t *, const wchar_t *, size_t) memcpyesque;
wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t) memcpyesque;
ssize_t strfmon(char *, size_t, const char *, ...) libcesque;
long a64l(const char *) libcesque;
char *l64a(long) libcesque;

typedef unsigned mbstate_t;

wchar_t *wcsncpy(wchar_t *, const wchar_t *, size_t) libcesque;
int mbtowc(wchar_t *, const char *, size_t) libcesque;
size_t mbrtowc(wchar_t *, const char *, size_t, mbstate_t *) libcesque;
size_t mbsrtowcs(wchar_t *, const char **, size_t, mbstate_t *) libcesque;
size_t mbstowcs(wchar_t *, const char *, size_t) libcesque;
size_t wcrtomb(char *, wchar_t, mbstate_t *) libcesque;
size_t c32rtomb(char *, char32_t, mbstate_t *) libcesque;
size_t mbrtoc32(char32_t *, const char *, size_t, mbstate_t *) libcesque;
size_t c16rtomb(char *, char16_t, mbstate_t *) libcesque;
size_t mbrtoc16(char16_t *, const char *, size_t, mbstate_t *) libcesque;
size_t mbrlen(const char *, size_t, mbstate_t *) libcesque;
size_t mbsnrtowcs(wchar_t *, const char **, size_t, size_t, mbstate_t *);
size_t wcsnrtombs(char *, const wchar_t **, size_t, size_t, mbstate_t *);
size_t wcsrtombs(char *, const wchar_t **, size_t, mbstate_t *) libcesque;
size_t wcstombs(char *, const wchar_t *, size_t) libcesque;
int mbsinit(const mbstate_t *) libcesque;
int mblen(const char *, size_t) libcesque;
int wctomb(char *, wchar_t) libcesque;
int wctob(wint_t) libcesque;
wint_t btowc(int) libcesque;

int getsubopt(char **, char *const *, char **) libcesque paramsnonnull();
char *strsignal(int) returnsnonnull libcesque;
char *strerror(int) returnsnonnull dontthrow dontcallback;
errno_t strerror_r(int, char *, size_t) libcesque;
char *__xpg_strerror_r(int, char *, size_t) libcesque;

int bcmp(const void *, const void *, size_t) strlenesque;
void bcopy(const void *, void *, size_t) memcpyesque;
void bzero(void *, size_t) memcpyesque;
char *index(const char *, int) strlenesque;
char *rindex(const char *, int) strlenesque;

#if defined(_COSMO_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || \
    defined(_XOPEN_SOURCE)
void *memccpy(void *, const void *, int, size_t) memcpyesque;
#endif

#if defined(_COSMO_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
char *strsep(char **, const char *) libcesque paramsnonnull();
void explicit_bzero(void *, size_t);
size_t strlcpy(char *, const char *, size_t) libcesque;
size_t strlcat(char *, const char *, size_t) libcesque;
#endif

#if defined(_COSMO_SOURCE) || defined(_GNU_SOURCE)
int strverscmp(const char *, const char *) libcesque;
char *strchrnul(const char *, int) strlenesque returnsnonnull;
char *strcasestr(const char *, const char *) strlenesque;
void *memmem(const void *, size_t, const void *, size_t) libcesque;
void *memrchr(const void *, int, size_t) strlenesque;
void *mempcpy(void *, const void *, size_t) memcpyesque;
wchar_t *wmempcpy(wchar_t *, const wchar_t *, size_t) memcpyesque;
#endif

#ifdef _COSMO_SOURCE
pureconst uint64_t tpenc(uint32_t) libcesque;
char *chomp(char *) libcesque;
wchar_t *wchomp(wchar_t *) libcesque;
uint64_t __fnv(const void *, size_t) strlenesque;
bool32 startswith(const char *, const char *) strlenesque;
bool32 startswithi(const char *, const char *) strlenesque;
bool32 endswith(const char *, const char *) strlenesque;
bool32 istext(const void *, size_t) libcesque;
bool32 isutf8(const void *, size_t) libcesque;
void *wmemrchr(const wchar_t *, wchar_t, size_t) strlenesque;
const char *strsignal_r(int, char[21]) returnsnonnull libcesque __wur;
char16_t *chomp16(char16_t *) libcesque;
size_t strlen16(const char16_t *) strlenesque;
size_t strnlen16(const char16_t *, size_t) strlenesque;
char16_t *strchr16(const char16_t *, int) strlenesque;
void *memchr16(const void *, int, size_t) strlenesque;
char16_t *strchrnul16(const char16_t *, int) strlenesque returnsnonnull;
void *rawmemchr16(const void *, int) strlenesque returnsnonnull;
char16_t *strstr16(const char16_t *, const char16_t *) strlenesque;
int memcasecmp(const void *, const void *, size_t) strlenesque;
int strcmp16(const char16_t *, const char16_t *) strlenesque;
int strncmp16(const char16_t *, const char16_t *, size_t) strlenesque;
int strcasecmp16(const char16_t *, const char16_t *) strlenesque;
int strncasecmp16(const char16_t *, const char16_t *, size_t) strlenesque;
char16_t *strrchr16(const char16_t *, int) strlenesque;
void *memrchr16(const void *, int, size_t) strlenesque;
char16_t *strpbrk16(const char16_t *, const char16_t *) strlenesque;
size_t strspn16(const char16_t *, const char16_t *) strlenesque;
size_t strcspn16(const char16_t *, const char16_t *) strlenesque;
char16_t *strcat16(char16_t *, const char16_t *) memcpyesque;
char16_t *strcpy16(char16_t *, const char16_t *) memcpyesque;
char16_t *strncat16(char16_t *, const char16_t *, size_t) memcpyesque;
char16_t *memset16(char16_t *, char16_t, size_t) memcpyesque;
bool32 startswith16(const char16_t *, const char16_t *) strlenesque;
bool32 endswith16(const char16_t *, const char16_t *) strlenesque;
axdx_t tprecode8to16(char16_t *, size_t, const char *) libcesque;
axdx_t tprecode16to8(char *, size_t, const char16_t *) libcesque;
bool32 wcsstartswith(const wchar_t *, const wchar_t *) strlenesque;
bool32 wcsendswith(const wchar_t *, const wchar_t *) strlenesque;
char *__join_paths(char *, size_t, const char *, const char *) libcesque __wur;
int __mkntpathat(int, const char *, int, char16_t[hasatleast 1024]);
#endif /* _COSMO_SOURCE */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_STR_H_ */
