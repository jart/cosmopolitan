#ifndef COSMOPOLITAN_LIBC_STDIO_H_
#define COSMOPOLITAN_LIBC_STDIO_H_

#define EOF    -1 /* end of file */
#define _IOFBF 0  /* fully buffered */
#define _IOLBF 1  /* line buffered */
#define _IONBF 2  /* no buffering */

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
/* clang-format off */

struct FILE;
typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

errno_t ferror(FILE *) libcesque paramsnonnull() __read_write(1);
void clearerr(FILE *) libcesque paramsnonnull() __read_write(1);
int feof(FILE *) libcesque paramsnonnull() __read_write(1);
int getc(FILE *) libcesque paramsnonnull() __read_write(1);
int putc(int, FILE *) libcesque paramsnonnull() __read_write(2);
int fflush(FILE *) libcesque __read_write(1);
int fpurge(FILE *) libcesque __read_write(1);
int fgetc(FILE *) libcesque paramsnonnull() __read_write(1);
char *fgetln(FILE *, size_t *) libcesque paramsnonnull((1)) __read_write(1) __write_only(2);
int ungetc(int, FILE *) libcesque paramsnonnull() __write_only(2);
int fileno(FILE *) libcesque paramsnonnull() nosideeffect __write_only(1);
int fputc(int, FILE *) libcesque paramsnonnull() __write_only(2);
int fputs(const char *, FILE *) libcesque paramsnonnull() __write_only(2);
int fputws(const wchar_t *, FILE *) libcesque paramsnonnull() __write_only(2);
void flockfile(FILE *) libcesque __write_only(1);
void funlockfile(FILE *) libcesque paramsnonnull() __write_only(1);
int ftrylockfile(FILE *) libcesque paramsnonnull() __write_only(1);
char *fgets(char *, int, FILE *) libcesque paramsnonnull() __write_only(1, 2) __read_write(3);
wchar_t *fgetws(wchar_t *, int, FILE *) libcesque paramsnonnull() __write_only(1, 2) __read_write(3);
wint_t putwc(wchar_t, FILE *) libcesque paramsnonnull() __write_only(2);
wint_t fputwc(wchar_t, FILE *) libcesque paramsnonnull() __write_only(2);
wint_t putwchar(wchar_t) libcesque;
wint_t getwchar(void) libcesque;
wint_t getwc(FILE *) libcesque paramsnonnull() __write_only(1);
wint_t fgetwc(FILE *) libcesque paramsnonnull() __write_only(1);
wint_t ungetwc(wint_t, FILE *) libcesque paramsnonnull() __write_only(2);
int getchar(void) libcesque;
int putchar(int) libcesque;
int puts(const char *) libcesque __read_only(1);

ssize_t getline(char **, size_t *, FILE *) libcesque paramsnonnull() __read_write(1) __read_write(2) __read_write(3);
ssize_t getdelim(char **, size_t *, int, FILE *) libcesque paramsnonnull() __read_write(1) __read_write(2) __read_write(4);
FILE *fopen(const char *, const char *) libcesque paramsnonnull((2)) __read_only(1) __read_only(2) __wur;
FILE *fdopen(int, const char *) libcesque paramsnonnull() __read_only(2) __wur;
FILE *fmemopen(void *, size_t, const char *) libcesque paramsnonnull((3)) __read_write(1) __read_only(3) __wur;
FILE *freopen(const char *, const char *, FILE *) paramsnonnull((2, 3)) __read_only(1) __read_only(2) __read_write(3);
size_t fread(void *, size_t, size_t, FILE *) libcesque paramsnonnull((4)) __write_only(1) __read_write(4);
size_t fwrite(const void *, size_t, size_t, FILE *) paramsnonnull((4)) __read_only(1) __read_write(4);
int fclose(FILE *) libcesque __read_write(1);
int fseek(FILE *, long, int) libcesque paramsnonnull() __read_write(1);
long ftell(FILE *) libcesque paramsnonnull() __read_write(1);
int fseeko(FILE *, int64_t, int) libcesque paramsnonnull() __read_write(1);
int64_t ftello(FILE *) libcesque paramsnonnull() __read_write(1);
void rewind(FILE *) libcesque paramsnonnull() __read_write(1);
int fopenflags(const char *) libcesque paramsnonnull() __read_only(1);
void setlinebuf(FILE *) libcesque __read_write(1);
void setbuf(FILE *, char *) libcesque __read_write(1) __write_only(2);
void setbuffer(FILE *, char *, size_t) libcesque __read_write(1) __write_only(2);
int setvbuf(FILE *, char *, int, size_t) libcesque __read_write(1);
int pclose(FILE *) libcesque __read_write(1);
char *ctermid(char *) libcesque __write_only(1);
void perror(const char *) libcesque relegated __read_only(1);

typedef uint64_t fpos_t;
char *gets(char *) libcesque paramsnonnull() __write_only(1);
int fgetpos(FILE *, fpos_t *) libcesque paramsnonnull() __read_write(1) __write_only(2);
int fsetpos(FILE *, const fpos_t *) libcesque paramsnonnull() __read_write(1) __read_only(2);

FILE *tmpfile(void) libcesque __wur;
char *tmpnam(char *) libcesque __write_only(1) __wur;
char *tmpnam_r(char *) libcesque __write_only(1) __wur;

