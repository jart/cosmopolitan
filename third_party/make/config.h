/* src/config.h.  Generated from config.h.in by configure.  */
/* src/config.h.in.  Generated from configure.ac by autoheader.  */

/* CPU and C ABI indicator */
#ifndef __i386__
#define __i386__ 1
#endif
#ifndef __x86_64_x32__
/* #undef __x86_64_x32__ */
#endif
#ifndef __x86_64__
/* #undef __x86_64__ */
#endif
#ifndef __alpha__
/* #undef __alpha__ */
#endif
#ifndef __arm__
/* #undef __arm__ */
#endif
#ifndef __armhf__
/* #undef __armhf__ */
#endif
#ifndef __arm64_ilp32__
/* #undef __arm64_ilp32__ */
#endif
#ifndef __arm64__
/* #undef __arm64__ */
#endif
#ifndef __hppa__
/* #undef __hppa__ */
#endif
#ifndef __hppa64__
/* #undef __hppa64__ */
#endif
#ifndef __ia64_ilp32__
/* #undef __ia64_ilp32__ */
#endif
#ifndef __ia64__
/* #undef __ia64__ */
#endif
#ifndef __loongarch64__
/* #undef __loongarch64__ */
#endif
#ifndef __m68k__
/* #undef __m68k__ */
#endif
#ifndef __mips__
/* #undef __mips__ */
#endif
#ifndef __mipsn32__
/* #undef __mipsn32__ */
#endif
#ifndef __mips64__
/* #undef __mips64__ */
#endif
#ifndef __powerpc__
/* #undef __powerpc__ */
#endif
#ifndef __powerpc64__
/* #undef __powerpc64__ */
#endif
#ifndef __powerpc64_elfv2__
/* #undef __powerpc64_elfv2__ */
#endif
#ifndef __riscv32__
/* #undef __riscv32__ */
#endif
#ifndef __riscv64__
/* #undef __riscv64__ */
#endif
#ifndef __riscv32_ilp32__
/* #undef __riscv32_ilp32__ */
#endif
#ifndef __riscv32_ilp32f__
/* #undef __riscv32_ilp32f__ */
#endif
#ifndef __riscv32_ilp32d__
/* #undef __riscv32_ilp32d__ */
#endif
#ifndef __riscv64_ilp32__
/* #undef __riscv64_ilp32__ */
#endif
#ifndef __riscv64_ilp32f__
/* #undef __riscv64_ilp32f__ */
#endif
#ifndef __riscv64_ilp32d__
/* #undef __riscv64_ilp32d__ */
#endif
#ifndef __riscv64_lp64__
/* #undef __riscv64_lp64__ */
#endif
#ifndef __riscv64_lp64f__
/* #undef __riscv64_lp64f__ */
#endif
#ifndef __riscv64_lp64d__
/* #undef __riscv64_lp64d__ */
#endif
#ifndef __s390__
/* #undef __s390__ */
#endif
#ifndef __s390x__
/* #undef __s390x__ */
#endif
#ifndef __sh__
/* #undef __sh__ */
#endif
#ifndef __sparc__
/* #undef __sparc__ */
#endif
#ifndef __sparc64__
/* #undef __sparc64__ */
#endif


/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 if the `closedir' function returns void instead of int. */
/* #undef CLOSEDIR_VOID */

/* Define to 1 if using 'alloca.c'. */
/* #undef C_ALLOCA */

/* Define to 1 for DGUX with <sys/dg_sys_info.h>. */
/* #undef DGUX */

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
/* #undef ENABLE_NLS */

/* Use high resolution file timestamps if nonzero. */
#define FILE_TIMESTAMP_HI_RES 1

/* Define to 1 when the gnulib module getloadavg should be tested. */
#define GNULIB_TEST_GETLOADAVG 1

/* Define to 1 if you have 'alloca' after including <alloca.h>, a header that
   may be supplied by this distribution. */
#define HAVE_ALLOCA 1

/* Define to 1 if <alloca.h> works. */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the `atexit' function. */
#define HAVE_ATEXIT 1

/* Use case insensitive file names */
/* #undef HAVE_CASE_INSENSITIVE_FS */

/* Define to 1 if you have the Mac OS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
/* #undef HAVE_CFLOCALECOPYCURRENT */

/* Define to 1 if you have the Mac OS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
/* #undef HAVE_CFPREFERENCESCOPYAPPVALUE */

/* Define to 1 if you have the clock_gettime function. */
#define HAVE_CLOCK_GETTIME 1

/* Define to 1 if bool, true and false work as per C2023. */
/* #undef HAVE_C_BOOL */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
/* #undef HAVE_DCGETTEXT */

/* Define to 1 if you have the declaration of `bsd_signal', and to 0 if you
   don't. */
#define HAVE_DECL_BSD_SIGNAL 0

/* Define to 1 if you have the declaration of `dlerror', and to 0 if you
   don't. */
/* #define HAVE_DECL_DLERROR 1 */

/* Define to 1 if you have the declaration of `dlopen', and to 0 if you don't.
   */
/* #define HAVE_DECL_DLOPEN 1 */

/* Define to 1 if you have the declaration of `dlsym', and to 0 if you don't.
   */
/* #define HAVE_DECL_DLSYM 1 */

/* Define to 1 if you have the declaration of 'getloadavg'. */
#define HAVE_DECL_GETLOADAVG 1

/* Define to 1 if you have the declaration of `sys_siglist', and to 0 if you
   don't. */
#define HAVE_DECL_SYS_SIGLIST 0

/* Define to 1 if you have the declaration of `_sys_siglist', and to 0 if you
   don't. */
#define HAVE_DECL__SYS_SIGLIST 0

/* Define to 1 if you have the declaration of `__sys_siglist', and to 0 if you
   don't. */
#define HAVE_DECL___SYS_SIGLIST 0

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Support DOS-style pathnames. */
/* #undef HAVE_DOS_PATHS */

