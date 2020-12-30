#ifndef COSMOPOLITAN_LIBC_STDIO_STDIO_H_
#define COSMOPOLITAN_LIBC_STDIO_STDIO_H_
#include "libc/fmt/pflink.h"
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o                                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef struct FILE {
  uint8_t bufmode;               // 0x00 _IOFBF, etc. (ignored if fd=-1)
  bool noclose;                  // 0x01 for fake dup()
  uint32_t iomode;               // 0x04 O_RDONLY, etc. (ignored if fd=-1)
  int32_t state;                 // 0x08 0=OK, -1=EOF, >0=errno
  int fd;                        // 0x0c ≥0=fd, -1=closed|buffer
  uint32_t beg;                  // 0x10
  uint32_t end;                  // 0x14
  uint8_t *buf;                  // 0x18
  uint32_t size;                 // 0x20
  uint32_t nofree;               // 0x24
  int (*reader)(struct FILE *);  // 0x28
  int (*writer)(struct FILE *);  // 0x30
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
int fgetc(FILE *) paramsnonnull();
int ungetc(int, FILE *) paramsnonnull();
int fileno(FILE *) paramsnonnull() nosideeffect;
int fputc(int, FILE *) paramsnonnull();
int fputs(const char *, FILE *) paramsnonnull();
int fputws(const wchar_t *, FILE *) paramsnonnull();
char *fgets(char *, int, FILE *) paramsnonnull();
wchar_t *fgetws(wchar_t *, int, FILE *) paramsnonnull();
wint_t fputwc(wchar_t, FILE *) paramsnonnull();
wint_t putwchar(wchar_t);
wint_t getwchar(void);
wint_t fgetwc(FILE *) paramsnonnull();
int getchar(void);
int putchar(int);
int puts(const char *) paramsnonnull();
ssize_t getline(char **, size_t *, FILE *) paramsnonnull();
ssize_t getdelim(char **, size_t *, int, FILE *) paramsnonnull();
int fputhex(int, FILE *) paramsnonnull();
int fgethex(FILE *) paramsnonnull();
FILE *fopen(const char *, const char *) paramsnonnull() nodiscard;
FILE *fdopen(int, const char *) paramsnonnull() nodiscard;
FILE *fmemopen(void *, size_t, const char *) paramsnonnull((3)) nodiscard;
FILE *freopen(const char *, const char *, FILE *) paramsnonnull((2, 3));
size_t fread(void *, size_t, size_t, FILE *) paramsnonnull();
size_t fwrite(const void *, size_t, size_t, FILE *) paramsnonnull();
int fclose(FILE *);
int fclose_s(FILE **) paramsnonnull();
long fseek(FILE *, long, int) paramsnonnull();
long ftell(FILE *) paramsnonnull();
void rewind(FILE *) paramsnonnull();
int fopenflags(const char *) paramsnonnull();
unsigned favail(FILE *);
void setbuf(FILE *, char *);
void setbuffer(FILE *, char *, size_t);
int setvbuf(FILE *, char *, int, size_t);

typedef uint64_t fpos_t;
compatfn char *gets(char *) paramsnonnull();
compatfn int fgetpos(FILE *, fpos_t *) paramsnonnull();
compatfn int fsetpos(FILE *, const fpos_t *) paramsnonnull();
compatfn int64_t fseeko(FILE *, long, int) paramsnonnull();
compatfn int64_t ftello(FILE *) paramsnonnull();

int system(const char *);
int systemecho(const char *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » formatting                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int printf(const char *, ...) printfesque(1)
    paramsnonnull((1)) nothrow nocallback;
int vprintf(const char *, va_list) paramsnonnull() nothrow nocallback;
int fprintf(FILE *, const char *, ...) printfesque(2)
    paramsnonnull((1, 2)) nothrow nocallback;
int vfprintf(FILE *, const char *, va_list) paramsnonnull() nothrow nocallback;
int scanf(const char *, ...) scanfesque(1);
int vscanf(const char *, va_list);
int fscanf(FILE *, const char *, ...) scanfesque(2);
int vfscanf(FILE *, const char *, va_list);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » optimizations                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define getc(f)    (f->beg < f->end ? f->buf[f->beg++] : fgetc(f))
#define putc(c, f) fputc(c, f)

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define printf(FMT, ...)     (printf)(PFLINK(FMT), ##__VA_ARGS__)
#define vprintf(FMT, VA)     (vprintf)(PFLINK(FMT), VA)
#define fprintf(F, FMT, ...) (fprintf)(F, PFLINK(FMT), ##__VA_ARGS__)
#define vfprintf(F, FMT, VA) (vfprintf)(F, PFLINK(FMT), VA)
#define vscanf(FMT, VA)      (vscanf)(SFLINK(FMT), VA)
#define scanf(FMT, ...)      (scanf)(SFLINK(FMT), ##__VA_ARGS__)
#define fscanf(F, FMT, ...)  (fscanf)(F, SFLINK(FMT), ##__VA_ARGS__)
#define vfscanf(F, FMT, VA)  (vfscanf)(F, SFLINK(FMT), VA)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define stdin  SYMBOLIC(stdin)
#define stdout SYMBOLIC(stdout)
#define stderr SYMBOLIC(stderr)

#endif /* COSMOPOLITAN_LIBC_STDIO_STDIO_H_ */
