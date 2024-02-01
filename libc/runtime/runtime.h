#ifndef COSMOPOLITAN_LIBC_RUNTIME_RUNTIME_H_
#define COSMOPOLITAN_LIBC_RUNTIME_RUNTIME_H_
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § runtime                                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

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

void mcount(void);
int daemon(int, int);
unsigned long getauxval(unsigned long);
int setjmp(jmp_buf)
libcesque returnstwice paramsnonnull();
void longjmp(jmp_buf, int) libcesque wontreturn paramsnonnull();
int _setjmp(jmp_buf)
libcesque returnstwice paramsnonnull();
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
int putenv(char *);
int setenv(const char *, const char *, int);
int unsetenv(const char *);
int clearenv(void);
void fpreset(void);
void *mmap(void *, uint64_t, int32_t, int32_t, int32_t, int64_t);
int munmap(void *, uint64_t);
int mprotect(void *, uint64_t, int);
int msync(void *, size_t, int);
int mlock(const void *, size_t);
int munlock(const void *, size_t);
long gethostid(void);
int sethostid(long);
char *getlogin(void);
int getlogin_r(char *, size_t);
int login_tty(int);
int getpagesize(void);
int syncfs(int) dontthrow;
int vhangup(void);
int getdtablesize(void);
int sethostname(const char *, size_t);
int acct(const char *);

#if defined(_GNU_SOURCE) || defined(_COSMO_SOURCE)
extern char **environ;
char *secure_getenv(const char *) paramsnonnull() __wur nosideeffect libcesque;
#endif

#ifdef _COSMO_SOURCE
extern int __argc;
extern char **__argv;
extern char **__envp;
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
void _intsort(int *, size_t);
void _longsort(long *, size_t);
/* diagnostics */
void ShowCrashReports(void);
int ftrace_install(void);
int ftrace_enabled(int);
int strace_enabled(int);
void __print_maps(void);
void __printargs(const char *);
/* builtin sh-like system/popen dsl */
int _cocmd(int, char **, char **);
/* executable program */
char *GetProgramExecutableName(void);
char *GetInterpreterExecutableName(char *, size_t);
int __open_executable(void);
/* execution control */
int verynice(void);
void __warn_if_powersave(void);
void _Exit1(int) libcesque wontreturn;
void __paginate(int, const char *);
/* memory management */
void _weakfree(void *);
void *_mapanon(size_t) attributeallocsize((1)) mallocesque;
void *_mapshared(size_t) attributeallocsize((1)) mallocesque;
void CheckForMemoryLeaks(void);
void CheckForFileLeaks(void);
void __enable_threads(void);
void __oom_hook(size_t);
bool32 _isheap(void *);
/* code morphing */
void __morph_begin(void);
void __morph_end(void);
void __jit_begin(void);
void __jit_end(void);
void __clear_cache(void *, void *);
/* portability */
int NtGetVersion(void) pureconst;
bool32 IsGenuineBlink(void);
bool32 IsCygwin(void);
const char *GetCpuidOs(void);
const char *GetCpuidEmulator(void);
void GetCpuidBrand(char[13], uint32_t);
long __get_rlimit(int);
const char *__describe_os(void);
long __get_sysctl(int, int);
int __get_arg_max(void) pureconst;
int __get_cpu_count(void) pureconst;
long __get_avphys_pages(void) pureconst;
long __get_phys_pages(void) pureconst;
long __get_minsigstksz(void) pureconst;
void __get_main_stack(void **, size_t *, int *);
long __get_safe_size(long, long);
char *__get_tmpdir(void);
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