/* Define to 1 if you have the `dup' function. */
#define HAVE_DUP 1

/* Define to 1 if you have the `dup2' function. */
#define HAVE_DUP2 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fdopen' function. */
#define HAVE_FDOPEN 1

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define to 1 if you have the `getgroups' function. */
#define HAVE_GETGROUPS 1

/* Define to 1 if you have the `gethostbyname' function. */
/* #undef HAVE_GETHOSTBYNAME */

/* Define to 1 if you have the `gethostname' function. */
/* #undef HAVE_GETHOSTNAME */

/* Define to 1 if you have the `getrlimit' function. */
#define HAVE_GETRLIMIT 1

/* Define if the GNU gettext() function is already present or preinstalled. */
/* #undef HAVE_GETTEXT */

/* Define to 1 if you have a standard gettimeofday function */
#define HAVE_GETTIMEOFDAY 1

/* Embed GNU Guile support */
/* #undef HAVE_GUILE */

/* Define if you have the iconv() function and it works. */
#define HAVE_ICONV 1

/* Define to 1 if the system has the type `intmax_t'. */
#define HAVE_INTMAX_T 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `isatty' function. */
#define HAVE_ISATTY 1

/* Define to 1 if you have the `dgc' library (-ldgc). */
/* #undef HAVE_LIBDGC */

/* Define to 1 if you have the `kstat' library (-lkstat). */
/* #undef HAVE_LIBKSTAT */

/* Define to 1 if you have the `perfstat' library (-lperfstat). */
/* #undef HAVE_LIBPERFSTAT */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if the system has the type 'long long int'. */
#define HAVE_LONG_LONG_INT 1

/* Define to 1 if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define to 1 if you have the <mach/mach.h> header file. */
/* #undef HAVE_MACH_MACH_H */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mempcpy' function. */
#define HAVE_MEMPCPY 1

/* Define to 1 if you have the `memrchr' function. */
#define HAVE_MEMRCHR 1

/* Define to 1 if you have the <minix/config.h> header file. */
/* #undef HAVE_MINIX_CONFIG_H */

/* Define to 1 if you have the `mkfifo' function. */
/* #undef HAVE_MKFIFO */

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if you have the `mktemp' function. */
#define HAVE_MKTEMP 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <nlist.h> header file. */
/* #undef HAVE_NLIST_H */

/* Define to 1 if you have the `pipe' function. */
#define HAVE_PIPE 1

/* Define to 1 if you have the `posix_spawn' function. */
#define HAVE_POSIX_SPAWN 1

/* Define to 1 if you have the `posix_spawnattr_setsigmask' function. */
#define HAVE_POSIX_SPAWNATTR_SETSIGMASK 1

/* Define to 1 if you have the `pselect' function. */
#define HAVE_PSELECT 1

/* Define to 1 if you have the `pstat_getdynamic' function. */
/* #undef HAVE_PSTAT_GETDYNAMIC */

/* Define to 1 if you have the `readlink' function. */
#define HAVE_READLINK 1

/* Define to 1 if you have the `realpath' function. */
#define HAVE_REALPATH 1

/* Define to 1 if <signal.h> defines the SA_RESTART constant. */
#define HAVE_SA_RESTART 1

/* Define to 1 if you have the `setegid' function. */
#define HAVE_SETEGID 1

/* Define to 1 if you have the `seteuid' function. */
#define HAVE_SETEUID 1

/* Define to 1 if you have the `setlinebuf' function. */
#define HAVE_SETLINEBUF 1

/* Define to 1 if you have the `setregid' function. */
#define HAVE_SETREGID 1

/* Define to 1 if you have the `setreuid' function. */
#define HAVE_SETREUID 1

/* Define to 1 if you have the `setrlimit' function. */
#define HAVE_SETRLIMIT 1

/* Define to 1 if you have the `setvbuf' function. */
#define HAVE_SETVBUF 1

/* Define to 1 if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define to 1 if you have the `sigsetmask' function. */
/* #undef HAVE_SIGSETMASK */

/* Define to 1 if the system has the type `sig_atomic_t'. */
#define HAVE_SIG_ATOMIC_T 1

/* Define to 1 if you have the `socket' function. */
/* #undef HAVE_SOCKET */

/* Define to 1 if you have the <spawn.h> header file. */
#define HAVE_SPAWN_H 1

/* Define to 1 if you have the <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `stpcpy' function. */
#define HAVE_STPCPY 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strcmpi' function. */
/* #undef HAVE_STRCMPI */

/* Define to 1 if you have the `strcoll' function and it is properly defined.
   */
#define HAVE_STRCOLL 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the `stricmp' function. */
/* #undef HAVE_STRICMP */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the `strncmpi' function. */
/* #undef HAVE_STRNCMPI */

/* Define to 1 if you have the `strndup' function. */
#define HAVE_STRNDUP 1

/* Define to 1 if you have the `strnicmp' function. */
/* #undef HAVE_STRNICMP */

/* Define to 1 if you have the `strsignal' function. */
#define HAVE_STRSIGNAL 1

/* Define to 1 if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* Define to 1 if `d_type' is a member of `struct dirent'. */
#define HAVE_STRUCT_DIRENT_D_TYPE 1

/* Define to 1 if `n_un.n_name' is a member of `struct nlist'. */
/* #undef HAVE_STRUCT_NLIST_N_UN_N_NAME */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/file.h> header file. */
#define HAVE_SYS_FILE_H 1

/* Define to 1 if you have the <sys/loadavg.h> header file. */
/* #undef HAVE_SYS_LOADAVG_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/timeb.h> header file. */
/* #undef HAVE_SYS_TIMEB_H */

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the `ttyname' function. */
#define HAVE_TTYNAME 1

/* Define to 1 if the system has the type `uintmax_t'. */
#define HAVE_UINTMAX_T 1

