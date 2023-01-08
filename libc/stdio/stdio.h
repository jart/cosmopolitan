#ifndef _STDIO_H
#define _STDIO_H
#include "libc/fmt/pflink.h"

#define L_ctermid    20
#define FILENAME_MAX PATH_MAX
#define FOPEN_MAX    1000
#define TMP_MAX      10000

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o                                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef struct FILE {
  uint8_t bufmode;             /* 0x00 _IOFBF, etc. (ignored if fd=-1) */
  bool noclose;                /* 0x01 for fake dup() todo delete! */
  uint32_t iomode;             /* 0x04 O_RDONLY, etc. (ignored if fd=-1) */
  int32_t state;               /* 0x08 0=OK, -1=EOF, >0=errno */
  int fd;                      /* 0x0c ≥0=fd, -1=closed|buffer */
  uint32_t beg;                /* 0x10 */
  uint32_t end;                /* 0x14 */
  char *buf;                   /* 0x18 */
  uint32_t size;               /* 0x20 */
  uint32_t nofree;             /* 0x24 */
  int pid;                     /* 0x28 */
  char *getln;                 /* 0x30 */
  char lock[16];               /* 0x38 */
  _Atomic(struct FILE *) next; /* 0x48 */
  char mem[BUFSIZ];            /* 0x50 */
} FILE;

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
FILE *fopen(const char *, const char *) paramsnonnull((2)) dontdiscard;
FILE *fdopen(int, const char *) paramsnonnull() dontdiscard;
FILE *fmemopen(void *, size_t, const char *) paramsnonnull((3)) dontdiscard;
FILE *freopen(const char *, const char *, FILE *) paramsnonnull((2, 3));
size_t fread(void *, size_t, size_t, FILE *) paramsnonnull((4));
size_t fwrite(const void *, size_t, size_t, FILE *) paramsnonnull((4));
int fclose(FILE *);
int fclose_s(FILE **) paramsnonnull();
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

typedef uint64_t fpos_t;
char *gets(char *) paramsnonnull();
int fgetpos(FILE *, fpos_t *) paramsnonnull();
int fsetpos(FILE *, const fpos_t *) paramsnonnull();

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

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » without mutexes                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int getc_unlocked(FILE *) paramsnonnull();
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
int fseeko_unlocked(FILE *, int64_t, int) paramsnonnull();
ssize_t getdelim_unlocked(char **, size_t *, int, FILE *) paramsnonnull();
int fprintf_unlocked(FILE *, const char *, ...) printfesque(2)
    paramsnonnull((1, 2)) dontthrow nocallback;
int vfprintf_unlocked(FILE *, const char *, va_list)
    paramsnonnull() dontthrow nocallback;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » optimizations                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define getc(f)     fgetc(f)
#define getwc(f)    fgetwc(f)
#define putc(c, f)  fputc(c, f)
#define putwc(c, f) fputwc(c, f)

#define getc_unlocked(f)     fgetc_unlocked(f)
#define getwc_unlocked(f)    fgetwc_unlocked(f)
#define putc_unlocked(c, f)  fputc_unlocked(c, f)
#define putwc_unlocked(c, f) fputwc_unlocked(c, f)

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
/* clang-format off */
#define printf(FMT, ...)     (printf)(PFLINK(FMT), ##__VA_ARGS__)
#define vprintf(FMT, VA)     (vprintf)(PFLINK(FMT), VA)
#define fprintf(F, FMT, ...) (fprintf)(F, PFLINK(FMT), ##__VA_ARGS__)
#define vfprintf(F, FMT, VA) (vfprintf)(F, PFLINK(FMT),  VA)
#define fprintf_unlocked(F, FMT, ...) (fprintf_unlocked)(F, PFLINK(FMT), ##__VA_ARGS__)
#define vfprintf_unlocked(F, FMT, VA) (vfprintf_unlocked)(F, PFLINK(FMT), VA)
#define vscanf(FMT, VA)      (vscanf)(SFLINK(FMT), VA)
#define scanf(FMT, ...)      (scanf)(SFLINK(FMT), ##__VA_ARGS__)
#define fscanf(F, FMT, ...)  (fscanf)(F, SFLINK(FMT), ##__VA_ARGS__)
#define vfscanf(F, FMT, VA)  (vfscanf)(F, SFLINK(FMT), VA)
/* clang-format on */
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* _STDIO_H */
