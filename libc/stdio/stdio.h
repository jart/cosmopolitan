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

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o                                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

struct FILE;
typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

errno_t ferror(FILE *) paramsnonnull();
void clearerr(FILE *) paramsnonnull();
int feof(FILE *) paramsnonnull();
int getc(FILE *) paramsnonnull();
int putc(int, FILE *) paramsnonnull();
int fflush(FILE *);
int fpurge(FILE *);
int fgetc(FILE *) paramsnonnull();
char *fgetln(FILE *, size_t *) paramsnonnull((1));
int ungetc(int, FILE *) paramsnonnull();
int fileno(FILE *) paramsnonnull() nosideeffect;
int fputc(int, FILE *) paramsnonnull();
int fputs(const char *, FILE *) paramsnonnull();
int fputws(const wchar_t *, FILE *) paramsnonnull();
void flockfile(FILE *) paramsnonnull();
void funlockfile(FILE *) paramsnonnull();
int ftrylockfile(FILE *) paramsnonnull();
char *fgets(char *, int, FILE *) paramsnonnull();
wchar_t *fgetws(wchar_t *, int, FILE *) paramsnonnull();
wint_t putwc(wchar_t, FILE *) paramsnonnull();
wint_t fputwc(wchar_t, FILE *) paramsnonnull();
wint_t putwchar(wchar_t);
wint_t getwchar(void);
wint_t getwc(FILE *) paramsnonnull();
wint_t fgetwc(FILE *) paramsnonnull();
wint_t ungetwc(wint_t, FILE *) paramsnonnull();
int getchar(void);
int putchar(int);
int puts(const char *);
ssize_t getline(char **, size_t *, FILE *) paramsnonnull();
ssize_t getdelim(char **, size_t *, int, FILE *) paramsnonnull();
FILE *fopen(const char *, const char *) paramsnonnull((2)) __wur;
FILE *fdopen(int, const char *) paramsnonnull() __wur;
FILE *fmemopen(void *, size_t, const char *) paramsnonnull((3)) __wur;
FILE *freopen(const char *, const char *, FILE *) paramsnonnull((2, 3));
size_t fread(void *, size_t, size_t, FILE *) paramsnonnull((4));
size_t fwrite(const void *, size_t, size_t, FILE *) paramsnonnull((4));
int fclose(FILE *);
int fseek(FILE *, long, int) paramsnonnull();
long ftell(FILE *) paramsnonnull();
int fseeko(FILE *, int64_t, int) paramsnonnull();
int64_t ftello(FILE *) paramsnonnull();
void rewind(FILE *) paramsnonnull();
int fopenflags(const char *) paramsnonnull();
void setlinebuf(FILE *);
void setbuf(FILE *, char *);
void setbuffer(FILE *, char *, size_t);
int setvbuf(FILE *, char *, int, size_t);
int pclose(FILE *);
char *ctermid(char *);
void perror(const char *) relegated;
size_t confstr(int, char *, size_t);

typedef uint64_t fpos_t;
char *gets(char *) paramsnonnull();
int fgetpos(FILE *, fpos_t *) paramsnonnull();
int fsetpos(FILE *, const fpos_t *) paramsnonnull();

FILE *tmpfile(void) __wur;
char *tmpnam(char *) __wur;
char *tmpnam_r(char *) __wur;
int system(const char *);
FILE *popen(const char *, const char *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » formatting                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int printf(const char *, ...) printfesque(1)
    paramsnonnull((1)) dontthrow nocallback;
int vprintf(const char *, va_list) paramsnonnull() dontthrow nocallback;
int fprintf(FILE *, const char *, ...) printfesque(2)
    paramsnonnull((1, 2)) dontthrow nocallback;
int vfprintf(FILE *, const char *, va_list)
    paramsnonnull() dontthrow nocallback;
int scanf(const char *, ...) scanfesque(1);
int vscanf(const char *, va_list);
int fscanf(FILE *, const char *, ...) scanfesque(2);
int vfscanf(FILE *, const char *, va_list);

int snprintf(char *, size_t, const char *, ...)
    printfesque(3) dontthrow nocallback;
int vsnprintf(char *, size_t, const char *, va_list)
dontthrow nocallback;
int sprintf(char *, const char *, ...) dontthrow nocallback;
int vsprintf(char *, const char *, va_list)
dontthrow nocallback;

int fwprintf(FILE *, const wchar_t *, ...);
int fwscanf(FILE *, const wchar_t *, ...);
int swprintf(wchar_t *, size_t, const wchar_t *, ...);
int swscanf(const wchar_t *, const wchar_t *, ...);
int vfwprintf(FILE *, const wchar_t *, va_list);
int vfwscanf(FILE *, const wchar_t *, va_list);
int vswprintf(wchar_t *, size_t, const wchar_t *, va_list);
int vswscanf(const wchar_t *, const wchar_t *, va_list);
int vwprintf(const wchar_t *, va_list);
int vwscanf(const wchar_t *, va_list);
int wprintf(const wchar_t *, ...);
int wscanf(const wchar_t *, ...);
int fwide(FILE *, int);

int sscanf(const char *, const char *, ...) scanfesque(2);
int vsscanf(const char *, const char *, va_list);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » allocating                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int asprintf(char **, const char *, ...) printfesque(2)
    paramsnonnull((1, 2)) libcesque;
int vasprintf(char **, const char *, va_list) paramsnonnull() libcesque;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » without mutexes                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int getc_unlocked(FILE *) paramsnonnull();
int puts_unlocked(const char *);
int getchar_unlocked(void);
int putc_unlocked(int, FILE *) paramsnonnull();
int putchar_unlocked(int);
void clearerr_unlocked(FILE *);
int feof_unlocked(FILE *);
int ferror_unlocked(FILE *);
int fileno_unlocked(FILE *);
int fflush_unlocked(FILE *);
int fgetc_unlocked(FILE *);
int fputc_unlocked(int, FILE *);
size_t fread_unlocked(void *, size_t, size_t, FILE *);
size_t fwrite_unlocked(const void *, size_t, size_t, FILE *);
char *fgets_unlocked(char *, int, FILE *);
int fputs_unlocked(const char *, FILE *);
wint_t getwc_unlocked(FILE *);
wint_t getwchar_unlocked(void);
wint_t fgetwc_unlocked(FILE *);
wint_t fputwc_unlocked(wchar_t, FILE *);
wint_t putwc_unlocked(wchar_t, FILE *);
wint_t putwchar_unlocked(wchar_t);
wchar_t *fgetws_unlocked(wchar_t *, int, FILE *);
int fputws_unlocked(const wchar_t *, FILE *);
wint_t ungetwc_unlocked(wint_t, FILE *) paramsnonnull();
int ungetc_unlocked(int, FILE *) paramsnonnull();
int fseek_unlocked(FILE *, int64_t, int) paramsnonnull();
ssize_t getdelim_unlocked(char **, size_t *, int, FILE *) paramsnonnull();
int fprintf_unlocked(FILE *, const char *, ...) printfesque(2)
    paramsnonnull((1, 2)) dontthrow nocallback;
int vfprintf_unlocked(FILE *, const char *, va_list)
    paramsnonnull() dontthrow nocallback;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § cxxabi                                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *__cxa_demangle(const char *, char *, size_t *, int *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_H_ */
