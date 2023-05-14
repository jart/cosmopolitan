#ifndef Py_PYCONFIG_H
#define Py_PYCONFIG_H
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/str/str.h"

#define STDC_HEADERS                    1
#define HAVE_LONG_DOUBLE                1
#define HAVE_GCC_UINT128_T              1
#define HAVE_STDARG_PROTOTYPES          1
#define HAVE_BUILTIN_ATOMIC             1
#define HAVE_COMPUTED_GOTOS             1
#define DOUBLE_IS_LITTLE_ENDIAN_IEEE754 1

#ifdef __x86_64__
#define HAVE_GCC_ASM_FOR_X64 1
#define HAVE_GCC_ASM_FOR_X87 1
#endif

#define HAVE_ACOSH         1
#define HAVE_ASINH         1
#define HAVE_ATANH         1
#define HAVE_COPYSIGN      1
#define HAVE_FINITE        1
#define HAVE_LGAMMA        1
#define HAVE_TGAMMA        1
#define HAVE_HYPOT         1
#define HAVE_ROUND         1
#define HAVE_LOG1P         1
#define HAVE_LOG2          1
#define HAVE_DECL_ISFINITE 1
#define HAVE_DECL_ISINF    1
#define HAVE_DECL_ISNAN    1
/* #undef HAVE_GAMMA */

#define HAVE_ACCEPT4  1
#define HAVE_ADDRINFO 1
#define HAVE_ALARM    1
#define HAVE_CLOCK    1
/* #undef HAVE_CHFLAGS */

#define HAVE_WORKING_TZSET     1
#define HAVE_STRUCT_TM_TM_ZONE 1
#define HAVE_TM_ZONE           1 /* deprecated */
#define HAVE_DECL_TZNAME       1
/* #undef HAVE_ALTZONE */
/* #undef GETTIMEOFDAY_NO_TZ */

#define HAVE_CLOCK_GETRES  1
#define HAVE_CLOCK_GETTIME 1
#define HAVE_CLOCK_SETTIME 1

/* Define to 1 if you have the `confstr' function. */
/* #undef HAVE_CONFSTR */

/* Define to 1 if you have the `ctermid' function. */
#define HAVE_CTERMID 1

/* Define if you have the 'ctermid_r' function. */
/* #undef HAVE_CTERMID_R */

/* Define if you have the 'filter' function. */
#define HAVE_CURSES_FILTER 1

/* Define if you have the 'has_key' function. */
#define HAVE_CURSES_HAS_KEY 1

/* Define if you have the 'immedok' function. */
#define HAVE_CURSES_IMMEDOK 1

/* Define if you have the 'is_pad' function or macro. */
#define HAVE_CURSES_IS_PAD 1

/* Define if you have the 'is_term_resized' function. */
#define HAVE_CURSES_IS_TERM_RESIZED 1

/* Define if you have the 'resizeterm' function. */
#define HAVE_CURSES_RESIZETERM 1

/* Define if you have the 'resize_term' function. */
#define HAVE_CURSES_RESIZE_TERM 1

/* Define if you have the 'syncok' function. */
#define HAVE_CURSES_SYNCOK 1

/* Define if you have the 'typeahead' function. */
#define HAVE_CURSES_TYPEAHEAD 1

/* Define if you have the 'use_env' function. */
#define HAVE_CURSES_USE_ENV 1

/* Define if you have the 'wchgat' function. */
#define HAVE_CURSES_WCHGAT 1

#define HAVE_DLOPEN             1
#define HAVE_DYNAMIC_LOADING    1
#define HAVE_DECL_RTLD_DEEPBIND 0
#define HAVE_DECL_RTLD_GLOBAL   1
#define HAVE_DECL_RTLD_LAZY     1
#define HAVE_DECL_RTLD_LOCAL    1
#define HAVE_DECL_RTLD_NODELETE 0
#define HAVE_DECL_RTLD_NOLOAD   0
#define HAVE_DECL_RTLD_NOW      1

