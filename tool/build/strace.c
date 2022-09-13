/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/user_regs_struct.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/ptrace.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/w.h"

/**
 * @fileoverview ptrace() tutorial
 */

#define PROLOGUE "%r%8d %'18T "

#undef __NR_execve
#define __WALL 0x40000000

#define PTR     0
#define ULONG   0
#define INT     1
#define LONG    2
#define STR     3
#define BUF     4
#define IOV     5
#define STRLIST 6
#define INTPTR  7
#define STAT    8
#define SIG     9
#define SIGSET  10
#define PIPE    11
#define OCTAL   0x80

static const long __NR_brk = 12;
static const long __NR_sigreturn = 15;

static const struct Syscall {
  long *number;
  const char *name;
  char arity;
  char eager;
  unsigned char ret;
  unsigned char arg[6];
} kSyscalls[] = {
    // clang-format off
    {&__NR_exit,                           "exit",                           1, 1, INT, {INT}},
    {&__NR_exit_group,                     "exit_group",                     1, 1, INT, {INT}},
    {&__NR_read,                           "read",                           3, 1, LONG, {INT, BUF, ULONG}},
    {&__NR_write,                          "write",                          3, 3, LONG, {INT, BUF, ULONG}},
    {&__NR_open,                           "open",                           3, 3, INT, {STR, INT, OCTAL|INT}},
    {&__NR_close,                          "close",                          1, 1, INT, {INT}},
    {&__NR_brk,                            "brk",                            1, 1, ULONG, {ULONG}},
    {&__NR_stat,                           "stat",                           2, 1, INT, {STR, STAT}},
    {&__NR_fstat,                          "fstat",                          2, 1, INT, {INT, STAT}},
    {&__NR_lstat,                          "lstat",                          2, 1, INT, {INT, STAT}},
    {&__NR_poll,                           "poll",                           3, 3, INT, {PTR, INT, INT}},
    {&__NR_ppoll,                          "ppoll",                          4, 4, INT},
    {&__NR_lseek,                          "lseek",                          3, 3, LONG, {INT, LONG, INT}},
    {&__NR_mmap,                           "mmap",                           6, 6, ULONG, {PTR, ULONG, INT, INT, INT, ULONG}},
    {&__NR_msync,                          "msync",                          3, 3, INT, {PTR, ULONG, INT}},
    {&__NR_mprotect,                       "mprotect",                       3, 3, INT, {PTR, ULONG, INT}},
    {&__NR_munmap,                         "munmap",                         2, 2, INT, {PTR, ULONG}},
    {&__NR_sigreturn,                      "rt_sigreturn",                   6, 6, LONG},
    {&__NR_sigaction,                      "rt_sigaction",                   4, 4, INT, {SIG}},
    {&__NR_sigprocmask,                    "rt_sigprocmask",                 4, 4, INT, {INT, SIGSET, SIGSET, LONG}},
    {&__NR_sigpending,                     "rt_sigpending",                  2, 2, INT, {SIGSET, LONG}},
    {&__NR_sigsuspend,                     "rt_sigsuspend",                  2, 2, INT, {SIGSET, LONG}},
    {&__NR_rt_sigqueueinfo,                "rt_sigqueueinfo",                6, 6},
    {&__NR_ioctl,                          "ioctl",                          3, 3, INT, {INT, ULONG, ULONG}},
    {&__NR_pread,                          "pread64",                        4, 1, LONG, {INT, BUF, ULONG, ULONG}},
    {&__NR_pwrite,                         "pwrite64",                       4, 4, LONG, {INT, BUF, ULONG, ULONG}},
    {&__NR_readv,                          "readv",                          3, 1, LONG, {INT, IOV, INT}},
    {&__NR_writev,                         "writev",                         3, 3, LONG, {INT, IOV, INT}},
    {&__NR_access,                         "access",                         2, 2, INT, {STR, OCTAL|INT}},
    {&__NR_pipe,                           "pipe",                           1, 0, INT, {PIPE}},
    {&__NR_pipe2,                          "pipe2",                          2, 0, INT, {PIPE, INT}},
    {&__NR_select,                         "select",                         5, 5},
    {&__NR_pselect,                        "pselect",                        6, 6},
    {&__NR_pselect6,                       "pselect6",                       6, 6},
    {&__NR_sched_yield,                    "sched_yield",                    0, 0, INT},
    {&__NR_mremap,                         "mremap",                         5, 5},
    {&__NR_mincore,                        "mincore",                        6, 6},
    {&__NR_madvise,                        "madvise",                        6, 6},
    {&__NR_shmget,                         "shmget",                         6, 6},
    {&__NR_shmat,                          "shmat",                          6, 6},
    {&__NR_shmctl,                         "shmctl",                         6, 6},
    {&__NR_dup,                            "dup",                            1, 1, INT, {INT}},
    {&__NR_dup2,                           "dup2",                           2, 2, INT, {INT, INT}},
    {&__NR_pause,                          "pause",                          0, 0, INT},
    {&__NR_nanosleep,                      "nanosleep",                      2, 1},
    {&__NR_getitimer,                      "getitimer",                      2, 2},
    {&__NR_setitimer,                      "setitimer",                      3, 3},
    {&__NR_alarm,                          "alarm",                          1, 1},
    {&__NR_getpid,                         "getpid",                         0, 0, INT},
    {&__NR_sendfile,                       "sendfile",                       6, 6},
    {&__NR_socket,                         "socket",                         3, 3, INT, {INT, INT, INT}},
    {&__NR_connect,                        "connect",                        3, 3},
    {&__NR_accept,                         "accept",                         3, 3},
    {&__NR_sendto,                         "sendto",                         6, 6},
    {&__NR_recvfrom,                       "recvfrom",                       6, 6},
    {&__NR_sendmsg,                        "sendmsg",                        6, 6},
    {&__NR_recvmsg,                        "recvmsg",                        6, 6},
    {&__NR_shutdown,                       "shutdown",                       6, 6},
    {&__NR_bind,                           "bind",                           6, 6},
    {&__NR_listen,                         "listen",                         6, 6},
    {&__NR_getsockname,                    "getsockname",                    6, 6},
    {&__NR_getpeername,                    "getpeername",                    6, 6},
    {&__NR_socketpair,                     "socketpair",                     6, 6},
    {&__NR_setsockopt,                     "setsockopt",                     6, 6},
    {&__NR_getsockopt,                     "getsockopt",                     6, 6},
    {&__NR_fork,                           "fork",                           0, 0, INT},
    {&__NR_vfork,                          "vfork",                          0, 0, INT},
    {&__NR_posix_spawn,                    "posix_spawn",                    6, 6},
    {&__NR_execve,                         "execve",                         3, 3, INT, {STR, STRLIST, STRLIST}},
    {&__NR_wait4,                          "wait4",                          4, 4, INT, {INT, INTPTR, INT, PTR}},
    {&__NR_kill,                           "kill",                           2, 2, INT, {INT, SIG}},
    {&__NR_killpg,                         "killpg",                         2, 2, INT, {INT, SIG}},
    {&__NR_clone,                          "clone",                          5, 5, INT, {PTR, PTR, INTPTR, INTPTR, ULONG}},
    {&__NR_tkill,                          "tkill",                          2, 2, INT, {INT, SIG}},
    {&__NR_futex,                          "futex",                          6, 6},
    {&__NR_set_robust_list,                "set_robust_list",                6, 6},
    {&__NR_get_robust_list,                "get_robust_list",                6, 6},
    {&__NR_uname,                          "uname",                          6, 6},
    {&__NR_semget,                         "semget",                         6, 6},
    {&__NR_semop,                          "semop",                          6, 6},
    {&__NR_semctl,                         "semctl",                         6, 6},
    {&__NR_shmdt,                          "shmdt",                          6, 6},
    {&__NR_msgget,                         "msgget",                         6, 6},
    {&__NR_msgsnd,                         "msgsnd",                         6, 6},
    {&__NR_msgrcv,                         "msgrcv",                         6, 6},
    {&__NR_msgctl,                         "msgctl",                         6, 6},
    {&__NR_fcntl,                          "fcntl",                          3, 3, INT, {INT, INT, ULONG}},
    {&__NR_flock,                          "flock",                          6, 6},
    {&__NR_fsync,                          "fsync",                          6, 6},
    {&__NR_fdatasync,                      "fdatasync",                      6, 6},
    {&__NR_truncate,                       "truncate",                       2, 2, INT, {STR, ULONG}},
    {&__NR_ftruncate,                      "ftruncate",                      6, 6, INT, {INT, ULONG}},
    {&__NR_getcwd,                         "getcwd",                         2, 2, INT, {BUF, ULONG}},
    {&__NR_chdir,                          "chdir",                          1, 1, INT, {STR}},
    {&__NR_fchdir,                         "fchdir",                         1, 1, INT, {INT}},
    {&__NR_rename,                         "rename",                         2, 2, INT, {STR, STR}},
    {&__NR_mkdir,                          "mkdir",                          2, 2, INT, {STR, OCTAL|INT}},
    {&__NR_rmdir,                          "rmdir",                          1, 1, INT, {STR}},
    {&__NR_creat,                          "creat",                          2, 2, INT, {STR, OCTAL|INT}},
    {&__NR_link,                           "link",                           2, 2, INT, {STR, STR}},
    {&__NR_unlink,                         "unlink",                         1, 1, INT, {STR}},
    {&__NR_symlink,                        "symlink",                        6, 6},
    {&__NR_readlink,                       "readlink",                       3, 1, INT, {STR, BUF, ULONG}},
    {&__NR_chmod,                          "chmod",                          6, 6},
    {&__NR_fchmod,                         "fchmod",                         6, 6},
    {&__NR_chown,                          "chown",                          6, 6},
    {&__NR_fchown,                         "fchown",                         6, 6},
    {&__NR_lchown,                         "lchown",                         6, 6},
    {&__NR_umask,                          "umask",                          1, 1, OCTAL|INT, {OCTAL|INT}},
    {&__NR_gettimeofday,                   "gettimeofday",                   6, 6},
    {&__NR_getrlimit,                      "getrlimit",                      6, 6},
    {&__NR_getrusage,                      "getrusage",                      6, 6},
    {&__NR_sysinfo,                        "sysinfo",                        6, 6},
    {&__NR_times,                          "times",                          6, 6},
    {&__NR_ptrace,                         "ptrace",                         6, 6},
    {&__NR_syslog,                         "syslog",                         6, 6},
    {&__NR_getuid,                         "getuid",                         0, 0, INT},
    {&__NR_getgid,                         "getgid",                         0, 0, INT},
    {&__NR_getppid,                        "getppid",                        0, 0, INT},
    {&__NR_getpgrp,                        "getpgrp",                        0, 0, INT},
    {&__NR_setsid,                         "setsid",                         1, 1, INT, {INT}},
    {&__NR_getsid,                         "getsid",                         0, 0, INT},
    {&__NR_getpgid,                        "getpgid",                        0, 0, INT},
    {&__NR_setpgid,                        "setpgid",                        2, 2, INT, {INT, INT}},
    {&__NR_geteuid,                        "geteuid",                        0, 0, INT},
    {&__NR_getegid,                        "getegid",                        0, 0, INT},
    {&__NR_getgroups,                      "getgroups",                      6, 6},
    {&__NR_setgroups,                      "setgroups",                      6, 6},
    {&__NR_setreuid,                       "setreuid",                       6, 6},
    {&__NR_setregid,                       "setregid",                       6, 6},
    {&__NR_setuid,                         "setuid",                         1, 1, INT, {INT}},
    {&__NR_setgid,                         "setgid",                         1, 1, INT, {INT}},
    {&__NR_setresuid,                      "setresuid",                      6, 6},
    {&__NR_setresgid,                      "setresgid",                      6, 6},
    {&__NR_getresuid,                      "getresuid",                      6, 6},
    {&__NR_getresgid,                      "getresgid",                      6, 6},
    {&__NR_sigaltstack,                    "sigaltstack",                    6, 6},
    {&__NR_mknod,                          "mknod",                          6, 6},
    {&__NR_mknodat,                        "mknodat",                        6, 6},
    {&__NR_mkfifo,                         "mkfifo",                         6, 6},
    {&__NR_mkfifoat,                       "mkfifoat",                       6, 6},
    {&__NR_statfs,                         "statfs",                         6, 6},
    {&__NR_fstatfs,                        "fstatfs",                        6, 6},
    {&__NR_getpriority,                    "getpriority",                    6, 6},
    {&__NR_setpriority,                    "setpriority",                    6, 6},
    {&__NR_mlock,                          "mlock",                          6, 6},
    {&__NR_munlock,                        "munlock",                        6, 6},
    {&__NR_mlockall,                       "mlockall",                       6, 6},
    {&__NR_munlockall,                     "munlockall",                     6, 6},
    {&__NR_setrlimit,                      "setrlimit",                      6, 6},
    {&__NR_chroot,                         "chroot",                         6, 6},
    {&__NR_sync,                           "sync",                           6, 6},
    {&__NR_acct,                           "acct",                           6, 6},
    {&__NR_settimeofday,                   "settimeofday",                   6, 6},
    {&__NR_mount,                          "mount",                          6, 6},
    {&__NR_reboot,                         "reboot",                         6, 6},
    {&__NR_quotactl,                       "quotactl",                       6, 6},
    {&__NR_setfsuid,                       "setfsuid",                       6, 6},
    {&__NR_setfsgid,                       "setfsgid",                       6, 6},
    {&__NR_capget,                         "capget",                         6, 6},
    {&__NR_capset,                         "capset",                         6, 6},
    {&__NR_sigtimedwait,                   "sigtimedwait",                   6, 6},
    {&__NR_personality,                    "personality",                    6, 6},
    {&__NR_ustat,                          "ustat",                          6, 6},
    {&__NR_sysfs,                          "sysfs",                          6, 6},
    {&__NR_sched_setparam,                 "sched_setparam",                 6, 6},
    {&__NR_sched_getparam,                 "sched_getparam",                 6, 6},
    {&__NR_sched_setscheduler,             "sched_setscheduler",             6, 6},
    {&__NR_sched_getscheduler,             "sched_getscheduler",             6, 6},
    {&__NR_sched_get_priority_max,         "sched_get_priority_max",         6, 6},
    {&__NR_sched_get_priority_min,         "sched_get_priority_min",         6, 6},
    {&__NR_sched_rr_get_interval,          "sched_rr_get_interval",          6, 6},
    {&__NR_vhangup,                        "vhangup",                        6, 6},
    {&__NR_modify_ldt,                     "modify_ldt",                     6, 6},
    {&__NR_pivot_root,                     "pivot_root",                     6, 6},
    {&__NR__sysctl,                        "_sysctl",                        6, 6},
    {&__NR_prctl,                          "prctl",                          6, 6},
    {&__NR_arch_prctl,                     "arch_prctl",                     2, 2, INT, {INT, ULONG}},
    {&__NR_adjtimex,                       "adjtimex",                       6, 6},
    {&__NR_umount2,                        "umount2",                        6, 6},
    {&__NR_swapon,                         "swapon",                         6, 6},
    {&__NR_swapoff,                        "swapoff",                        6, 6},
    {&__NR_sethostname,                    "sethostname",                    6, 6},
    {&__NR_setdomainname,                  "setdomainname",                  6, 6},
    {&__NR_iopl,                           "iopl",                           6, 6},
    {&__NR_ioperm,                         "ioperm",                         6, 6},
    {&__NR_init_module,                    "init_module",                    6, 6},
    {&__NR_delete_module,                  "delete_module",                  6, 6},
    {&__NR_gettid,                         "gettid",                         6, 6},
    {&__NR_readahead,                      "readahead",                      6, 6},
    {&__NR_setxattr,                       "setxattr",                       6, 6},
    {&__NR_fsetxattr,                      "fsetxattr",                      6, 6},
    {&__NR_getxattr,                       "getxattr",                       6, 6},
    {&__NR_fgetxattr,                      "fgetxattr",                      6, 6},
    {&__NR_listxattr,                      "listxattr",                      6, 6},
    {&__NR_flistxattr,                     "flistxattr",                     6, 6},
    {&__NR_removexattr,                    "removexattr",                    6, 6},
    {&__NR_fremovexattr,                   "fremovexattr",                   6, 6},
    {&__NR_lsetxattr,                      "lsetxattr",                      6, 6},
    {&__NR_lgetxattr,                      "lgetxattr",                      6, 6},
    {&__NR_llistxattr,                     "llistxattr",                     6, 6},
    {&__NR_lremovexattr,                   "lremovexattr",                   6, 6},
    {&__NR_sched_setaffinity,              "sched_setaffinity",              6, 6},
    {&__NR_sched_getaffinity,              "sched_getaffinity",              6, 6},
    {&__NR_cpuset_getaffinity,             "cpuset_getaffinity",             6, 6},
    {&__NR_cpuset_setaffinity,             "cpuset_setaffinity",             6, 6},
    {&__NR_io_setup,                       "io_setup",                       6, 6},
    {&__NR_io_destroy,                     "io_destroy",                     6, 6},
    {&__NR_io_getevents,                   "io_getevents",                   6, 6},
    {&__NR_io_submit,                      "io_submit",                      6, 6},
    {&__NR_io_cancel,                      "io_cancel",                      6, 6},
    {&__NR_lookup_dcookie,                 "lookup_dcookie",                 6, 6},
    {&__NR_epoll_create,                   "epoll_create",                   6, 6},
    {&__NR_epoll_wait,                     "epoll_wait",                     6, 6},
    {&__NR_epoll_ctl,                      "epoll_ctl",                      6, 6},
    {&__NR_getdents,                       "getdents64",                     6, 6},
    {&__NR_set_tid_address,                "set_tid_address",                1, 1},
    {&__NR_restart_syscall,                "restart_syscall",                6, 6},
    {&__NR_semtimedop,                     "semtimedop",                     6, 6},
    {&__NR_fadvise,                        "fadvise",                        6, 6},
    {&__NR_timer_create,                   "timer_create",                   6, 6},
    {&__NR_timer_settime,                  "timer_settime",                  6, 6},
    {&__NR_timer_gettime,                  "timer_gettime",                  6, 6},
    {&__NR_timer_getoverrun,               "timer_getoverrun",               6, 6},
    {&__NR_timer_delete,                   "timer_delete",                   6, 6},
    {&__NR_clock_settime,                  "clock_settime",                  6, 6},
    {&__NR_clock_gettime,                  "clock_gettime",                  6, 6},
    {&__NR_clock_getres,                   "clock_getres",                   6, 6},
    {&__NR_clock_nanosleep,                "clock_nanosleep",                6, 6},
    {&__NR_tgkill,                         "tgkill",                         6, 6},
    {&__NR_mbind,                          "mbind",                          6, 6},
    {&__NR_set_mempolicy,                  "set_mempolicy",                  6, 6},
    {&__NR_get_mempolicy,                  "get_mempolicy",                  6, 6},
    {&__NR_mq_open,                        "mq_open",                        6, 6},
    {&__NR_mq_unlink,                      "mq_unlink",                      6, 6},
    {&__NR_mq_timedsend,                   "mq_timedsend",                   6, 6},
    {&__NR_mq_timedreceive,                "mq_timedreceive",                6, 6},
    {&__NR_mq_notify,                      "mq_notify",                      6, 6},
    {&__NR_mq_getsetattr,                  "mq_getsetattr",                  6, 6},
    {&__NR_kexec_load,                     "kexec_load",                     6, 6},
    {&__NR_waitid,                         "waitid",                         6, 6},
    {&__NR_add_key,                        "add_key",                        6, 6},
    {&__NR_request_key,                    "request_key",                    6, 6},
    {&__NR_keyctl,                         "keyctl",                         6, 6},
    {&__NR_ioprio_set,                     "ioprio_set",                     6, 6},
    {&__NR_ioprio_get,                     "ioprio_get",                     6, 6},
    {&__NR_inotify_init,                   "inotify_init",                   6, 6},
    {&__NR_inotify_add_watch,              "inotify_add_watch",              6, 6},
    {&__NR_inotify_rm_watch,               "inotify_rm_watch",               6, 6},
    {&__NR_openat,                         "openat",                         4, 4, INT, {INT, STR, INT, OCTAL|INT}},
    {&__NR_mkdirat,                        "mkdirat",                        3, 3, INT, {INT, STR, OCTAL|INT}},
    {&__NR_fchownat,                       "fchownat",                       6, 6},
    {&__NR_utime,                          "utime",                          6, 6},
    {&__NR_utimes,                         "utimes",                         6, 6},
    {&__NR_futimesat,                      "futimesat",                      6, 6},
    {&__NR_futimes,                        "futimes",                        6, 6},
    {&__NR_futimens,                       "futimens",                       6, 6},
    {&__NR_fstatat,                        "newfstatat",                     4, 2, INT, {INT, STR, STAT, INT}},
    {&__NR_unlinkat,                       "unlinkat",                       3, 3, INT, {INT, STR, INT}},
    {&__NR_renameat,                       "renameat",                       4, 4, INT, {INT, STR, INT, STR}},
    {&__NR_linkat,                         "linkat",                         6, 6},
    {&__NR_symlinkat,                      "symlinkat",                      6, 6},
    {&__NR_readlinkat,                     "readlinkat",                     6, 6},
    {&__NR_fchmodat,                       "fchmodat",                       6, 6},
    {&__NR_faccessat,                      "faccessat",                      4, 4, INT, {INT, STR, INT, INT}},
    {&__NR_unshare,                        "unshare",                        6, 6},
    {&__NR_splice,                         "splice",                         6, 6},
    {&__NR_tee,                            "tee",                            6, 6},
    {&__NR_sync_file_range,                "sync_file_range",                4, 4},
    {&__NR_vmsplice,                       "vmsplice",                       6, 6},
    {&__NR_migrate_pages,                  "migrate_pages",                  6, 6},
    {&__NR_move_pages,                     "move_pages",                     6, 6},
    {&__NR_preadv,                         "preadv",                         4, 1, LONG, {INT, IOV, ULONG, ULONG}},
    {&__NR_pwritev,                        "pwritev",                        6, 6, LONG, {INT, IOV, ULONG, ULONG}},
    {&__NR_utimensat,                      "utimensat",                      6, 6},
    {&__NR_fallocate,                      "fallocate",                      6, 6},
    {&__NR_posix_fallocate,                "posix_fallocate",                6, 6},
    {&__NR_accept4,                        "accept4",                        4, 4},
    {&__NR_dup3,                           "dup3",                           3, 3, INT},
    {&__NR_epoll_pwait,                    "epoll_pwait",                    6, 6},
    {&__NR_epoll_create1,                  "epoll_create1",                  6, 6},
    {&__NR_perf_event_open,                "perf_event_open",                6, 6},
    {&__NR_inotify_init1,                  "inotify_init1",                  6, 6},
    {&__NR_rt_tgsigqueueinfo,              "rt_tgsigqueueinfo",              6, 6},
    {&__NR_signalfd,                       "signalfd",                       6, 6},
    {&__NR_signalfd4,                      "signalfd4",                      6, 6},
    {&__NR_eventfd,                        "eventfd",                        6, 6},
    {&__NR_eventfd2,                       "eventfd2",                       6, 6},
    {&__NR_timerfd_create,                 "timerfd_create",                 6, 6},
    {&__NR_timerfd_settime,                "timerfd_settime",                6, 6},
    {&__NR_timerfd_gettime,                "timerfd_gettime",                6, 6},
    {&__NR_recvmmsg,                       "recvmmsg",                       6, 6},
    {&__NR_fanotify_init,                  "fanotify_init",                  6, 6},
    {&__NR_fanotify_mark,                  "fanotify_mark",                  6, 6},
    {&__NR_prlimit,                        "prlimit",                        6, 6},
    {&__NR_name_to_handle_at,              "name_to_handle_at",              6, 6},
    {&__NR_open_by_handle_at,              "open_by_handle_at",              6, 6},
    {&__NR_clock_adjtime,                  "clock_adjtime",                  6, 6},
    {&__NR_syncfs,                         "syncfs",                         6, 6},
    {&__NR_sendmmsg,                       "sendmmsg",                       6, 6},
    {&__NR_setns,                          "setns",                          6, 6},
    {&__NR_getcpu,                         "getcpu",                         6, 6},
    {&__NR_process_vm_readv,               "process_vm_readv",               6, 6},
    {&__NR_process_vm_writev,              "process_vm_writev",              6, 6},
    {&__NR_kcmp,                           "kcmp",                           6, 6},
    {&__NR_finit_module,                   "finit_module",                   6, 6},
    {&__NR_sched_setattr,                  "sched_setattr",                  6, 6},
    {&__NR_sched_getattr,                  "sched_getattr",                  6, 6},
    {&__NR_renameat2,                      "renameat2",                      6, 6},
    {&__NR_seccomp,                        "seccomp",                        6, 6},
    {&__NR_getrandom,                      "getrandom",                      6, 6},
    {&__NR_memfd_create,                   "memfd_create",                   6, 6},
    {&__NR_kexec_file_load,                "kexec_file_load",                6, 6},
    {&__NR_bpf,                            "bpf",                            6, 6},
    {&__NR_execveat,                       "execveat",                       6, 6},
    {&__NR_userfaultfd,                    "userfaultfd",                    6, 6},
    {&__NR_membarrier,                     "membarrier",                     6, 6},
    {&__NR_mlock2,                         "mlock2",                         6, 6},
    {&__NR_copy_file_range,                "copy_file_range",                6, 6},
    {&__NR_preadv2,                        "preadv2",                        6, 6},
    {&__NR_pwritev2,                       "pwritev2",                       6, 6},
    {&__NR_pkey_mprotect,                  "pkey_mprotect",                  6, 6},
    {&__NR_pkey_alloc,                     "pkey_alloc",                     6, 6},
    {&__NR_pkey_free,                      "pkey_free",                      6, 6},
    {&__NR_statx,                          "statx",                          6, 6},
    {&__NR_io_pgetevents,                  "io_pgetevents",                  6, 6},
    {&__NR_rseq,                           "rseq",                           6, 6},
    {&__NR_pidfd_send_signal,              "pidfd_send_signal",              6, 6},
    {&__NR_io_uring_setup,                 "io_uring_setup",                 6, 6},
    {&__NR_io_uring_enter,                 "io_uring_enter",                 6, 6},
    {&__NR_io_uring_register,              "io_uring_register",              6, 6},
    // clang-format on
};