FILE *popen(const char *, const char *) libcesque __read_only(1) __read_only(2);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » formatting                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int printf(const char *, ...) printfesque(1) paramsnonnull((1)) libcesque __read_only(1);
int vprintf(const char *, va_list) paramsnonnull() libcesque __read_only(1);
int fprintf(FILE *, const char *, ...) printfesque(2) paramsnonnull((1, 2)) libcesque __read_write(1) __read_only(2);
int vfprintf(FILE *, const char *, va_list) paramsnonnull() libcesque __read_write(1) __read_only(2);
int scanf(const char *, ...) libcesque scanfesque(1) __read_only(1);
int vscanf(const char *, va_list) libcesque __read_only(1);
int fscanf(FILE *, const char *, ...) libcesque scanfesque(2) __read_write(1) __read_only(2);
int vfscanf(FILE *, const char *, va_list) libcesque __read_write(1) __read_only(2);

int snprintf(char *, size_t, const char *, ...) printfesque(3) libcesque __write_only(1) __read_only(3);
int vsnprintf(char *, size_t, const char *, va_list) libcesque __write_only(1) __read_only(3);
int sprintf(char *, const char *, ...) libcesque __write_only(1) __read_only(2);
int vsprintf(char *, const char *, va_list) libcesque __write_only(1) __read_only(2);

int fwprintf(FILE *, const wchar_t *, ...) libcesque __read_write(1) __read_only(2);
int fwscanf(FILE *, const wchar_t *, ...) libcesque __read_write(1) __read_only(2);
int swprintf(wchar_t *, size_t, const wchar_t *, ...) libcesque __write_only(1) __read_only(3);
int swscanf(const wchar_t *, const wchar_t *, ...) libcesque __read_only(1) __read_only(2);
int vfwprintf(FILE *, const wchar_t *, va_list) libcesque __read_write(1) __read_only(2);
int vfwscanf(FILE *, const wchar_t *, va_list) libcesque __read_write(1) __read_only(2);
int vswprintf(wchar_t *, size_t, const wchar_t *, va_list) libcesque __write_only(1) __read_only(3);
int vswscanf(const wchar_t *, const wchar_t *, va_list) libcesque __read_only(1) __read_only(2);
int vwprintf(const wchar_t *, va_list) libcesque __read_only(1);
int vwscanf(const wchar_t *, va_list) libcesque __read_only(1);
int wprintf(const wchar_t *, ...) libcesque __read_only(1);
int wscanf(const wchar_t *, ...) libcesque __read_only(1);
int fwide(FILE *, int) libcesque __read_write(1);

int sscanf(const char *, const char *, ...) libcesque scanfesque(2) __read_only(1) __read_only(2);
int vsscanf(const char *, const char *, va_list) libcesque __read_only(1) __read_only(2);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » allocating                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int asprintf(char **, const char *, ...) printfesque(2) paramsnonnull((1, 2)) libcesque __write_only(1);
int vasprintf(char **, const char *, va_list) paramsnonnull() libcesque __write_only(1);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard i/o » without mutexes                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int getc_unlocked(FILE *) libcesque paramsnonnull() __read_write(1);
int puts_unlocked(const char *) libcesque __read_only(1);
int getchar_unlocked(void) libcesque;
int putc_unlocked(int, FILE *) libcesque paramsnonnull() __read_write(2);
int putchar_unlocked(int) libcesque;
void clearerr_unlocked(FILE *) libcesque __write_only(1);
int feof_unlocked(FILE *) libcesque __read_only(1);
int ferror_unlocked(FILE *) libcesque __read_only(1);
int fileno_unlocked(FILE *) libcesque __read_only(1);
int fflush_unlocked(FILE *) libcesque __read_write(1);
int fgetc_unlocked(FILE *) libcesque __read_write(1);
int fputc_unlocked(int, FILE *) libcesque __read_write(2);
size_t fread_unlocked(void *, size_t, size_t, FILE *) libcesque __write_only(1) __read_write(4);
size_t fwrite_unlocked(const void *, size_t, size_t, FILE *) libcesque __read_only(1) __read_write(4);
char *fgets_unlocked(char *, int, FILE *) libcesque __write_only(1) __read_write(3);
int fputs_unlocked(const char *, FILE *) libcesque __read_only(1) __read_write(2);
wint_t getwc_unlocked(FILE *) libcesque __read_write(1);
wint_t getwchar_unlocked(void) libcesque;
wint_t fgetwc_unlocked(FILE *) libcesque __read_write(1);
wint_t fputwc_unlocked(wchar_t, FILE *) libcesque __read_write(2);
wint_t putwc_unlocked(wchar_t, FILE *) libcesque __read_write(2);
wint_t putwchar_unlocked(wchar_t) libcesque;
wchar_t *fgetws_unlocked(wchar_t *, int, FILE *) libcesque __write_only(1, 2) __read_write(3);
int fputws_unlocked(const wchar_t *, FILE *) libcesque __read_only(1) __read_write(2);
wint_t ungetwc_unlocked(wint_t, FILE *) libcesque paramsnonnull() __read_write(2);
int ungetc_unlocked(int, FILE *) libcesque paramsnonnull() __read_write(2);
int fseek_unlocked(FILE *, int64_t, int) libcesque paramsnonnull() __read_write(1);
ssize_t getdelim_unlocked(char **, size_t *, int, FILE *) paramsnonnull() __read_write(1) __read_write(2) __read_write(4);
int fprintf_unlocked(FILE *, const char *, ...) printfesque(2) libcesque __read_write(1) __read_only(2);
int vfprintf_unlocked(FILE *, const char *, va_list) paramsnonnull() libcesque __read_write(1) __read_only(2);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_H_ */