/* Define to 1 if you have the `umask' function. */
#define HAVE_UMASK 1

/* Define to 1 if you have the 'union wait' type in <sys/wait.h>. */
/* #undef HAVE_UNION_WAIT */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if the system has the type 'unsigned long long int'. */
#define HAVE_UNSIGNED_LONG_LONG_INT 1

/* Define to 1 if you have the `vfork' function. */
#define HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the `wait3' function. */
#define HAVE_WAIT3 1

/* Define to 1 if you have the `waitpid' function. */
#define HAVE_WAITPID 1

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define to 1 if `fork' works. */
#define HAVE_WORKING_FORK 1

/* Define to 1 if `vfork' works. */
#define HAVE_WORKING_VFORK 1

/* Default C++ compiler. */
#define MAKE_CXX "c++"

/* Build host information. */
#define MAKE_HOST "x86_64-linux-cosmo"

/* Define to 1 to enable job server support in GNU Make. */
#define MAKE_JOBSERVER 1

/* Define to 1 to enable 'load' support in GNU Make. */
/* #define MAKE_LOAD 1 */

/* Define to 1 to enable symbolic link timestamp checking. */
#define MAKE_SYMLINKS 1

/* Define to 1 if config.h is generated by running the configure script. */
#define MK_CONFIGURE 1

/* Define to 1 if the nlist n_name member is a pointer */
/* #undef N_NAME_POINTER */

/* Name of package */
#define PACKAGE "make"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "bug-make@gnu.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "GNU Make"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "GNU Make 4.4.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "make"

/* Define to the home page for this package. */
#define PACKAGE_URL "https://www.gnu.org/software/make/"

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.4.1"

/* Define to the character that separates directories in PATH. */
#define PATH_SEPARATOR_CHAR ':'

/* Define to the name of the SCCS 'get' command. */
#define SCCS_GET "get"