#define HAVE_GETSID        1
#define HAVE_GETEUID       1
#define HAVE_GETTIMEOFDAY  1
#define HAVE_GETUID        1
#define HAVE_GETNAMEINFO   1
#define HAVE_GETPAGESIZE   1
#define HAVE_GETPEERNAME   1
#define HAVE_GETPGID       1
#define HAVE_GETPGRP       1
#define HAVE_GETPID        1
#define HAVE_GETGID        1
#define HAVE_GETPRIORITY   1
#define HAVE_GETPWENT      1
#define HAVE_GETRANDOM     1
#define HAVE_GETITIMER     1
#define HAVE_CHOWN         1
#define HAVE_CHROOT        1
#define HAVE_DIRENT_D_TYPE 1
#define HAVE_DUP2          1
#define HAVE_DUP3          1
#define HAVE_EPOLL         1
#define HAVE_EPOLL_CREATE1 1
#define HAVE_ERF           1
#define HAVE_ERFC          1
#define HAVE_EXECV         1
#define HAVE_EXPM1         1
#define HAVE_FACCESSAT     1
#define HAVE_FCHDIR        1
#define HAVE_FCHMOD        1
#define HAVE_FCHMODAT      1
#define HAVE_FCHOWN        1
#define HAVE_FCHOWNAT      1
#define HAVE_FDATASYNC     1
#define HAVE_FDOPENDIR     1
#define HAVE_FLOCK         1
#define HAVE_FORK          1
#define HAVE_FPATHCONF     1
#define HAVE_FTRUNCATE     1
#define HAVE_FUTIMENS      1
#define HAVE_FUTIMES       1
#define HAVE_FUTIMESAT     1
#define HAVE_FSTATAT       1
#define HAVE_FSYNC         1
#define HAVE_GETENTROPY    1
#define HAVE_GETLOADAVG    1
#define HAVE_FSTATVFS      1
/* #undef HAVE_FEXECVE */
/* #undef HAVE_FTIME */
/* #define HAVE_GETGROUPS    1 */
/* #define HAVE_SETGROUPS    1 */
/* #define HAVE_INITGROUPS   1 */
/* #define HAVE_GETGROUPLIST 1 */

#define HAVE_FSEEKO        1
#define HAVE_FTELLO        1
#define HAVE_GETC_UNLOCKED 1

#define HAVE_GETADDRINFO   1
#define HAVE_GAI_STRERROR  1
#define HAVE_GETHOSTBYNAME 1
/* #undef HAVE_GETHOSTBYNAME_R */
/* #undef HAVE_GETHOSTBYNAME_R_3_ARG */
/* #undef HAVE_GETHOSTBYNAME_R_5_ARG */
/* #undef HAVE_GETHOSTBYNAME_R_6_ARG */

#define HAVE_GETRESGID 1
#define HAVE_GETRESUID 1
/* #undef HAVE_GETSPENT */
/* #undef HAVE_GETSPNAM */

/* Define to 1 if you have the `getwd' function. */
/* #undef HAVE_GETWD */

/* Define if you have the 'hstrerror' function. */
/* #undef HAVE_HSTRERROR */

/* Define this if you have le64toh() */
#define HAVE_HTOLE64 1

/* Define to 1 if you have the 'if_nameindex' function. */
/* #undef HAVE_IF_NAMEINDEX */

/* Define if you have the 'inet_aton' function. */
#define HAVE_INET_ATON 1

/* Define if you have the 'inet_pton' function. */
#define HAVE_INET_PTON 1

/* Define if gcc has the ipa-pure-const bug. */
#define HAVE_IPA_PURE_CONST_BUG 1

/* Define to 1 if you have the `kill' function. */
#define HAVE_KILL 1

/* Define to 1 if you have the `killpg' function. */
#define HAVE_KILLPG 1

/* Define if you have the 'kqueue' functions. */
/* #undef HAVE_KQUEUE */

/* Defined to enable large file support when an off_t is bigger than a long
   and long long is available and at least as big as an off_t. You may need to
   add some flags for configuration and compilation to enable this mode. (For
   Solaris and Linux, the necessary defines are already defined.) */
#define HAVE_LARGEFILE_SUPPORT 1

#define HAVE_GETLOGIN 1
#define HAVE_LCHOWN   1
#define HAVE_LCHMOD   1
/* #undef HAVE_LCHFLAGS */

/* Define to 1 if you have the `dl' library (-ldl). */
/* #undef HAVE_LIBDL */

/* Define to 1 if you have the `dld' library (-ldld). */
/* #undef HAVE_LIBDLD */

/* Define to 1 if you have the `ieee' library (-lieee). */
/* #undef HAVE_LIBIEEE */

/* Define to 1 if you have the `resolv' library (-lresolv). */
/* #undef HAVE_LIBRESOLV */

/* Define to 1 if you have the `sendfile' library (-lsendfile). */
/* #undef HAVE_LIBSENDFILE */

/* Define if compiling using Linux 3.6 or later. */
/* #undef HAVE_LINUX_CAN_RAW_FD_FRAMES */

/* Define to 1 if you have the 'lockf' function and the F_LOCK macro. */
#define HAVE_LOCKF 1

/* #define HAVE_DEVICE_MACROS 1 */
#define HAVE_MAKEDEV 1
#define HAVE_MKDIRAT 1

