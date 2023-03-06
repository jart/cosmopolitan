#ifndef COSMOPOLITAN_LIBC_RUNTIME_RUNTIME_H_
#define COSMOPOLITAN_LIBC_RUNTIME_RUNTIME_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § runtime                                                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef long jmp_buf[8];
typedef long sigjmp_buf[12];

extern char **environ;                      /* CRT */
extern int __argc;                          /* CRT */
extern char **__argv;                       /* CRT */
extern char **__envp;                       /* CRT */
extern unsigned long *__auxv;               /* CRT */
extern intptr_t __oldstack;                 /* CRT */
extern uint64_t __nosync;                   /* SYS */
extern int __strace;                        /* SYS */
extern int __ftrace;                        /* SYS */
extern char *program_invocation_name;       /* RII */
extern char *program_invocation_short_name; /* RII */
extern uint64_t __syscount;                 /* RII */
extern uint64_t kStartTsc;                  /* RII */
extern char kTmpPath[];                     /* RII */
extern const char kNtSystemDirectory[];     /* RII */
extern const char kNtWindowsDirectory[];    /* RII */
extern size_t __virtualmax;
extern bool __isworker;

void mcount(void);
int daemon(int, int);
int _freestack(void *);
void _bt(const char *, ...);
int _cocmd(int, char **, char **);
unsigned long getauxval(unsigned long);
void *_mapanon(size_t) attributeallocsize((1)) mallocesque;
void *_mapshared(size_t) attributeallocsize((1)) mallocesque;
void *_mapstack(void) returnsaligned((FRAMESIZE)) mallocesque;
int setjmp(jmp_buf)
libcesque returnstwice paramsnonnull();
void longjmp(jmp_buf, int) libcesque wontreturn paramsnonnull();
axdx_t setlongerjmp(jmp_buf)
libcesque returnstwice paramsnonnull();
void longerjmp(jmp_buf, intptr_t) libcesque wontreturn paramsnonnull();
int _setjmp(jmp_buf)
libcesque returnstwice paramsnonnull();
int sigsetjmp(sigjmp_buf, int) libcesque returnstwice paramsnonnull();
void siglongjmp(sigjmp_buf, int) libcesque wontreturn paramsnonnull();
void _longjmp(jmp_buf, int) libcesque wontreturn paramsnonnull();
void exit(int) wontreturn;
void _exit(int) libcesque wontreturn;
void _Exit(int) libcesque wontreturn;
void _Exitr(int) libcesque wontreturn;
void _Exit1(int) libcesque wontreturn;
void quick_exit(int) wontreturn;
void abort(void) wontreturn;
int __cxa_atexit(void *, void *, void *) libcesque;
int atfork(void *, void *) libcesque;
int atexit(void (*)(void)) libcesque;
char *getenv(const char *) nosideeffect libcesque;
int putenv(char *) paramsnonnull();
int setenv(const char *, const char *, int);
int unsetenv(const char *);
int clearenv(void);
void fpreset(void);
void *mmap(void *, uint64_t, int32_t, int32_t, int32_t, int64_t);
void *mremap(void *, size_t, size_t, int, ...);
int munmap(void *, uint64_t);
int mprotect(void *, uint64_t, int) privileged;
int msync(void *, size_t, int);
void *sbrk(intptr_t);
int brk(void *);
long gethostid(void);
int sethostid(long);
char *getlogin(void);
int getlogin_r(char *, size_t);
int login_tty(int);
int getpagesize(void);
int syncfs(int);
int vhangup(void);
int getdtablesize(void);
int sethostname(const char *, size_t);
int acct(const char *);
void _intsort(int *, size_t);
void _longsort(long *, size_t);
bool _isheap(void *);
int NtGetVersion(void) pureconst;
unsigned _getcpucount(void) pureconst;
long _missingno();
void __oom_hook(size_t);
void _loadxmm(void *);
void _peekall(void);
void _savexmm(void *);
void _weakfree(void *);
void free_s(void *) paramsnonnull() libcesque;
int _OpenExecutable(void);
int ftrace_install(void);
int ftrace_enabled(int);
int strace_enabled(int);
long _GetResourceLimit(int);
long _GetMaxFd(void);
char *GetProgramExecutableName(void);
char *GetInterpreterExecutableName(char *, size_t);
void __printargs(const char *);
void __paginate(int, const char *);
int __arg_max(void);
void __print_maps(void);
void __warn_if_powersave(void);
const char *__describe_os(void);
bool _IsDynamicExecutable(const char *);
void _restorewintty(void);
const char *GetCpuidOs(void);
const char *GetCpuidEmulator(void);
void GetCpuidBrand(char[13], uint32_t);
bool IsGenuineBlink(void);
bool IsCygwin(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_RUNTIME_H_ */
