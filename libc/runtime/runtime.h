#ifndef COSMOPOLITAN_LIBC_RUNTIME_RUNTIME_H_
#define COSMOPOLITAN_LIBC_RUNTIME_RUNTIME_H_
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § runtime                                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
/* clang-format off */

#ifdef __x86_64__
typedef long jmp_buf[8];
typedef long sigjmp_buf[11];
#elif defined(__aarch64__)
typedef long jmp_buf[22];
typedef long sigjmp_buf[25];
#elif defined(__powerpc64__)
typedef unsigned __int128 jmp_buf[32];
#elif defined(__s390x__)
typedef unsigned long jmp_buf[18];
#elif defined(__riscv)
typedef unsigned long jmp_buf[26];
#endif

void mcount(void) libcesque;
int daemon(int, int) libcesque;
unsigned long getauxval(unsigned long) libcesque;
int setjmp(jmp_buf) libcesque returnstwice paramsnonnull();
void longjmp(jmp_buf, int) libcesque wontreturn paramsnonnull();
int _setjmp(jmp_buf) libcesque returnstwice paramsnonnull();
int sigsetjmp(sigjmp_buf, int) libcesque returnstwice paramsnonnull();
void siglongjmp(sigjmp_buf, int) libcesque wontreturn paramsnonnull();
void _longjmp(jmp_buf, int) libcesque wontreturn paramsnonnull();
void exit(int) wontreturn;
void _exit(int) libcesque wontreturn;
void _Exit(int) libcesque wontreturn;
void quick_exit(int) wontreturn;
void abort(void) wontreturn;
int atexit(void (*)(void)) paramsnonnull() libcesque;
char *getenv(const char *) paramsnonnull() __wur nosideeffect libcesque;
int putenv(char *) libcesque __read_write(1);
int setenv(const char *, const char *, int) libcesque;
int unsetenv(const char *) libcesque;
int clearenv(void) libcesque;
void fpreset(void) libcesque;
void *mmap(void *, uint64_t, int32_t, int32_t, int32_t, int64_t) libcesque;
void *mremap(void *, size_t, size_t, int, ...) libcesque;
int munmap(void *, uint64_t) libcesque;
int mprotect(void *, uint64_t, int) libcesque;
int msync(void *, size_t, int) libcesque;
int mlock(const void *, size_t) libcesque;
int munlock(const void *, size_t) libcesque;
long gethostid(void) libcesque;
int sethostid(long) libcesque;
char *getlogin(void) libcesque;
int getlogin_r(char *, size_t) libcesque __write_only(1, 2);
int login_tty(int) libcesque __fd_arg(1);
int getpagesize(void) pureconst libcesque;
int getgransize(void) pureconst libcesque;
int syncfs(int) dontthrow libcesque;
int vhangup(void) libcesque;
int getdtablesize(void) libcesque;
int sethostname(const char *, size_t) libcesque;
int acct(const char *) libcesque;

#if defined(_GNU_SOURCE) || defined(_COSMO_SOURCE)
extern char **environ;
char *secure_getenv(const char *) paramsnonnull() __wur nosideeffect libcesque;
#endif

#ifdef _COSMO_SOURCE
extern int __argc;
extern char **__argv;
extern char **__envp;
extern int __pagesize;
extern int __gransize;
extern unsigned long *__auxv;
extern intptr_t __oldstack;
extern char *__program_executable_name;
extern uint64_t __nosync;
extern int __strace;
extern int __ftrace;
extern uint64_t __syscount;
extern uint64_t kStartTsc;
extern const char kNtSystemDirectory[];
extern const char kNtWindowsDirectory[];
extern size_t __virtualmax;
extern size_t __stackmax;
extern bool32 __isworker;
/* utilities */
void _intsort(int *, size_t) libcesque __read_write(1, 2);
void _longsort(long *, size_t) libcesque __read_write(1, 2);
/* diagnostics */
void ShowCrashReports(void) libcesque;
int ftrace_install(void) libcesque;
int ftrace_enabled(int) libcesque;
int strace_enabled(int) libcesque;
void __print_maps(size_t) libcesque;
void __print_maps_win32(int64_t, const char *, size_t) libcesque;
void __printargs(const char *) libcesque;
/* builtin sh-like system/popen dsl */
int _cocmd(int, char **, char **) libcesque;
/* executable program */
char *GetProgramExecutableName(void) libcesque;
char *GetInterpreterExecutableName(char *, size_t) libcesque;
int __open_executable(void) libcesque;
/* execution control */
int verynice(void) libcesque;
void __warn_if_powersave(void) libcesque;
void _Exit1(int) libcesque wontreturn libcesque;
void __paginate(int, const char *) libcesque __fd_arg(1);
void __paginate_file(int, const char *) libcesque __fd_arg(1);
/* memory management */
void _weakfree(void *) libcesque;
void *_mapanon(size_t) attributeallocsize((1)) mallocesque libcesque;
void *_mapshared(size_t) attributeallocsize((1)) mallocesque libcesque;
void CheckForFileLeaks(void) libcesque;
void __oom_hook(size_t) libcesque;
/* code morphing */
void __morph_begin(void) libcesque;
void __morph_end(void) libcesque;
void __jit_begin(void) libcesque;
void __jit_end(void) libcesque;
void __clear_cache(void *, void *);
/* portability */
bool32 IsGenuineBlink(void) libcesque;
bool32 IsCygwin(void) libcesque;
const char *GetCpuidOs(void) libcesque;
const char *GetCpuidEmulator(void) libcesque;
void GetCpuidBrand(char[13], uint32_t) libcesque;
long __get_rlimit(int) libcesque;
const char *__describe_os(void) libcesque;
long __get_sysctl(int, int) libcesque;
int __get_arg_max(void) pureconst libcesque;
int __get_cpu_count(void) pureconst libcesque;
long __get_avphys_pages(void) pureconst libcesque;
long __get_phys_pages(void) pureconst libcesque;
long __get_minsigstksz(void) pureconst libcesque;
long __get_safe_size(long, long) libcesque;
char *__get_tmpdir(void) libcesque;
forceinline int __trace_disabled(int x) {
  return 0;
}
#ifndef FTRACE
#define ftrace_enabled(...) __trace_disabled(__VA_ARGS__)
#endif
#ifndef SYSDEBUG
#define strace_enabled(...) __trace_disabled(__VA_ARGS__)
#endif
#endif /* _COSMO_SOURCE */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RUNTIME_RUNTIME_H_ */