#define HAVE_MBRTOWC 1
#define HAVE_MEMMOVE 1
#define HAVE_MEMRCHR 1

#define HAVE_SEM_GETVALUE  1
#define HAVE_SEM_OPEN      1
#define HAVE_SEM_TIMEDWAIT 1
#define HAVE_SEM_UNLINK    1

/* #define HAVE_MKNOD       1 */
/* #define HAVE_MKNODAT     1 */
/* #define HAVE_MKFIFO      1 */
/* #define HAVE_MKFIFOAT    1 */
/* #define HAVE_SETHOSTNAME 1 */

#define HAVE_DIRENT_H         1
#define HAVE_GETPPID          1
#define HAVE_LINK             1
#define HAVE_LINKAT           1
#define HAVE_LSTAT            1
#define HAVE_LUTIMES          1
#define HAVE_MKTIME           1
#define HAVE_MMAP             1
#define HAVE_NICE             1
#define HAVE_OPENAT           1
#define HAVE_PATHCONF         1
#define HAVE_PAUSE            1
#define HAVE_PIPE             1
#define HAVE_PIPE2            1
#define HAVE_POLL             1
#define HAVE_POSIX_FADVISE    1
#define HAVE_PREAD            1
#define HAVE_PUTENV           1
#define HAVE_PWRITE           1
#define HAVE_READLINK         1
#define HAVE_READLINKAT       1
#define HAVE_READV            1
#define HAVE_REALPATH         1
#define HAVE_RENAMEAT         1
#define HAVE_SELECT           1
#define HAVE_SENDFILE         1
#define HAVE_SETEGID          1
#define HAVE_SETEUID          1
#define HAVE_SETGID           1
#define HAVE_SETITIMER        1
#define HAVE_SETLOCALE        1
#define HAVE_SETPGID          1
#define HAVE_SETPRIORITY      1
#define HAVE_SETREGID         1
#define HAVE_SETRESGID        1
#define HAVE_SETRESUID        1
#define HAVE_SETREUID         1
#define HAVE_SETSID           1
#define HAVE_SETUID           1
#define HAVE_SETVBUF          1
#define HAVE_SIGACTION        1
#define HAVE_SIGALTSTACK      1
#define HAVE_SIGINTERRUPT     1
#define HAVE_SOCKADDR_STORAGE 1
#define HAVE_SYMLINK          1
#define HAVE_SYMLINKAT        1
#define HAVE_SYNC             1
#define HAVE_SYSCONF          1
#define HAVE_TCGETPGRP        1
#define HAVE_TCSETPGRP        1
#define HAVE_TEMPNAM          1
#define HAVE_TIMEGM           1
#define HAVE_TIMES            1
#define HAVE_TMPFILE          1
#define HAVE_TRUNCATE         1
#define HAVE_UNAME            1
#define HAVE_UNLINKAT         1
#define HAVE_UNSETENV         1
#define HAVE_UTIMENSAT        1
#define HAVE_UTIMES           1
#define HAVE_WAIT             1
#define HAVE_WAIT3            1
#define HAVE_WAIT4            1
#define HAVE_WAITPID          1
#define HAVE_STATVFS          1

/* #define HAVE_MREMAP 1 */
/* #undef HAVE_PLOCK */
/* #undef HAVE_POSIX_FALLOCATE */
/* #undef HAVE_PRLIMIT */
/* #undef HAVE_WAITID */
/* #undef HAVE_TMPNAM */
/* #undef HAVE_TMPNAM_R */
/* #undef HAVE_SETPGRP */
/* #undef HAVE_STAT_TV_NSEC2 */
/* #undef HAVE_SIGPENDING */
/* #undef HAVE_SIGRELSE */
/* #undef HAVE_SIGTIMEDWAIT */
/* #undef HAVE_SIGWAIT */
/* #undef HAVE_SIGWAITINFO */
/* #undef HAVE_SOCKADDR_ALG */
/* #undef HAVE_SOCKADDR_SA_LEN */
/* #undef HAVE_STD_ATOMIC */

#define HAVE_SNPRINTF 1
#define HAVE_STRDUP   1
#define HAVE_STRFTIME 1
#define HAVE_STRLCPY  1
#define HAVE_WMEMCMP  1
/* #undef HAVE_WCSCOLL */
/* #undef HAVE_WCSFTIME */
/* #undef HAVE_WCSXFRM */