/* Define to 1 if the SCCS 'get' command understands the '-G<file>' option. */
/* #undef SCCS_GET_MINUS_G */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at runtime.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if the `S_IS*' macros in <sys/stat.h> do not work properly. */
/* #undef STAT_MACROS_BROKEN */

/* Define to 1 if all of the C90 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Define if struct stat contains a nanoseconds field */
#define ST_MTIM_NSEC st_mtim.tv_nsec

/* Define to 1 on System V Release 4. */
/* #undef SVR4 */

/* Define to 1 for Encore UMAX. */
/* #undef UMAX */

/* Define to 1 for Encore UMAX 4.3 that has <inq_status/cpustats.h> instead of
   <sys/cpustats.h>. */
/* #undef UMAX4_3 */

/* Define to 1 to use posix_spawn(). */
#define USE_POSIX_SPAWN 1

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable general extensions on macOS.  */
#ifndef _DARWIN_C_SOURCE
# define _DARWIN_C_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable X/Open compliant socket functions that do not require linking
   with -lxnet on HP-UX 11.11.  */
#ifndef _HPUX_ALT_XOPEN_SOCKET_API
# define _HPUX_ALT_XOPEN_SOCKET_API 1
#endif
/* Identify the host operating system as Minix.
   This macro does not affect the system headers' behavior.
   A future release of Autoconf may stop defining this macro.  */
#ifndef _MINIX
/* # undef _MINIX */
#endif
/* Enable general extensions on NetBSD.
   Enable NetBSD compatibility extensions on Minix.  */
#ifndef _NETBSD_SOURCE
# define _NETBSD_SOURCE 1
#endif
/* Enable OpenBSD compatibility extensions on NetBSD.
   Oddly enough, this does nothing on OpenBSD.  */
#ifndef _OPENBSD_SOURCE
# define _OPENBSD_SOURCE 1
#endif
/* Define to 1 if needed for POSIX-compatible behavior.  */
#ifndef _POSIX_SOURCE
/* # undef _POSIX_SOURCE */
#endif
/* Define to 2 if needed for POSIX-compatible behavior.  */
#ifndef _POSIX_1_SOURCE
/* # undef _POSIX_1_SOURCE */
#endif
/* Enable POSIX-compatible threading on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-5:2014.  */
#ifndef __STDC_WANT_IEC_60559_ATTRIBS_EXT__
# define __STDC_WANT_IEC_60559_ATTRIBS_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-1:2014.  */
#ifndef __STDC_WANT_IEC_60559_BFP_EXT__
# define __STDC_WANT_IEC_60559_BFP_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-2:2015.  */
#ifndef __STDC_WANT_IEC_60559_DFP_EXT__
# define __STDC_WANT_IEC_60559_DFP_EXT__ 1
#endif
/* Enable extensions specified by C23 Annex F.  */
#ifndef __STDC_WANT_IEC_60559_EXT__
# define __STDC_WANT_IEC_60559_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-4:2015.  */
#ifndef __STDC_WANT_IEC_60559_FUNCS_EXT__
# define __STDC_WANT_IEC_60559_FUNCS_EXT__ 1
#endif
/* Enable extensions specified by C23 Annex H and ISO/IEC TS 18661-3:2015.  */
#ifndef __STDC_WANT_IEC_60559_TYPES_EXT__
# define __STDC_WANT_IEC_60559_TYPES_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TR 24731-2:2010.  */
#ifndef __STDC_WANT_LIB_EXT2__
# define __STDC_WANT_LIB_EXT2__ 1
#endif
/* Enable extensions specified by ISO/IEC 24747:2009.  */
#ifndef __STDC_WANT_MATH_SPEC_FUNCS__
# define __STDC_WANT_MATH_SPEC_FUNCS__ 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable X/Open extensions.  Define to 500 only if necessary
   to make mbstate_t available.  */
#ifndef _XOPEN_SOURCE
/* # undef _XOPEN_SOURCE */
#endif


/* Version number of package */
#define VERSION "4.4.1"

/* Build for the WINDOWS32 API. */
/* #undef WINDOWS32 */

/* Define if using the dmalloc debugging malloc package */
/* #undef WITH_DMALLOC */

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* True if the compiler says it groks GNU C version MAJOR.MINOR.  */
#if defined __GNUC__ && defined __GNUC_MINOR__
# define _GL_GNUC_PREREQ(major, minor) \
    ((major) < __GNUC__ + ((minor) <= __GNUC_MINOR__))
#else
# define _GL_GNUC_PREREQ(major, minor) 0
#endif


/* Define to enable the declarations of ISO C 11 types and functions. */
/* #undef _ISOC11_SOURCE */

/* Define to 1 on platforms where this makes off_t a 64-bit type. */
/* #undef _LARGE_FILES */

/* The _Noreturn keyword of C11.  */
#ifndef _Noreturn
# if (defined __cplusplus \
      && ((201103 <= __cplusplus && !(__GNUC__ == 4 && __GNUC_MINOR__ == 7)) \
          || (defined _MSC_VER && 1900 <= _MSC_VER)) \
      && 0)
    /* [[noreturn]] is not practically usable, because with it the syntax
         extern _Noreturn void func (...);
       would not be valid; such a declaration would only be valid with 'extern'
       and '_Noreturn' swapped, or without the 'extern' keyword.  However, some
       AIX system header files and several gnulib header files use precisely
       this syntax with 'extern'.  */
#  define _Noreturn [[noreturn]]
# elif (defined __clang__ && __clang_major__ < 16 \
        && defined _GL_WORK_AROUND_LLVM_BUG_59792)
   /* Compile with -D_GL_WORK_AROUND_LLVM_BUG_59792 to work around
      that rare LLVM bug, though you may get many false-alarm warnings.  */
#  define _Noreturn
# elif ((!defined __cplusplus || defined __clang__) \
        && (201112 <= (defined __STDC_VERSION__ ? __STDC_VERSION__ : 0) \
            || (!defined __STRICT_ANSI__ \
                && (_GL_GNUC_PREREQ (4, 7) \
                    || (defined __apple_build_version__ \
                        ? 6000000 <= __apple_build_version__ \
                        : 3 < __clang_major__ + (5 <= __clang_minor__))))))
   /* _Noreturn works as-is.  */
# elif _GL_GNUC_PREREQ (2, 8) || defined __clang__ || 0x5110 <= __SUNPRO_C
#  define _Noreturn __attribute__ ((__noreturn__))
# elif 1200 <= (defined _MSC_VER ? _MSC_VER : 0)
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif


/* Number of bits in time_t, on hosts where this is settable. */
/* #undef _TIME_BITS */

/* Define to 1 on platforms where this makes time_t a 64-bit type. */
/* #undef __MINGW_USE_VC2005_COMPAT */

/* The _GL_ASYNC_SAFE marker should be attached to functions that are
   signal handlers (for signals other than SIGABRT, SIGPIPE) or can be
   invoked from such signal handlers.  Such functions have some restrictions:
     * All functions that it calls should be marked _GL_ASYNC_SAFE as well,
       or should be listed as async-signal-safe in POSIX
       <https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_04>
       section 2.4.3.  Note that malloc(), sprintf(), and fwrite(), in
       particular, are NOT async-signal-safe.
     * All memory locations (variables and struct fields) that these functions
       access must be marked 'volatile'.  This holds for both read and write
       accesses.  Otherwise the compiler might optimize away stores to and
       reads from such locations that occur in the program, depending on its
       data flow analysis.  For example, when the program contains a loop
       that is intended to inspect a variable set from within a signal handler
           while (!signal_occurred)
             ;
       the compiler is allowed to transform this into an endless loop if the
       variable 'signal_occurred' is not declared 'volatile'.
   Additionally, recall that:
     * A signal handler should not modify errno (except if it is a handler
       for a fatal signal and ends by raising the same signal again, thus
       provoking the termination of the process).  If it invokes a function
       that may clobber errno, it needs to save and restore the value of
       errno.  */
#define _GL_ASYNC_SAFE


/* Attributes.  */
#if (defined __has_attribute \
     && (!defined __clang_minor__ \
         || (defined __apple_build_version__ \
             ? 6000000 <= __apple_build_version__ \
             : 3 < __clang_major__ + (5 <= __clang_minor__))))
# define _GL_HAS_ATTRIBUTE(attr) __has_attribute (__##attr##__)
#else
# define _GL_HAS_ATTRIBUTE(attr) _GL_ATTR_##attr
# define _GL_ATTR_alloc_size _GL_GNUC_PREREQ (4, 3)
# define _GL_ATTR_always_inline _GL_GNUC_PREREQ (3, 2)
# define _GL_ATTR_artificial _GL_GNUC_PREREQ (4, 3)
# define _GL_ATTR_cold _GL_GNUC_PREREQ (4, 3)
# define _GL_ATTR_const _GL_GNUC_PREREQ (2, 95)
# define _GL_ATTR_deprecated _GL_GNUC_PREREQ (3, 1)
# define _GL_ATTR_diagnose_if 0
# define _GL_ATTR_error _GL_GNUC_PREREQ (4, 3)
# define _GL_ATTR_externally_visible _GL_GNUC_PREREQ (4, 1)
# define _GL_ATTR_fallthrough _GL_GNUC_PREREQ (7, 0)
# define _GL_ATTR_format _GL_GNUC_PREREQ (2, 7)
# define _GL_ATTR_leaf _GL_GNUC_PREREQ (4, 6)
# define _GL_ATTR_malloc _GL_GNUC_PREREQ (3, 0)
# ifdef _ICC
#  define _GL_ATTR_may_alias 0
# else
#  define _GL_ATTR_may_alias _GL_GNUC_PREREQ (3, 3)
# endif
# define _GL_ATTR_noinline _GL_GNUC_PREREQ (3, 1)
# define _GL_ATTR_nonnull _GL_GNUC_PREREQ (3, 3)
# define _GL_ATTR_nonstring _GL_GNUC_PREREQ (8, 0)
# define _GL_ATTR_nothrow _GL_GNUC_PREREQ (3, 3)
# define _GL_ATTR_packed _GL_GNUC_PREREQ (2, 7)
# define _GL_ATTR_pure _GL_GNUC_PREREQ (2, 96)
# define _GL_ATTR_returns_nonnull _GL_GNUC_PREREQ (4, 9)
# define _GL_ATTR_sentinel _GL_GNUC_PREREQ (4, 0)
# define _GL_ATTR_unused _GL_GNUC_PREREQ (2, 7)
# define _GL_ATTR_warn_unused_result _GL_GNUC_PREREQ (3, 4)
#endif

/* Disable GCC -Wpedantic if using __has_c_attribute and this is not C23+.  */
#if (defined __has_c_attribute && _GL_GNUC_PREREQ (4, 6) \
     && (defined __STDC_VERSION__ ? __STDC_VERSION__ : 0) <= 201710)
# pragma GCC diagnostic ignored "-Wpedantic"
#endif


/* _GL_ATTRIBUTE_ALLOC_SIZE ((N)) declares that the Nth argument of the function
   is the size of the returned memory block.
   _GL_ATTRIBUTE_ALLOC_SIZE ((M, N)) declares that the Mth argument multiplied
   by the Nth argument of the function is the size of the returned memory block.
 */
/* Applies to: function, pointer to function, function types.  */
#ifndef _GL_ATTRIBUTE_ALLOC_SIZE
# if _GL_HAS_ATTRIBUTE (alloc_size)
#  define _GL_ATTRIBUTE_ALLOC_SIZE(args) __attribute__ ((__alloc_size__ args))
# else
#  define _GL_ATTRIBUTE_ALLOC_SIZE(args)
# endif
#endif

/* _GL_ATTRIBUTE_ALWAYS_INLINE tells that the compiler should always inline the
   function and report an error if it cannot do so.  */
/* Applies to: function.  */
#ifndef _GL_ATTRIBUTE_ALWAYS_INLINE
# if _GL_HAS_ATTRIBUTE (always_inline)
#  define _GL_ATTRIBUTE_ALWAYS_INLINE __attribute__ ((__always_inline__))
# else
#  define _GL_ATTRIBUTE_ALWAYS_INLINE
# endif
#endif

/* _GL_ATTRIBUTE_ARTIFICIAL declares that the function is not important to show
    in stack traces when debugging.  The compiler should omit the function from
    stack traces.  */
/* Applies to: function.  */
#ifndef _GL_ATTRIBUTE_ARTIFICIAL
# if _GL_HAS_ATTRIBUTE (artificial)
#  define _GL_ATTRIBUTE_ARTIFICIAL __attribute__ ((__artificial__))
# else
#  define _GL_ATTRIBUTE_ARTIFICIAL
# endif
#endif

/* _GL_ATTRIBUTE_COLD declares that the function is rarely executed.  */
/* Applies to: functions.  */
/* Avoid __attribute__ ((cold)) on MinGW; see thread starting at
   <https://lists.gnu.org/r/emacs-devel/2019-04/msg01152.html>.
   Also, Oracle Studio 12.6 requires 'cold' not '__cold__'.  */
#ifndef _GL_ATTRIBUTE_COLD
# if _GL_HAS_ATTRIBUTE (cold) && !defined __MINGW32__
#  ifndef __SUNPRO_C
#   define _GL_ATTRIBUTE_COLD __attribute__ ((__cold__))
#  else
#   define _GL_ATTRIBUTE_COLD __attribute__ ((cold))
#  endif
# else
#  define _GL_ATTRIBUTE_COLD
# endif
#endif

/* _GL_ATTRIBUTE_CONST declares that it is OK for a compiler to omit duplicate
   calls to the function with the same arguments.
   This attribute is safe for a function that neither depends on nor affects
   observable state, and always returns exactly once - e.g., does not loop
   forever, and does not call longjmp.
   (This attribute is stricter than _GL_ATTRIBUTE_PURE.)  */
/* Applies to: functions.  */
#ifndef _GL_ATTRIBUTE_CONST
# if _GL_HAS_ATTRIBUTE (const)
#  define _GL_ATTRIBUTE_CONST __attribute__ ((__const__))
# else
#  define _GL_ATTRIBUTE_CONST
# endif
#endif

/* _GL_ATTRIBUTE_DEALLOC (F, I) declares that the function returns pointers
   that can be freed by passing them as the Ith argument to the
   function F.
   _GL_ATTRIBUTE_DEALLOC_FREE declares that the function returns pointers that
   can be freed via 'free'; it can be used only after declaring 'free'.  */
/* Applies to: functions.  Cannot be used on inline functions.  */
#ifndef _GL_ATTRIBUTE_DEALLOC
# if _GL_GNUC_PREREQ (11, 0)
#  define _GL_ATTRIBUTE_DEALLOC(f, i) __attribute__ ((__malloc__ (f, i)))
# else
#  define _GL_ATTRIBUTE_DEALLOC(f, i)
# endif
#endif
/* If gnulib's <string.h> or <wchar.h> has already defined this macro, continue
   to use this earlier definition, since <stdlib.h> may not have been included
   yet.  */
#ifndef _GL_ATTRIBUTE_DEALLOC_FREE
# if defined __cplusplus && defined __GNUC__ && !defined __clang__
/* Work around GCC bug <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=108231> */
#  define _GL_ATTRIBUTE_DEALLOC_FREE \
     _GL_ATTRIBUTE_DEALLOC ((void (*) (void *)) free, 1)
# else
#  define _GL_ATTRIBUTE_DEALLOC_FREE \
     _GL_ATTRIBUTE_DEALLOC (free, 1)
# endif
#endif

/* _GL_ATTRIBUTE_DEPRECATED: Declares that an entity is deprecated.
   The compiler may warn if the entity is used.  */
/* Applies to:
     - function, variable,
     - struct, union, struct/union member,
     - enumeration, enumeration item,
     - typedef,
   in C++ also: namespace, class, template specialization.  */
#ifndef _GL_ATTRIBUTE_DEPRECATED
# ifdef __has_c_attribute
#  if __has_c_attribute (__deprecated__)
#   define _GL_ATTRIBUTE_DEPRECATED [[__deprecated__]]
#  endif
# endif
# if !defined _GL_ATTRIBUTE_DEPRECATED && _GL_HAS_ATTRIBUTE (deprecated)
#  define _GL_ATTRIBUTE_DEPRECATED __attribute__ ((__deprecated__))
# endif
# ifndef _GL_ATTRIBUTE_DEPRECATED
#  define _GL_ATTRIBUTE_DEPRECATED
# endif
#endif

/* _GL_ATTRIBUTE_ERROR(msg) requests an error if a function is called and
   the function call is not optimized away.
   _GL_ATTRIBUTE_WARNING(msg) requests a warning if a function is called and
   the function call is not optimized away.  */
/* Applies to: functions.  */
#if !(defined _GL_ATTRIBUTE_ERROR && defined _GL_ATTRIBUTE_WARNING)
# if _GL_HAS_ATTRIBUTE (error)
#  define _GL_ATTRIBUTE_ERROR(msg) __attribute__ ((__error__ (msg)))
#  define _GL_ATTRIBUTE_WARNING(msg) __attribute__ ((__warning__ (msg)))
# elif _GL_HAS_ATTRIBUTE (diagnose_if)
#  define _GL_ATTRIBUTE_ERROR(msg) __attribute__ ((__diagnose_if__ (1, msg, "error")))
#  define _GL_ATTRIBUTE_WARNING(msg) __attribute__ ((__diagnose_if__ (1, msg, "warning")))
# else
#  define _GL_ATTRIBUTE_ERROR(msg)
#  define _GL_ATTRIBUTE_WARNING(msg)
# endif
#endif

/* _GL_ATTRIBUTE_EXTERNALLY_VISIBLE declares that the entity should remain
   visible to debuggers etc., even with '-fwhole-program'.  */
/* Applies to: functions, variables.  */
#ifndef _GL_ATTRIBUTE_EXTERNALLY_VISIBLE
# if _GL_HAS_ATTRIBUTE (externally_visible)
#  define _GL_ATTRIBUTE_EXTERNALLY_VISIBLE __attribute__ ((externally_visible))
# else
#  define _GL_ATTRIBUTE_EXTERNALLY_VISIBLE
# endif
#endif

/* _GL_ATTRIBUTE_FALLTHROUGH declares that it is not a programming mistake if
   the control flow falls through to the immediately following 'case' or
   'default' label.  The compiler should not warn in this case.  */
/* Applies to: Empty statement (;), inside a 'switch' statement.  */
/* Always expands to something.  */
#ifndef _GL_ATTRIBUTE_FALLTHROUGH
# ifdef __has_c_attribute
#  if __has_c_attribute (__fallthrough__)
#   define _GL_ATTRIBUTE_FALLTHROUGH [[__fallthrough__]]
#  endif
# endif
# if !defined _GL_ATTRIBUTE_FALLTHROUGH && _GL_HAS_ATTRIBUTE (fallthrough)
#  define _GL_ATTRIBUTE_FALLTHROUGH __attribute__ ((__fallthrough__))
# endif
# ifndef _GL_ATTRIBUTE_FALLTHROUGH
#  define _GL_ATTRIBUTE_FALLTHROUGH ((void) 0)
# endif
#endif

/* _GL_ATTRIBUTE_FORMAT ((ARCHETYPE, STRING-INDEX, FIRST-TO-CHECK))
   declares that the STRING-INDEXth function argument is a format string of
   style ARCHETYPE, which is one of:
     printf, gnu_printf
     scanf, gnu_scanf,
     strftime, gnu_strftime,
     strfmon,
   or the same thing prefixed and suffixed with '__'.
   If FIRST-TO-CHECK is not 0, arguments starting at FIRST-TO_CHECK
   are suitable for the format string.  */
/* Applies to: functions.  */
#ifndef _GL_ATTRIBUTE_FORMAT
# if _GL_HAS_ATTRIBUTE (format)
#  define _GL_ATTRIBUTE_FORMAT(spec) __attribute__ ((__format__ spec))
# else
#  define _GL_ATTRIBUTE_FORMAT(spec)
# endif
#endif

/* _GL_ATTRIBUTE_LEAF declares that if the function is called from some other
   compilation unit, it executes code from that unit only by return or by
   exception handling.  This declaration lets the compiler optimize that unit
   more aggressively.  */
/* Applies to: functions.  */
#ifndef _GL_ATTRIBUTE_LEAF
# if _GL_HAS_ATTRIBUTE (leaf)
#  define _GL_ATTRIBUTE_LEAF __attribute__ ((__leaf__))
# else
#  define _GL_ATTRIBUTE_LEAF
# endif
#endif

/* _GL_ATTRIBUTE_MALLOC declares that the function returns a pointer to freshly
   allocated memory.  */
/* Applies to: functions.  */
#ifndef _GL_ATTRIBUTE_MALLOC
# if _GL_HAS_ATTRIBUTE (malloc)
#  define _GL_ATTRIBUTE_MALLOC __attribute__ ((__malloc__))
# else
#  define _GL_ATTRIBUTE_MALLOC
# endif
#endif

/* _GL_ATTRIBUTE_MAY_ALIAS declares that pointers to the type may point to the
   same storage as pointers to other types.  Thus this declaration disables
   strict aliasing optimization.  */
/* Applies to: types.  */
/* Oracle Studio 12.6 mishandles may_alias despite __has_attribute OK.  */
#ifndef _GL_ATTRIBUTE_MAY_ALIAS
# if _GL_HAS_ATTRIBUTE (may_alias) && !defined __SUNPRO_C
#  define _GL_ATTRIBUTE_MAY_ALIAS __attribute__ ((__may_alias__))
# else
#  define _GL_ATTRIBUTE_MAY_ALIAS
# endif
#endif

/* _GL_ATTRIBUTE_MAYBE_UNUSED declares that it is not a programming mistake if
   the entity is not used.  The compiler should not warn if the entity is not
   used.  */
/* Applies to:
     - function, variable,
     - struct, union, struct/union member,
     - enumeration, enumeration item,
     - typedef,
   in C++ also: class.  */
/* In C++ and C23, this is spelled [[__maybe_unused__]].
   GCC's syntax is __attribute__ ((__unused__)).
   clang supports both syntaxes.  Except that with clang ≥ 6, < 10, in C++ mode,
   __has_c_attribute (__maybe_unused__) yields true but the use of
   [[__maybe_unused__]] nevertheless produces a warning.  */
#ifndef _GL_ATTRIBUTE_MAYBE_UNUSED
# if defined __clang__ && defined __cplusplus
#  if !defined __apple_build_version__ && __clang_major__ >= 10
#   define _GL_ATTRIBUTE_MAYBE_UNUSED [[__maybe_unused__]]
#  endif
# elif defined __has_c_attribute
#  if __has_c_attribute (__maybe_unused__)
#   define _GL_ATTRIBUTE_MAYBE_UNUSED [[__maybe_unused__]]
#  endif
# endif
# ifndef _GL_ATTRIBUTE_MAYBE_UNUSED
#  define _GL_ATTRIBUTE_MAYBE_UNUSED _GL_ATTRIBUTE_UNUSED
# endif
#endif
/* Alternative spelling of this macro, for convenience and for
   compatibility with glibc/include/libc-symbols.h.  */
#define _GL_UNUSED _GL_ATTRIBUTE_MAYBE_UNUSED
/* Earlier spellings of this macro.  */
#define _UNUSED_PARAMETER_ _GL_ATTRIBUTE_MAYBE_UNUSED

/* _GL_ATTRIBUTE_NODISCARD declares that the caller of the function should not
   discard the return value.  The compiler may warn if the caller does not use
   the return value, unless the caller uses something like ignore_value.  */
/* Applies to: function, enumeration, class.  */
#ifndef _GL_ATTRIBUTE_NODISCARD
# if defined __clang__ && defined __cplusplus
  /* With clang up to 15.0.6 (at least), in C++ mode, [[__nodiscard__]] produces
     a warning.
     The 1000 below means a yet unknown threshold.  When clang++ version X
     starts supporting [[__nodiscard__]] without warning about it, you can
     replace the 1000 with X.  */
#  if __clang_major__ >= 1000
#   define _GL_ATTRIBUTE_NODISCARD [[__nodiscard__]]
#  endif
# elif defined __has_c_attribute
#  if __has_c_attribute (__nodiscard__)
#   define _GL_ATTRIBUTE_NODISCARD [[__nodiscard__]]
#  endif
# endif
# if !defined _GL_ATTRIBUTE_NODISCARD && _GL_HAS_ATTRIBUTE (warn_unused_result)
#  define _GL_ATTRIBUTE_NODISCARD __attribute__ ((__warn_unused_result__))
# endif
# ifndef _GL_ATTRIBUTE_NODISCARD
#  define _GL_ATTRIBUTE_NODISCARD
# endif
#endif

/* _GL_ATTRIBUTE_NOINLINE tells that the compiler should not inline the
   function.  */
/* Applies to: functions.  */
#ifndef _GL_ATTRIBUTE_NOINLINE
# if _GL_HAS_ATTRIBUTE (noinline)
#  define _GL_ATTRIBUTE_NOINLINE __attribute__ ((__noinline__))
# else
#  define _GL_ATTRIBUTE_NOINLINE
# endif
#endif

/* _GL_ATTRIBUTE_NONNULL ((N1, N2,...)) declares that the arguments N1, N2,...
   must not be NULL.
   _GL_ATTRIBUTE_NONNULL () declares that all pointer arguments must not be
   null.  */
/* Applies to: functions.  */
#ifndef _GL_ATTRIBUTE_NONNULL
# if _GL_HAS_ATTRIBUTE (nonnull)
#  define _GL_ATTRIBUTE_NONNULL(args) __attribute__ ((__nonnull__ args))
# else
#  define _GL_ATTRIBUTE_NONNULL(args)
# endif
#endif

/* _GL_ATTRIBUTE_NONSTRING declares that the contents of a character array is
   not meant to be NUL-terminated.  */
/* Applies to: struct/union members and variables that are arrays of element
   type '[[un]signed] char'.  */
#ifndef _GL_ATTRIBUTE_NONSTRING
# if _GL_HAS_ATTRIBUTE (nonstring)
#  define _GL_ATTRIBUTE_NONSTRING __attribute__ ((__nonstring__))
# else
#  define _GL_ATTRIBUTE_NONSTRING
# endif
#endif

/* There is no _GL_ATTRIBUTE_NORETURN; use _Noreturn instead.  */

/* _GL_ATTRIBUTE_NOTHROW declares that the function does not throw exceptions.
 */
/* Applies to: functions.  */
#ifndef _GL_ATTRIBUTE_NOTHROW
# if _GL_HAS_ATTRIBUTE (nothrow) && !defined __cplusplus
#  define _GL_ATTRIBUTE_NOTHROW __attribute__ ((__nothrow__))
# else
#  define _GL_ATTRIBUTE_NOTHROW
# endif
#endif

/* _GL_ATTRIBUTE_PACKED declares:
   For struct members: The member has the smallest possible alignment.
   For struct, union, class: All members have the smallest possible alignment,
   minimizing the memory required.  */
/* Applies to: struct members, struct, union,
   in C++ also: class.  */
#ifndef _GL_ATTRIBUTE_PACKED
# if _GL_HAS_ATTRIBUTE (packed)
#  define _GL_ATTRIBUTE_PACKED __attribute__ ((__packed__))
# else
#  define _GL_ATTRIBUTE_PACKED
# endif
#endif

/* _GL_ATTRIBUTE_PURE declares that It is OK for a compiler to omit duplicate
   calls to the function with the same arguments if observable state is not
   changed between calls.
   This attribute is safe for a function that does not affect
   observable state, and always returns exactly once.
   (This attribute is looser than _GL_ATTRIBUTE_CONST.)  */
/* Applies to: functions.  */
#ifndef _GL_ATTRIBUTE_PURE
# if _GL_HAS_ATTRIBUTE (pure)
#  define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define _GL_ATTRIBUTE_PURE
# endif
#endif

/* _GL_ATTRIBUTE_RETURNS_NONNULL declares that the function's return value is
   a non-NULL pointer.  */
/* Applies to: functions.  */
#ifndef _GL_ATTRIBUTE_RETURNS_NONNULL
# if _GL_HAS_ATTRIBUTE (returns_nonnull)
#  define _GL_ATTRIBUTE_RETURNS_NONNULL __attribute__ ((__returns_nonnull__))
# else
#  define _GL_ATTRIBUTE_RETURNS_NONNULL
# endif
#endif

/* _GL_ATTRIBUTE_SENTINEL(pos) declares that the variadic function expects a
   trailing NULL argument.
   _GL_ATTRIBUTE_SENTINEL () - The last argument is NULL (requires C99).
   _GL_ATTRIBUTE_SENTINEL ((N)) - The (N+1)st argument from the end is NULL.  */
/* Applies to: functions.  */
#ifndef _GL_ATTRIBUTE_SENTINEL
# if _GL_HAS_ATTRIBUTE (sentinel)
#  define _GL_ATTRIBUTE_SENTINEL(pos) __attribute__ ((__sentinel__ pos))
# else
#  define _GL_ATTRIBUTE_SENTINEL(pos)
# endif
#endif

/* A helper macro.  Don't use it directly.  */
#ifndef _GL_ATTRIBUTE_UNUSED
# if _GL_HAS_ATTRIBUTE (unused)
#  define _GL_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define _GL_ATTRIBUTE_UNUSED
# endif
#endif


/* _GL_UNUSED_LABEL; declares that it is not a programming mistake if the
   immediately preceding label is not used.  The compiler should not warn
   if the label is not used.  */
/* Applies to: label (both in C and C++).  */
/* Note that g++ < 4.5 does not support the '__attribute__ ((__unused__)) ;'
   syntax.  But clang does.  */
#ifndef _GL_UNUSED_LABEL
# if !(defined __cplusplus && !_GL_GNUC_PREREQ (4, 5)) || defined __clang__
#  define _GL_UNUSED_LABEL _GL_ATTRIBUTE_UNUSED
# else
#  define _GL_UNUSED_LABEL
# endif
#endif


/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define as 'access' if you don't have the eaccess() function. */
/* #undef eaccess */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define to the widest signed integer type if <stdint.h> and <inttypes.h> do
   not define. */
/* #undef intmax_t */

/* Work around a bug in Apple GCC 4.0.1 build 5465: In C99 mode, it supports
   the ISO C 99 semantics of 'extern inline' (unlike the GNU C semantics of
   earlier versions), but does not display it by setting __GNUC_STDC_INLINE__.
   __APPLE__ && __MACH__ test for Mac OS X.
   __APPLE_CC__ tests for the Apple compiler and its version.
   __STDC_VERSION__ tests for the C99 mode.  */
#if defined __APPLE__ && defined __MACH__ && __APPLE_CC__ >= 5465 && !defined __cplusplus && __STDC_VERSION__ >= 199901L && !defined __GNUC_STDC_INLINE__
# define __GNUC_STDC_INLINE__ 1
#endif

/* _GL_CMP (n1, n2) performs a three-valued comparison on n1 vs. n2, where
   n1 and n2 are expressions without side effects, that evaluate to real
   numbers (excluding NaN).
   It returns
     1  if n1 > n2
     0  if n1 == n2
     -1 if n1 < n2
   The naïve code   (n1 > n2 ? 1 : n1 < n2 ? -1 : 0)  produces a conditional
   jump with nearly all GCC versions up to GCC 10.
   This variant     (n1 < n2 ? -1 : n1 > n2)  produces a conditional with many
   GCC versions up to GCC 9.
   The better code  (n1 > n2) - (n1 < n2)  from Hacker's Delight § 2-9
   avoids conditional jumps in all GCC versions >= 3.4.  */
#define _GL_CMP(n1, n2) (((n1) > (n2)) - ((n1) < (n2)))


/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define as a signed integer type capable of holding a process identifier. */
/* #undef pid_t */

/* Define as an integer type suitable for memory locations that can be
   accessed atomically even in the presence of asynchronous signals. */
/* #undef sig_atomic_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */

/* Define to the widest unsigned integer type if <stdint.h> and <inttypes.h>
   do not define. */
/* #undef uintmax_t */

/* Define as `fork' if `vfork' does not work. */
/* #undef vfork */

#ifndef HAVE_C_BOOL
# if !defined __cplusplus && !defined __bool_true_false_are_defined
#  if HAVE_STDBOOL_H
#   include <stdbool.h>
#  else
#   if defined __SUNPRO_C
#    error "<stdbool.h> is not usable with this configuration. To make it usable, add -D_STDC_C99= to $CC."
#   else
#    error "<stdbool.h> does not exist on this platform. Use gnulib module 'stdbool-c99' instead of gnulib module 'stdbool'."
#   endif
#  endif
# endif
# if !true
#  define true (!false)
# endif
#endif