static const struct Errno {
  errno_t *number;
  const char *name;
} kErrnos[] = {
    {&ENOSYS, "ENOSYS"},                    //
    {&EPERM, "EPERM"},                      //
    {&ENOENT, "ENOENT"},                    //
    {&ESRCH, "ESRCH"},                      //
    {&EINTR, "EINTR"},                      //
    {&EIO, "EIO"},                          //
    {&ENXIO, "ENXIO"},                      //
    {&E2BIG, "E2BIG"},                      //
    {&ENOEXEC, "ENOEXEC"},                  //
    {&EBADF, "EBADF"},                      //
    {&ECHILD, "ECHILD"},                    //
    {&EAGAIN, "EAGAIN"},                    //
    {&ENOMEM, "ENOMEM"},                    //
    {&EACCES, "EACCES"},                    //
    {&EFAULT, "EFAULT"},                    //
    {&ENOTBLK, "ENOTBLK"},                  //
    {&EBUSY, "EBUSY"},                      //
    {&EEXIST, "EEXIST"},                    //
    {&EXDEV, "EXDEV"},                      //
    {&ENODEV, "ENODEV"},                    //
    {&ENOTDIR, "ENOTDIR"},                  //
    {&EISDIR, "EISDIR"},                    //
    {&EINVAL, "EINVAL"},                    //
    {&ENFILE, "ENFILE"},                    //
    {&EMFILE, "EMFILE"},                    //
    {&ENOTTY, "ENOTTY"},                    //
    {&ETXTBSY, "ETXTBSY"},                  //
    {&EFBIG, "EFBIG"},                      //
    {&ENOSPC, "ENOSPC"},                    //
    {&EDQUOT, "EDQUOT"},                    //
    {&ESPIPE, "ESPIPE"},                    //
    {&EROFS, "EROFS"},                      //
    {&EMLINK, "EMLINK"},                    //
    {&EPIPE, "EPIPE"},                      //
    {&EDOM, "EDOM"},                        //
    {&ERANGE, "ERANGE"},                    //
    {&EDEADLK, "EDEADLK"},                  //
    {&ENAMETOOLONG, "ENAMETOOLONG"},        //
    {&ENOLCK, "ENOLCK"},                    //
    {&ENOTEMPTY, "ENOTEMPTY"},              //
    {&ELOOP, "ELOOP"},                      //
    {&ENOMSG, "ENOMSG"},                    //
    {&EIDRM, "EIDRM"},                      //
    {&ETIME, "ETIME"},                      //
    {&EPROTO, "EPROTO"},                    //
    {&EOVERFLOW, "EOVERFLOW"},              //
    {&EILSEQ, "EILSEQ"},                    //
    {&EUSERS, "EUSERS"},                    //
    {&ENOTSOCK, "ENOTSOCK"},                //
    {&EDESTADDRREQ, "EDESTADDRREQ"},        //
    {&EMSGSIZE, "EMSGSIZE"},                //
    {&EPROTOTYPE, "EPROTOTYPE"},            //
    {&ENOPROTOOPT, "ENOPROTOOPT"},          //
    {&EPROTONOSUPPORT, "EPROTONOSUPPORT"},  //
    {&ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT"},  //
    {&ENOTSUP, "ENOTSUP"},                  //
    {&EOPNOTSUPP, "EOPNOTSUPP"},            //
    {&EPFNOSUPPORT, "EPFNOSUPPORT"},        //
    {&EAFNOSUPPORT, "EAFNOSUPPORT"},        //
    {&EADDRINUSE, "EADDRINUSE"},            //
    {&EADDRNOTAVAIL, "EADDRNOTAVAIL"},      //
    {&ENETDOWN, "ENETDOWN"},                //
    {&ENETUNREACH, "ENETUNREACH"},          //
    {&ENETRESET, "ENETRESET"},              //
    {&ECONNABORTED, "ECONNABORTED"},        //
    {&ECONNRESET, "ECONNRESET"},            //
    {&ENOBUFS, "ENOBUFS"},                  //
    {&EISCONN, "EISCONN"},                  //
    {&ENOTCONN, "ENOTCONN"},                //
    {&ESHUTDOWN, "ESHUTDOWN"},              //
    {&ETOOMANYREFS, "ETOOMANYREFS"},        //
    {&ETIMEDOUT, "ETIMEDOUT"},              //
    {&ECONNREFUSED, "ECONNREFUSED"},        //
    {&EHOSTDOWN, "EHOSTDOWN"},              //
    {&EHOSTUNREACH, "EHOSTUNREACH"},        //
    {&EALREADY, "EALREADY"},                //
    {&EINPROGRESS, "EINPROGRESS"},          //
    {&ESTALE, "ESTALE"},                    //
    {&EREMOTE, "EREMOTE"},                  //
    {&EBADRPC, "EBADRPC"},                  //
    {&ERPCMISMATCH, "ERPCMISMATCH"},        //
    {&EPROGUNAVAIL, "EPROGUNAVAIL"},        //
    {&EPROGMISMATCH, "EPROGMISMATCH"},      //
    {&EPROCUNAVAIL, "EPROCUNAVAIL"},        //
    {&EFTYPE, "EFTYPE"},                    //
    {&EAUTH, "EAUTH"},                      //
    {&ENEEDAUTH, "ENEEDAUTH"},              //
    {&EPROCLIM, "EPROCLIM"},                //
    {&ENOATTR, "ENOATTR"},                  //
    {&EPWROFF, "EPWROFF"},                  //
    {&EDEVERR, "EDEVERR"},                  //
    {&EBADEXEC, "EBADEXEC"},                //
    {&EBADARCH, "EBADARCH"},                //
    {&ESHLIBVERS, "ESHLIBVERS"},            //
    {&EBADMACHO, "EBADMACHO"},              //
    {&ENOPOLICY, "ENOPOLICY"},              //
    {&EBADMSG, "EBADMSG"},                  //
    {&ECANCELED, "ECANCELED"},              //
    {&EOWNERDEAD, "EOWNERDEAD"},            //
    {&ENOTRECOVERABLE, "ENOTRECOVERABLE"},  //
    {&ENONET, "ENONET"},                    //
    {&ERESTART, "ERESTART"},                //
    {&ENOSR, "ENOSR"},                      //
    {&ENOSTR, "ENOSTR"},                    //
    {&ENODATA, "ENODATA"},                  //
    {&EMULTIHOP, "EMULTIHOP"},              //
    {&ENOLINK, "ENOLINK"},                  //
    {&ENOMEDIUM, "ENOMEDIUM"},              //
    {&EMEDIUMTYPE, "EMEDIUMTYPE"},          //
    {&EWOULDBLOCK, "EWOULDBLOCK"},          //
};

struct Pid {
  int pid;
  uint32_t hash;
  bool insyscall;
  struct Syscall *call;
  struct Syscall fakecall;
  struct user_regs_struct args;
  char fakename[12];
};

struct PidList {
  uint32_t i, n;
  struct Pid *p;
};

char *ob;        // output buffer
struct Pid *sp;  // active subprocess

static uint32_t Hash(uint64_t pid) {
  uint32_t hash;
  hash = (pid * 6364136223846793005 + 1442695040888963407) >> 32;
  if (!hash) hash = 1;
  return hash;
}

static struct Pid *GetPid(struct PidList *list, int pid) {
  uint32_t i, hash, step;
  DCHECK_NE(0, pid);
  DCHECK_LE(list->i, list->n >> 1);
  if (list->n) {
    i = 0;
    step = 0;
    hash = Hash(pid);
    do {
      i = (hash + step * (step + 1) / 2) & (list->n - 1);
      if (list->p[i].pid == pid) {
        return list->p + i;
      }
      step++;
    } while (list->p[i].hash);
  }
  return 0;
}

static void ReservePid(struct PidList *list, int count) {
  size_t i, j, step;
  struct PidList old;
  DCHECK_LE(list->i, list->n >> 1);
  while (list->i + count >= (list->n >> 1)) {
    memcpy(&old, list, sizeof(*list));
    list->n = list->n ? list->n << 1 : 16;
    list->p = calloc(list->n, sizeof(*list->p));
    for (i = 0; i < old.n; ++i) {
      if (!old.p[i].hash) continue;
      step = 0;
      do {
        j = (old.p[i].hash + step * (step + 1) / 2) & (list->n - 1);
        step++;
      } while (list->p[j].hash);
      list->p[j] = old.p[i];
    }
    free(old.p);
  }
}

static struct Pid *AddPid(struct PidList *list, int pid) {
  struct Pid *item;
  uint32_t i, hash, step;
  DCHECK_NE(0, pid);
  DCHECK_LE(list->i, list->n >> 1);
  if (!(item = GetPid(list, pid))) {
    ReservePid(list, 1);
    hash = Hash(pid);
    ++list->i;
    i = 0;
    step = 0;
    do {
      i = (hash + step * (step + 1) / 2) & (list->n - 1);
      step++;
    } while (list->p[i].hash);
    list->p[i].hash = hash;
    list->p[i].pid = pid;
    item = list->p + i;
  }
  return item;
}

static void RemovePid(struct PidList *list, int pid) {
  uint32_t i, hash, step;
  DCHECK_NE(0, pid);
  DCHECK_LE(list->i, list->n >> 1);
  if (list->n) {
    i = 0;
    step = 0;
    hash = Hash(pid);
    do {
      i = (hash + step * (step + 1) / 2) & (list->n - 1);
      if (list->p[i].pid == pid) {
        bzero(list->p + i, sizeof(*list->p));
        list->i--;
        assert(list->i < 99999);
        return;
      }
      step++;
    } while (list->p[i].hash);
  }
}

static ssize_t WriteAll(int fd, const char *p, size_t n) {
  ssize_t rc;
  size_t i, got;
  for (i = 0; i < n;) {
    rc = write(fd, p + i, n - i);
    if (rc != -1) {
      got = rc;
      i += got;
    } else if (errno != EINTR) {
      return -1;
    }
  }
  return i;
}

static void Flush(void) {
  WriteAll(2, ob, appendz(ob).i);
  appendr(&ob, 0);
}

static const char *GetErrnoName(int x) {
  const char *s;
  static char buf[16];
  if ((s = _strerrno(x))) return s;
  FormatInt64(buf, x);
  return buf;
}

static struct Syscall *GetSyscall(int x) {
  int i;
  if (x >= 0) {
    for (i = 0; i < ARRAYLEN(kSyscalls); ++i) {
      if (x == *kSyscalls[i].number) {
        return kSyscalls + i;
      }
    }
  }
  return NULL;
}

static char *PeekString(unsigned long x) {
  union {
    char buf[8];
    long word;
  } u;
  char *p;
  unsigned offset;
  unsigned long addr, i, n;
  if (!x) return NULL;
  addr = ROUNDDOWN(x, 8);
  offset = x - addr;
  errno = 0;
  u.word = ptrace(PTRACE_PEEKTEXT, sp->pid, addr);
  if (errno) return 0;
  n = strnlen(u.buf + offset, 8 - offset);
  p = calloc(1, n);
  memcpy(p, u.buf + offset, n);
  if (n == 8 - offset) {
    do {
      addr += 8;
      errno = 0;
      u.word = ptrace(PTRACE_PEEKDATA, sp->pid, addr);
      if (errno) break;
      i = strnlen(u.buf, 8);
      p = realloc(p, n + i);
      memcpy(p + n, u.buf, i);
      n += i;
    } while (i == 8);
  }
  p = realloc(p, n + 1);
  p[n] = 0;
  return p;
}

static char *PrintString(char *s) {
  kappendf(&ob, "%#s", s);
  return s;
}

static void *PeekData(unsigned long x, size_t size) {
  union {
    char buf[8];
    long word;
  } u;
  char *p;
  unsigned offset;
  unsigned long addr, i, n;
  if (!x) return NULL;
  addr = ROUNDDOWN(x, 8);
  offset = x - addr;
  errno = 0;
  u.word = ptrace(PTRACE_PEEKTEXT, sp->pid, addr);
  if (errno) return 0;
  p = calloc(1, size);
  memcpy(p, u.buf + offset, MIN(size, 8 - offset));
  for (i = 8 - offset; i < size; i += MIN(8, size - i)) {
    addr += 8;
    errno = 0;
    u.word = ptrace(PTRACE_PEEKDATA, sp->pid, addr);
    if (errno) break;
    memcpy(p + i, u.buf, MIN(8, size - i));
  }
  return p;
}

static char *PrintData(char *data, size_t size) {
  kappendf(&ob, "%#.*hhs%s", MIN(40, size), data, size > 40 ? "..." : "", data);
  return data;
}

static struct iovec *PeekIov(unsigned long addr, int len) {
  int i;
  char *p;
  long word;
  struct iovec *iov;
  if (!addr) return NULL;
  p = calloc(1, len * 16);
  for (i = 0; i < len * 2; ++i, addr += sizeof(word)) {
    errno = 0;
    word = ptrace(PTRACE_PEEKTEXT, sp->pid, addr);
    if (errno) break;
    memcpy(p + i * sizeof(word), &word, sizeof(word));
  }
  iov = (struct iovec *)p;
  for (i = 0; i < len; ++i) {
    errno = 0;
    iov[i].iov_base = PeekData((long)iov[i].iov_base, iov[i].iov_len);
    if (errno) break;
  }
  return iov;
}

static struct iovec *PrintIov(struct iovec *iov, int iovlen) {
  int i;
  if (iov) {
    appendw(&ob, '{');
    for (i = 0; i < iovlen; ++i) {
      if (i) appendw(&ob, READ16LE(", "));
      appendw(&ob, '{');
      PrintData(iov[i].iov_base, iov[i].iov_len);
      kappendf(&ob, ", %#lx}", iov[i].iov_len);
    }
    appendw(&ob, '}');
  } else {
    appendw(&ob, READ32LE("NULL"));
  }
  return iov;
}

static void FreeIov(struct iovec *iov, int iovlen) {
  int i;
  if (iov) {
    for (i = 0; i < iovlen; ++i) {
      free(iov[i].iov_base);
    }
    free(iov);
  }
}

static char **PeekStringList(unsigned long addr) {
  char *p;
  long word;
  size_t i, n;
  char **list;
  if (!addr) return NULL;
  for (p = NULL, n = 0;; ++n) {
    p = realloc(p, (n + 1) * sizeof(word));
    word = ptrace(PTRACE_PEEKTEXT, sp->pid, addr + n * sizeof(word));
    memcpy(p + n * sizeof(word), &word, sizeof(word));
    if (!word) break;
  }
  list = (char **)p;
  for (i = 0; i < n; ++i) {
    list[i] = PeekString((long)list[i]);
  }
  return list;
}

static char **PrintStringList(char **list) {
  int i;
  if (list) {
    appendw(&ob, '{');
    for (i = 0;; ++i) {
      if (i) appendw(&ob, READ16LE(", "));
      PrintString(list[i]);
      if (!list[i]) break;
    }
    appendw(&ob, '}');
  } else {
    appendw(&ob, READ32LE("NULL"));
  }
  return list;
}

static void FreeStringList(char **list) {
  int i;
  if (list) {
    for (i = 0; list[i]; ++i) {
      free(list[i]);
    }
    free(list);
  }
}

static void PrintPipe(unsigned long addr) {
  unsigned long word;
  word = ptrace(PTRACE_PEEKDATA, sp->pid, addr);
  kappendf(&ob, "[{%d, %d}]", (int)word, (int)(word >> 32));
}

static struct stat *PeekStat(unsigned long addr) {
  int i;
  char *p;
  long word;
  if (!addr) return NULL;
  p = calloc(1, sizeof(struct stat));
  for (i = 0; i < sizeof(struct stat); i += sizeof(word)) {
    errno = 0;
    word = ptrace(PTRACE_PEEKTEXT, sp->pid, addr + i);
    if (errno) break;
    memcpy(p + i, &word, sizeof(word));
  }
  return (struct stat *)p;
}

static struct stat *PrintStat(struct stat *st) {
  bool printed;
  printed = false;
  appendw(&ob, '{');
  if (st->st_size) {
    kappendf(&ob, ".st_size = %#lx", st->st_size);
    printed = true;
  }
  if (st->st_mode) {
    if (printed) appendw(&ob, READ16LE(", "));
    kappendf(&ob, ".st_mode = %#o", st->st_mode);
    printed = true;
  }
  appendw(&ob, '}');
  return st;
}

static void PrintSigset(unsigned long p) {
  kappendf(&ob, "{%#lx}", ptrace(PTRACE_PEEKTEXT, sp->pid, p));
}

static void PrintSyscallArg(int type, unsigned long x, unsigned long y) {
  char *s;
  switch (type & 31) {
    case ULONG:
      kappendf(&ob, "%#lx", x);
      break;
    case INT:
      if (type & OCTAL) {
        kappendf(&ob, "%#o", x);
      } else {
        kappendf(&ob, "%d", x);
      }
      break;
    case LONG:
      kappendf(&ob, "%ld", x);
      break;
    case STR:
      free(PrintString(PeekString(x)));
      break;
    case BUF:
      free(PrintData(PeekData(x, MIN(32, y)), MIN(32, y)));
      break;
    case IOV:
      FreeIov(PrintIov(PeekIov(x, y), y), y);
      break;
    case STAT:
      free(PrintStat(PeekStat(x)));
      break;
    case PIPE:
      PrintPipe(x);
      break;
    case SIG:
      appends(&ob, strsignal(x));
      break;
    case SIGSET:
      PrintSigset(x);
      break;
    case STRLIST:
      FreeStringList(PrintStringList(PeekStringList(x)));
      break;
    case INTPTR:
      if (x) {
        s = PeekData(x, 4);
        kappendf(&ob, "{%d}", READ32LE(s));
        free(s);
      }
      break;
    default:
      kappendf(&ob, "wut[%'ld]", x);
      break;
  }
}

static void PrintSyscall(bool issecond) {
  int a, b, eager, arity;
  bool gotsome, isresuming, isunfinished;
  gotsome = false;
  if (issecond) {
    if (!sp->call) {
      return;
    }
  } else {
    if (!(sp->call = GetSyscall(sp->args.orig_rax))) {
      ksnprintf(sp->fakename, sizeof(sp->fakename), "%d", sp->args.orig_rax);
      bzero(&sp->fakecall, sizeof(sp->fakecall));
      sp->call = &sp->fakecall;
      sp->call->name = sp->fakename;
      sp->call->arity = 6;
      sp->call->eager = 6;
    }
    if (sp->args.orig_rax == __NR_execve) {
      if (sp->args.rax == -ENOSYS) {
        sp->args.rax = 0;
      }
    } else if (sp->args.rax == -ENOSYS) {
      return;
    }
  }
  isresuming = false;
  isunfinished = false;
  if (!isunfinished) {
    a = 0;
    b = sp->call->arity;
  } else if (!isresuming) {
    a = 0;
    b = sp->call->eager;
  } else {
    a = sp->call->eager;
    b = sp->call->arity;
  }
  kappendf(&ob, PROLOGUE " %s%s(", sp->pid, isresuming ? "<resuming> " : "",
           sp->call->name);
  if (a <= 0 && 0 < b) {
    if (gotsome) appendw(&ob, READ16LE(", "));
    PrintSyscallArg(sp->call->arg[0], sp->args.rdi, sp->args.rsi);
    gotsome = true;
  }
  if (a <= 1 && 1 < b) {
    if (gotsome) appendw(&ob, READ16LE(", "));
    PrintSyscallArg(sp->call->arg[1], sp->args.rsi, sp->args.rdx);
    gotsome = true;
  }
  if (a <= 2 && 2 < b) {
    if (gotsome) appendw(&ob, READ16LE(", "));
    PrintSyscallArg(sp->call->arg[2], sp->args.rdx, sp->args.r10);
    gotsome = true;
  }
  if (a <= 3 && 3 < b) {
    if (gotsome) appendw(&ob, READ16LE(", "));
    PrintSyscallArg(sp->call->arg[3], sp->args.r10, sp->args.r8);
    gotsome = true;
  }
  if (a <= 4 && 4 < b) {
    if (gotsome) appendw(&ob, READ16LE(", "));
    PrintSyscallArg(sp->call->arg[4], sp->args.r8, sp->args.r9);
    gotsome = true;
  }
  if (a <= 5 && 5 < b) {
    if (gotsome) appendw(&ob, READ16LE(", "));
    PrintSyscallArg(sp->call->arg[5], sp->args.r9, 0);
    gotsome = true;
  }
  if (isunfinished) {
    appends(&ob, ") → <unfinished>");
  } else {
    appends(&ob, ") → ");
    if (sp->args.rax > (unsigned long)-4096) {
      kappendf(&ob, "-1 %s", GetErrnoName(-sp->args.rax));
    } else {
      PrintSyscallArg(sp->call->ret, sp->args.rax, 0);
    }
  }
  kappendf(&ob, "%n");
  Flush();
  sp->call = 0;
}

wontreturn void PropagateExit(int wstatus) {
  exit(WEXITSTATUS(wstatus));
}

wontreturn void PropagateTermination(int wstatus) {
  sigset_t mask;
  // if signal that terminated program was sent to our whole
  // process group, then that signal should be delivered and kill
  // this process the moment it's unblocked here. we only unblock
  // here because if the child process ignores the signal and does
  // exit(0) then we want to propagate that intent too.
  sigemptyset(&mask);
  sigaddset(&mask, WTERMSIG(wstatus));
  sigprocmask(SIG_UNBLOCK, &mask, 0);
  // otherwise we can propagate it by the exit code convention
  // commonly used with shell scripts
  exit(128 + WTERMSIG(wstatus));
}

wontreturn void StraceMain(int argc, char *argv[]) {
  unsigned long msg;
  struct siginfo si;
  struct Pid *s, *child;
  struct PidList pidlist;
  sigset_t mask, origmask;
  int i, sig, evpid, root, wstatus, signal;
  struct sigaction sigign, saveint, savequit;

  if (!IsLinux()) {
    kprintf("error: ptrace() is only supported on linux right now%n");
    exit(1);
  }

  if (IsModeDbg()) ShowCrashReports();

  if (argc < 2) {
    kprintf("Usage: %s PROGRAM [ARGS...]%n", argv[0]);
    exit(1);
  }

  pidlist.i = 0;
  pidlist.n = 0;
  pidlist.p = 0;

  sigign.sa_flags = 0;
  sigign.sa_handler = SIG_IGN;
  sigemptyset(&sigign.sa_mask);

  sigaction(SIGINT, &sigign, &saveint);
  sigaction(SIGQUIT, &sigign, &savequit);

  sigemptyset(&mask);
  /* sigaddset(&mask, SIGCHLD); */
  sigprocmask(SIG_BLOCK, &mask, &origmask);

  CHECK_NE(-1, (root = fork()));
  if (!root) {
    sigaction(SIGINT, &saveint, 0);
    sigaction(SIGQUIT, &savequit, 0);
    sigprocmask(SIG_SETMASK, &origmask, 0);
    ptrace(PTRACE_TRACEME);
    execvp(argv[1], argv + 1);
    _Exit(127);
  }
  sp = AddPid(&pidlist, root);

  // wait for ptrace(PTRACE_TRACEME) to be called
  CHECK_EQ(sp->pid, waitpid(sp->pid, &wstatus, 0));

  // configure linux process tracing
  CHECK_NE(-1, ptrace(PTRACE_SETOPTIONS, sp->pid, 0,
                      (PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK |
                       PTRACE_O_TRACECLONE | PTRACE_O_TRACEEXEC |
                       PTRACE_O_TRACEEXIT | PTRACE_O_TRACESYSGOOD)));

  // continue child process setting breakpoint at next system call
  CHECK_NE(-1, ptrace(PTRACE_SYSCALL, sp->pid, 0, 0));

  for (;;) {
    CHECK_NE(-1, (evpid = waitpid(-1, &wstatus, __WALL)));

    // prevent rehash in second addpid call
    ReservePid(&pidlist, 2);

    // we can get wait notifications before fork/vfork/etc. events
    sp = AddPid(&pidlist, evpid);

    // handle actual exit
    if (WIFEXITED(wstatus)) {
      kprintf(PROLOGUE " exited with %d%n", sp->pid, WEXITSTATUS(wstatus));
      RemovePid(&pidlist, sp->pid);
      sp = 0;
      // we exit when the last process being monitored exits
      if (!pidlist.i) {
        PropagateExit(wstatus);
      } else {
        continue;
      }
    }

    // handle actual kill
    if (WIFSIGNALED(wstatus)) {
      kprintf(PROLOGUE " exited with signal %G%n", sp->pid, WTERMSIG(wstatus));
      RemovePid(&pidlist, sp->pid);
      sp = 0;
      // we die when the last process being monitored dies
      if (!pidlist.i) {
        PropagateTermination(wstatus);
      } else {
        continue;
      }
    }

    // handle core dump
    if (WCOREDUMP(wstatus)) {
      kprintf(PROLOGUE " exited with core%n", sp->pid);
      RemovePid(&pidlist, sp->pid);
      sp = 0;
      // we die when the last process being monitored dies
      if (!pidlist.i) {
        PropagateTermination(wstatus);
      } else {
        continue;
      }
    }

    // handle trace events
    sig = 0;
    signal = (wstatus >> 8) & 0xffff;
    assert(WIFSTOPPED(wstatus));
    if (signal == SIGTRAP | PTRACE_EVENT_STOP) {
      CHECK_NE(-1, ptrace(PTRACE_GETSIGINFO, sp->pid, 0, &si));
      if (si.si_code == SIGTRAP || si.si_code == SIGTRAP | 0x80) {
        CHECK_NE(-1, ptrace(PTRACE_GETREGS, sp->pid, 0, &sp->args));
        PrintSyscall(sp->insyscall);
        sp->insyscall = !sp->insyscall;
        ptrace(PTRACE_SYSCALL, sp->pid, 0, 0);
      } else {
        sig = signal & 127;
        kappendf(&ob, PROLOGUE " got signal %G%n", sp->pid, sig);
        Flush();
        ptrace(PTRACE_SYSCALL, sp->pid, 0, sig);
      }
    } else if (signal == (SIGTRAP | (PTRACE_EVENT_EXIT << 8))) {
      CHECK_NE(-1, ptrace(PTRACE_GETEVENTMSG, sp->pid, 0, &msg));
      sig = WSTOPSIG(wstatus);
      ptrace(PTRACE_SYSCALL, sp->pid, 0, 0);
    } else if (signal == (SIGTRAP | (PTRACE_EVENT_EXEC << 8))) {
      CHECK_NE(-1, ptrace(PTRACE_GETEVENTMSG, sp->pid, 0, &msg));
      ptrace(PTRACE_SYSCALL, sp->pid, 0, 0);
    } else if (signal == (SIGTRAP | (PTRACE_EVENT_FORK << 8)) ||
               signal == (SIGTRAP | (PTRACE_EVENT_VFORK << 8)) ||
               signal == (SIGTRAP | (PTRACE_EVENT_CLONE << 8))) {
      CHECK_NE(-1, ptrace(PTRACE_GETEVENTMSG, evpid, 0, &msg));
      child = AddPid(&pidlist, msg);
      child->pid = msg;
      if (signal == (SIGTRAP | (PTRACE_EVENT_FORK << 8))) {
        kappendf(&ob, PROLOGUE " fork() → %d%n", sp->pid, child->pid);
      } else if (signal == (SIGTRAP | (PTRACE_EVENT_VFORK << 8))) {
        kappendf(&ob, PROLOGUE " vfork() → %d%n", sp->pid, child->pid);
      } else {
        kappendf(&ob, PROLOGUE " clone() → %d%n", sp->pid, child->pid);
      }
      Flush();
      ptrace(PTRACE_SYSCALL, child->pid, 0, 0);
      ptrace(PTRACE_SYSCALL, sp->pid, 0, 0);
    } else {
      kappendf(&ob, PROLOGUE " gottish signal %G%n", sp->pid, sig);
      Flush();
      ptrace(PTRACE_SYSCALL, sp->pid, 0, signal & 127);
    }
  }
}

int main(int argc, char *argv[]) {
  StraceMain(argc, argv);
}