#define HAVE_USABLE_WCHAR_T                 1
#define HAVE_SOCKETPAIR                     1
#define HAVE_SSIZE_T                        1
#define HAVE_STAT_TV_NSEC                   1
#define HAVE_STRUCT_PASSWD_PW_GECOS         1
#define HAVE_STRUCT_PASSWD_PW_PASSWD        1
#define HAVE_STRUCT_STAT_ST_BIRTHTIME       1
#define HAVE_STRUCT_STAT_ST_BLKSIZE         1
#define HAVE_STRUCT_STAT_ST_BLOCKS          1
#define HAVE_STRUCT_STAT_ST_FLAGS           1
#define HAVE_STRUCT_STAT_ST_GEN             1
#define HAVE_STRUCT_STAT_ST_RDEV            1
#define HAVE_STRUCT_STAT_ST_FILE_ATTRIBUTES 1
/* #undef HAVE_UCS4_TCL */

/* Define to 1 if you have the `writev' function. */
#define HAVE_WRITEV 1

/* Define if the zlib library has inflateCopy */
#define HAVE_ZLIB_COPY 1

/* Define if log1p(-0.) is 0. rather than -0. */
/* #undef LOG1P_DROPS_ZERO_SIGN */

/* Define to 1 if `major', `minor', and `makedev' are declared in <mkdev.h>.
 */
/* #undef MAJOR_IN_MKDEV */

/* Define to 1 if `major', `minor', and `makedev' are declared in
   <sysmacros.h>. */
/* #undef MAJOR_IN_SYSMACROS */

/* Define if mvwdelch in curses.h is an expression. */
#define MVWDELCH_IS_EXPRESSION 1

/* Define to the address where bug reports for this package should be sent. */
/* #undef PACKAGE_BUGREPORT */

/* Define to the full name of this package. */
/* #undef PACKAGE_NAME */

/* Define to the full name and version of this package. */
/* #undef PACKAGE_STRING */

/* Define to the one symbol short name of this package. */
/* #undef PACKAGE_TARNAME */

/* Define to the home page for this package. */
/* #undef PACKAGE_URL */

/* Define to the version of this package. */
/* #undef PACKAGE_VERSION */

/* Define if POSIX semaphores aren't enabled on your system */
/* #define POSIX_SEMAPHORES_NOT_ENABLED 1 */

/* Defined if PTHREAD_SCOPE_SYSTEM supported. */
/* #undef PTHREAD_SYSTEM_SCHED_SUPPORTED */

#define HAVE_PTHREAD_ATFORK     1
#define HAVE_PTHREAD_DESTRUCTOR 1
/* #undef HAVE_PTHREAD_INIT ??? */
#define HAVE_PTHREAD_KILL    1
#define HAVE_PTHREAD_SIGMASK 1

/* Define as the preferred size in bits of long digits */
/* #undef PYLONG_BITS_IN_DIGIT */

/* Define to printf format modifier for Py_ssize_t */
#define PY_FORMAT_SIZE_T "z"

/* Defined if Python is built as a shared library. */
/* #undef Py_ENABLE_SHARED */

/* Define hash algorithm for str, bytes and memoryview. SipHash24: 1, FNV: 2,
   externally defined: 0 */
/* #undef Py_HASH_ALGORITHM */

/* assume C89 semantics that RETSIGTYPE is always void */
#define RETSIGTYPE void

/* Define if setpgrp() must be called as setpgrp(0, 0). */
/* #undef SETPGRP_HAVE_ARG */

#define SIZEOF_DOUBLE      __SIZEOF_DOUBLE__
#define SIZEOF_FLOAT       __SIZEOF_FLOAT__
#define SIZEOF_FPOS_T      8
#define SIZEOF_INT         __SIZEOF_INT__
#define SIZEOF_LONG        __SIZEOF_LONG__
#define SIZEOF_LONG_DOUBLE __SIZEOF_LONG_DOUBLE__
#define SIZEOF_LONG_LONG   __SIZEOF_LONG_LONG__
#define SIZEOF_OFF_T       8
#define SIZEOF_PID_T       4
#define SIZEOF_SHORT       __SIZEOF_SHORT__
#define SIZEOF_SIZE_T      __SIZEOF_SIZE_T__
#define SIZEOF_TIME_T      8
#define SIZEOF_UINTPTR_T   8
#define SIZEOF_VOID_P      8
#define SIZEOF_WCHAR_T     __SIZEOF_WCHAR_T__
#define SIZEOF__BOOL       1
/* #undef SIZEOF_PTHREAD_T */

/* Define if you can safely include both <sys/select.h> and <sys/time.h>
   (which you can't on SCO ODT 3.0). */
#define SYS_SELECT_WITH_SYS_TIME 1

