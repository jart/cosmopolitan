#ifndef COSMOPOLITAN_LIBC_STDIO_H_
#define COSMOPOLITAN_LIBC_STDIO_H_

#define EOF      -1  /* end of file */
#define WEOF     -1u /* end of file (multibyte) */
#define _IOFBF   0   /* fully buffered */
#define _IOLBF   1   /* line buffered */
#define _IONBF   2   /* no buffering */
#define _CS_PATH 0

#define L_tmpnam     20
#define L_ctermid    20
#define P_tmpdir     "/tmp"
#define FILENAME_MAX 1024
#define FOPEN_MAX    1000
#define TMP_MAX      10000
#define BUFSIZ       4096

COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o                                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

struct FILE;
typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

errno_t ferror(FILE *) libcesque paramsnonnull();
void clearerr(FILE *) libcesque paramsnonnull();
int feof(FILE *) libcesque paramsnonnull();
int getc(FILE *) libcesque paramsnonnull();
int putc(int, FILE *) libcesque paramsnonnull();
int fflush(FILE *) libcesque;
int fpurge(FILE *) libcesque;
int fgetc(FILE *) libcesque paramsnonnull();
char *fgetln(FILE *, size_t *) libcesque paramsnonnull((1));
int ungetc(int, FILE *) libcesque paramsnonnull();
int fileno(FILE *) libcesque paramsnonnull() nosideeffect;
int fputc(int, FILE *) libcesque paramsnonnull();
int fputs(const char *, FILE *) libcesque paramsnonnull();
int fputws(const wchar_t *, FILE *) libcesque paramsnonnull();
void flockfile(FILE *) libcesque paramsnonnull();
void funlockfile(FILE *) libcesque paramsnonnull();
int ftrylockfile(FILE *) libcesque paramsnonnull();
char *fgets(char *, int, FILE *) libcesque paramsnonnull();
wchar_t *fgetws(wchar_t *, int, FILE *) libcesque paramsnonnull();
wint_t putwc(wchar_t, FILE *) libcesque paramsnonnull();
wint_t fputwc(wchar_t, FILE *) libcesque paramsnonnull();
wint_t putwchar(wchar_t) libcesque;
wint_t getwchar(void) libcesque;
wint_t getwc(FILE *) libcesque paramsnonnull();
wint_t fgetwc(FILE *) libcesque paramsnonnull();
wint_t ungetwc(wint_t, FILE *) libcesque paramsnonnull();
int getchar(void) libcesque;
int putchar(int) libcesque;
int puts(const char *) libcesque;
ssize_t getline(char **, size_t *, FILE *) libcesque paramsnonnull();
ssize_t getdelim(char **, size_t *, int, FILE *) libcesque paramsnonnull();
FILE *fopen(const char *, const char *) libcesque paramsnonnull((2)) __wur;
FILE *fdopen(int, const char *) libcesque paramsnonnull() __wur;
FILE *fmemopen(void *, size_t, const char *) libcesque paramsnonnull((3)) __wur;
FILE *freopen(const char *, const char *, FILE *) paramsnonnull((2, 3));
size_t fread(void *, size_t, size_t, FILE *) libcesque paramsnonnull((4));
size_t fwrite(const void *, size_t, size_t, FILE *) paramsnonnull((4));
int fclose(FILE *) libcesque;
int fseek(FILE *, long, int) libcesque paramsnonnull();
long ftell(FILE *) libcesque paramsnonnull();
int fseeko(FILE *, int64_t, int) libcesque paramsnonnull();
int64_t ftello(FILE *) libcesque paramsnonnull();
void rewind(FILE *) libcesque paramsnonnull();
int fopenflags(const char *) libcesque paramsnonnull();
void setlinebuf(FILE *) libcesque;
void setbuf(FILE *, char *) libcesque;
void setbuffer(FILE *, char *, size_t) libcesque;
int setvbuf(FILE *, char *, int, size_t) libcesque;
int pclose(FILE *) libcesque;
char *ctermid(char *) libcesque;
void perror(const char *) libcesque relegated;
size_t confstr(int, char *, size_t) libcesque;

typedef uint64_t fpos_t;
char *gets(char *) libcesque paramsnonnull();
int fgetpos(FILE *, fpos_t *) libcesque paramsnonnull();
int fsetpos(FILE *, const fpos_t *) libcesque paramsnonnull();