/* Define if tanh(-0.) is -0., or if platform doesn't have signed zeros */
#define TANH_PRESERVES_ZERO_SIGN 1

/* Library needed by timemodule.c: librt may be needed for clock_gettime() */
/* #undef TIMEMODULE_LIB */

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Define if you want to use computed gotos in ceval.c. */
#define USE_COMPUTED_GOTOS 1

/* Define to use the C99 inline keyword. */
#define USE_INLINE 1

/* Define if you want SIGFPE handled (see Include/pyfpe.h). */
/* #undef WANT_SIGFPE_HANDLER */

/* Define if WINDOW in curses.h offers a field _flags. */
#define WINDOW_HAS_FLAGS 1

/* Define if you want documentation strings in extension modules */
#if !IsTiny()
#define WITH_DOC_STRINGS 1
#endif

/* Define if you want to compile in DTrace support */
/* #undef WITH_DTRACE */

/* Define if you want to use the new-style (Openstep, Rhapsody, MacOS) dynamic
   linker (dyld) instead of the old-style (NextStep) dynamic linker (rld).
   Dyld is necessary to support frameworks. */
/* #undef WITH_DYLD */

/* Define to 1 if libintl is needed for locale functions. */
/* #undef WITH_LIBINTL */

/* Define if you want to compile in Python-specific mallocs */
#ifndef __SANITIZE_ADDRESS__
#define WITH_PYMALLOC 0
#endif

/* Define if you want to compile in rudimentary thread support */
#ifndef WITH_THREAD
#define WITH_THREAD 1
#endif

/* Define if you want pymalloc to be disabled when running under valgrind */
/* #undef WITH_VALGRIND */

/* Define if arithmetic is subject to x87-style double rounding issue */
/* #undef X87_DOUBLE_ROUNDING */

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define if you have POSIX threads, and your system does not define that. */
#ifndef _POSIX_THREADS
#define _POSIX_THREADS
#endif

/* #define _Py_MEMORY_SANITIZER */

/* #define HAVE_DEV_PTMX 1 */
/* #define HAVE_OPENPTY  1 */
/* #undef HAVE__GETPTY */
/* #undef HAVE_DEV_PTC */
/* #define HAVE_FORKPTY 1 */
/* #define HAVE_SETGROUPS 1 */

/* define to 1 if your sem_getvalue is broken. */
/* #define HAVE_BROKEN_SEM_GETVALUE 1 */
/* Define if --enable-ipv6 is specified */
/* #undef ENABLE_IPV6 */
/* Define if flock needs to be linked with bsd library. */
/* #undef FLOCK_NEEDS_LIBBSD */
/* Define if getpgrp() must be called as getpgrp(0). */
/* #undef GETPGRP_HAVE_ARG */
/* Define to 1 if you have the `bind_textdomain_codeset' function. */
/* #undef HAVE_BIND_TEXTDOMAIN_CODESET */
/* Define if mbstowcs(NULL, "text", 0) does not return the number of wide
   chars that would be converted. */
/* #undef HAVE_BROKEN_MBSTOWCS */
/* Define if nice() returns success/failure instead of the new priority. */
/* #undef HAVE_BROKEN_NICE */
/* Define if the system reports an invalid PIPE_BUF value. */
/* #undef HAVE_BROKEN_PIPE_BUF */
/* Define if poll() sets errno on invalid file descriptors. */
/* #undef HAVE_BROKEN_POLL */
/* Define if the Posix semaphores do not work on your system */
/* #undef HAVE_BROKEN_POSIX_SEMAPHORES */
/* Define if pthread_sigmask() does not work on your system. */
/* #undef HAVE_BROKEN_PTHREAD_SIGMASK */
/* Define if `unsetenv` does not return an int. */
/* #undef HAVE_BROKEN_UNSETENV */

#define HAVE_SCHED_SETAFFINITY      1
#define HAVE_SCHED_SETPARAM         1
#define HAVE_SCHED_SETSCHEDULER     1
#define HAVE_SCHED_GET_PRIORITY_MAX 1
#define HAVE_SCHED_RR_GET_INTERVAL  1

#define Py_NSIG           32
#define HAVE_SYSTEM       1
#define SELECT_USES_HEAP  1
#define OPENSSL_NO_SCRYPT 1
#define OPENSSL_NO_COMP   1
#define HAVE_LANGINFO_H   1

#if IsModeDbg()
#define Py_DEBUG 1
#endif

/* #define FAST_LOOPS 1 /\* froot loops *\/ */

#ifdef __SANITIZE_UNDEFINED__
#define HAVE_ALIGNED_REQUIRED 1
#endif

#endif /*Py_PYCONFIG_H*/