FILE *tmpfile(void) libcesque __wur;
char *tmpnam(char *) libcesque __wur;
char *tmpnam_r(char *) libcesque __wur;

FILE *popen(const char *, const char *) libcesque;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » formatting                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int printf(const char *, ...) printfesque(1) paramsnonnull((1)) libcesque;
int vprintf(const char *, va_list) paramsnonnull() libcesque;
int fprintf(FILE *, const char *, ...) printfesque(2)
    paramsnonnull((1, 2)) libcesque;
int vfprintf(FILE *, const char *, va_list) paramsnonnull() libcesque;
int scanf(const char *, ...) libcesque scanfesque(1);
int vscanf(const char *, va_list) libcesque;
int fscanf(FILE *, const char *, ...) libcesque scanfesque(2);
int vfscanf(FILE *, const char *, va_list) libcesque;

int snprintf(char *, size_t, const char *, ...) printfesque(3) libcesque;
int vsnprintf(char *, size_t, const char *, va_list) libcesque;
int sprintf(char *, const char *, ...) libcesque;
int vsprintf(char *, const char *, va_list) libcesque;

int fwprintf(FILE *, const wchar_t *, ...) libcesque;
int fwscanf(FILE *, const wchar_t *, ...) libcesque;
int swprintf(wchar_t *, size_t, const wchar_t *, ...) libcesque;
int swscanf(const wchar_t *, const wchar_t *, ...) libcesque;
int vfwprintf(FILE *, const wchar_t *, va_list) libcesque;
int vfwscanf(FILE *, const wchar_t *, va_list) libcesque;
int vswprintf(wchar_t *, size_t, const wchar_t *, va_list) libcesque;
int vswscanf(const wchar_t *, const wchar_t *, va_list) libcesque;
int vwprintf(const wchar_t *, va_list) libcesque;
int vwscanf(const wchar_t *, va_list) libcesque;
int wprintf(const wchar_t *, ...) libcesque;
int wscanf(const wchar_t *, ...) libcesque;
int fwide(FILE *, int) libcesque;

int sscanf(const char *, const char *, ...) libcesque scanfesque(2);
int vsscanf(const char *, const char *, va_list) libcesque;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » allocating                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int asprintf(char **, const char *, ...) printfesque(2)
    paramsnonnull((1, 2)) libcesque;
int vasprintf(char **, const char *, va_list) paramsnonnull() libcesque;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » without mutexes                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int getc_unlocked(FILE *) libcesque paramsnonnull();
int puts_unlocked(const char *) libcesque;
int getchar_unlocked(void) libcesque;
int putc_unlocked(int, FILE *) libcesque paramsnonnull();
int putchar_unlocked(int) libcesque;
void clearerr_unlocked(FILE *) libcesque;
int feof_unlocked(FILE *) libcesque;
int ferror_unlocked(FILE *) libcesque;
int fileno_unlocked(FILE *) libcesque;
int fflush_unlocked(FILE *) libcesque;
int fgetc_unlocked(FILE *) libcesque;
int fputc_unlocked(int, FILE *) libcesque;
size_t fread_unlocked(void *, size_t, size_t, FILE *) libcesque;
size_t fwrite_unlocked(const void *, size_t, size_t, FILE *) libcesque;
char *fgets_unlocked(char *, int, FILE *) libcesque;
int fputs_unlocked(const char *, FILE *) libcesque;
wint_t getwc_unlocked(FILE *) libcesque;
wint_t getwchar_unlocked(void) libcesque;
wint_t fgetwc_unlocked(FILE *) libcesque;
wint_t fputwc_unlocked(wchar_t, FILE *) libcesque;
wint_t putwc_unlocked(wchar_t, FILE *) libcesque;
wint_t putwchar_unlocked(wchar_t) libcesque;
wchar_t *fgetws_unlocked(wchar_t *, int, FILE *) libcesque;
int fputws_unlocked(const wchar_t *, FILE *) libcesque;
wint_t ungetwc_unlocked(wint_t, FILE *) libcesque paramsnonnull();
int ungetc_unlocked(int, FILE *) libcesque paramsnonnull();
int fseek_unlocked(FILE *, int64_t, int) libcesque paramsnonnull();
ssize_t getdelim_unlocked(char **, size_t *, int, FILE *) paramsnonnull();
int fprintf_unlocked(FILE *, const char *, ...) printfesque(2) libcesque;
int vfprintf_unlocked(FILE *, const char *, va_list) paramsnonnull() libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_H_ */
