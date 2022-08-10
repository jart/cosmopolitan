#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_NR_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_NR_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long __NR_exit;
extern const long __NR_exit_group;
extern const long __NR_read;
extern const long __NR_write;
extern const long __NR_open;
extern const long __NR_close;
extern const long __NR_stat;
extern const long __NR_fstat;
extern const long __NR_lstat;
extern const long __NR_poll;
extern const long __NR_ppoll;
extern const long __NR_lseek;
extern const long __NR_mmap;
extern const long __NR_msync;
extern const long __NR_mprotect;
extern const long __NR_munmap;
extern const long __NR_sigaction;
extern const long __NR_sigprocmask;
extern const long __NR_ioctl;
extern const long __NR_pread;
extern const long __NR_pwrite;
extern const long __NR_readv;
extern const long __NR_writev;
extern const long __NR_access;
extern const long __NR_pipe;
extern const long __NR_select;
extern const long __NR_pselect;
extern const long __NR_pselect6;
extern const long __NR_sched_yield;
extern const long __NR_mremap;
extern const long __NR_mincore;
extern const long __NR_madvise;
extern const long __NR_shmget;
extern const long __NR_shmat;
extern const long __NR_shmctl;
extern const long __NR_dup;
extern const long __NR_dup2;
extern const long __NR_pause;
extern const long __NR_nanosleep;
extern const long __NR_getitimer;
extern const long __NR_setitimer;
extern const long __NR_alarm;
extern const long __NR_getpid;
extern const long __NR_sendfile;
extern const long __NR_socket;
extern const long __NR_connect;
extern const long __NR_accept;
extern const long __NR_sendto;
extern const long __NR_recvfrom;
extern const long __NR_sendmsg;
extern const long __NR_recvmsg;
extern const long __NR_shutdown;
extern const long __NR_bind;
extern const long __NR_listen;
extern const long __NR_getsockname;
extern const long __NR_getpeername;
extern const long __NR_socketpair;
extern const long __NR_setsockopt;
extern const long __NR_getsockopt;
extern const long __NR_fork;
extern const long __NR_vfork;
extern const long __NR_posix_spawn;
extern const long __NR_execve;
extern const long __NR_wait4;
extern const long __NR_kill;
extern const long __NR_killpg;
extern const long __NR_clone;
extern const long __NR_tkill;
extern const long __NR_futex;
extern const long __NR_set_robust_list;
extern const long __NR_get_robust_list;
extern const long __NR_uname;
extern const long __NR_semget;
extern const long __NR_semop;
extern const long __NR_semctl;
extern const long __NR_shmdt;
extern const long __NR_msgget;
extern const long __NR_msgsnd;
extern const long __NR_msgrcv;
extern const long __NR_msgctl;
extern const long __NR_fcntl;
extern const long __NR_flock;
extern const long __NR_fsync;
extern const long __NR_fdatasync;
extern const long __NR_truncate;
extern const long __NR_ftruncate;
extern const long __NR_getcwd;
extern const long __NR_chdir;
extern const long __NR_fchdir;
extern const long __NR_rename;
extern const long __NR_mkdir;
extern const long __NR_rmdir;
extern const long __NR_creat;
extern const long __NR_link;
extern const long __NR_unlink;
extern const long __NR_symlink;
extern const long __NR_readlink;
extern const long __NR_chmod;
extern const long __NR_fchmod;
extern const long __NR_chown;
extern const long __NR_fchown;
extern const long __NR_lchown;
extern const long __NR_umask;
extern const long __NR_gettimeofday;
extern const long __NR_getrlimit;
extern const long __NR_getrusage;
extern const long __NR_sysinfo;
extern const long __NR_times;
extern const long __NR_ptrace;
extern const long __NR_syslog;
extern const long __NR_getuid;
extern const long __NR_getgid;
extern const long __NR_getppid;
extern const long __NR_getpgrp;
extern const long __NR_setsid;
extern const long __NR_getsid;
extern const long __NR_getpgid;
extern const long __NR_setpgid;
extern const long __NR_geteuid;
extern const long __NR_getegid;
extern const long __NR_getgroups;
extern const long __NR_setgroups;
extern const long __NR_setreuid;
extern const long __NR_setregid;
extern const long __NR_setuid;
extern const long __NR_setgid;
extern const long __NR_setresuid;
extern const long __NR_setresgid;
extern const long __NR_getresuid;
extern const long __NR_getresgid;
extern const long __NR_sigpending;
extern const long __NR_sigsuspend;
extern const long __NR_sigaltstack;
extern const long __NR_mknod;
extern const long __NR_mknodat;
extern const long __NR_mkfifo;
extern const long __NR_mkfifoat;
extern const long __NR_statfs;
extern const long __NR_fstatfs;
extern const long __NR_getpriority;
extern const long __NR_setpriority;
extern const long __NR_mlock;
extern const long __NR_munlock;
extern const long __NR_mlockall;
extern const long __NR_munlockall;
extern const long __NR_setrlimit;
extern const long __NR_chroot;
extern const long __NR_sync;
extern const long __NR_acct;
extern const long __NR_settimeofday;
extern const long __NR_mount;
extern const long __NR_reboot;
extern const long __NR_quotactl;
extern const long __NR_setfsuid;
extern const long __NR_setfsgid;
extern const long __NR_capget;
extern const long __NR_capset;
extern const long __NR_sigtimedwait;
extern const long __NR_rt_sigqueueinfo;
extern const long __NR_personality;
extern const long __NR_ustat;
extern const long __NR_sysfs;
extern const long __NR_sched_setparam;
extern const long __NR_sched_getparam;
extern const long __NR_sched_setscheduler;
extern const long __NR_sched_getscheduler;
extern const long __NR_sched_get_priority_max;
extern const long __NR_sched_get_priority_min;
extern const long __NR_sched_rr_get_interval;
extern const long __NR_vhangup;
extern const long __NR_modify_ldt;
extern const long __NR_pivot_root;
extern const long __NR__sysctl;
extern const long __NR_prctl;
extern const long __NR_arch_prctl;
extern const long __NR_adjtimex;
extern const long __NR_umount2;
extern const long __NR_swapon;
extern const long __NR_swapoff;
extern const long __NR_sethostname;
extern const long __NR_setdomainname;
extern const long __NR_iopl;
extern const long __NR_ioperm;
extern const long __NR_init_module;
extern const long __NR_delete_module;
extern const long __NR_gettid;
extern const long __NR_readahead;
extern const long __NR_setxattr;
extern const long __NR_fsetxattr;
extern const long __NR_getxattr;
extern const long __NR_fgetxattr;
extern const long __NR_listxattr;
extern const long __NR_flistxattr;
extern const long __NR_removexattr;
extern const long __NR_fremovexattr;
extern const long __NR_lsetxattr;
extern const long __NR_lgetxattr;
extern const long __NR_llistxattr;
extern const long __NR_lremovexattr;
extern const long __NR_sched_setaffinity;
extern const long __NR_sched_getaffinity;
extern const long __NR_cpuset_getaffinity;
extern const long __NR_cpuset_setaffinity;
extern const long __NR_io_setup;
extern const long __NR_io_destroy;
extern const long __NR_io_getevents;
extern const long __NR_io_submit;
extern const long __NR_io_cancel;
extern const long __NR_lookup_dcookie;
extern const long __NR_epoll_create;
extern const long __NR_epoll_wait;
extern const long __NR_epoll_ctl;
extern const long __NR_getdents;
extern const long __NR_set_tid_address;
extern const long __NR_restart_syscall;
extern const long __NR_semtimedop;
extern const long __NR_fadvise;
extern const long __NR_timer_create;
extern const long __NR_timer_settime;
extern const long __NR_timer_gettime;
extern const long __NR_timer_getoverrun;
extern const long __NR_timer_delete;
extern const long __NR_clock_settime;
extern const long __NR_clock_gettime;
extern const long __NR_clock_getres;
extern const long __NR_clock_nanosleep;
extern const long __NR_tgkill;
extern const long __NR_mbind;
extern const long __NR_set_mempolicy;
extern const long __NR_get_mempolicy;
extern const long __NR_mq_open;
extern const long __NR_mq_unlink;
extern const long __NR_mq_timedsend;
extern const long __NR_mq_timedreceive;
extern const long __NR_mq_notify;
extern const long __NR_mq_getsetattr;
extern const long __NR_kexec_load;
extern const long __NR_waitid;
extern const long __NR_add_key;
extern const long __NR_request_key;
extern const long __NR_keyctl;
extern const long __NR_ioprio_set;
extern const long __NR_ioprio_get;
extern const long __NR_inotify_init;
extern const long __NR_inotify_add_watch;
extern const long __NR_inotify_rm_watch;
extern const long __NR_openat;
extern const long __NR_mkdirat;
extern const long __NR_fchownat;
extern const long __NR_utime;
extern const long __NR_utimes;
extern const long __NR_futimesat;
extern const long __NR_futimes;
extern const long __NR_futimens;
extern const long __NR_fstatat;
extern const long __NR_unlinkat;
extern const long __NR_renameat;
extern const long __NR_linkat;
extern const long __NR_symlinkat;
extern const long __NR_readlinkat;
extern const long __NR_fchmodat;
extern const long __NR_faccessat;
extern const long __NR_unshare;
extern const long __NR_splice;
extern const long __NR_tee;
extern const long __NR_sync_file_range;
extern const long __NR_vmsplice;
extern const long __NR_migrate_pages;
extern const long __NR_move_pages;
extern const long __NR_preadv;
extern const long __NR_pwritev;
extern const long __NR_utimensat;
extern const long __NR_fallocate;
extern const long __NR_posix_fallocate;
extern const long __NR_accept4;
extern const long __NR_dup3;
extern const long __NR_pipe2;
extern const long __NR_epoll_pwait;
extern const long __NR_epoll_create1;
extern const long __NR_perf_event_open;
extern const long __NR_inotify_init1;
extern const long __NR_rt_tgsigqueueinfo;
extern const long __NR_signalfd;
extern const long __NR_signalfd4;
extern const long __NR_eventfd;
extern const long __NR_eventfd2;
extern const long __NR_timerfd_create;
extern const long __NR_timerfd_settime;
extern const long __NR_timerfd_gettime;
extern const long __NR_recvmmsg;
extern const long __NR_fanotify_init;
extern const long __NR_fanotify_mark;
extern const long __NR_prlimit;
extern const long __NR_name_to_handle_at;
extern const long __NR_open_by_handle_at;
extern const long __NR_clock_adjtime;
extern const long __NR_syncfs;
extern const long __NR_sendmmsg;
extern const long __NR_setns;
extern const long __NR_getcpu;
extern const long __NR_process_vm_readv;
extern const long __NR_process_vm_writev;
extern const long __NR_kcmp;
extern const long __NR_finit_module;
extern const long __NR_sched_setattr;
extern const long __NR_sched_getattr;
extern const long __NR_renameat2;
extern const long __NR_seccomp;
extern const long __NR_getrandom;
extern const long __NR_memfd_create;
extern const long __NR_kexec_file_load;
extern const long __NR_bpf;
extern const long __NR_execveat;
extern const long __NR_userfaultfd;
extern const long __NR_membarrier;
extern const long __NR_mlock2;
extern const long __NR_copy_file_range;
extern const long __NR_preadv2;
extern const long __NR_pwritev2;
extern const long __NR_pkey_mprotect;
extern const long __NR_pkey_alloc;
extern const long __NR_pkey_free;
extern const long __NR_statx;
extern const long __NR_io_pgetevents;
extern const long __NR_rseq;
extern const long __NR_pidfd_send_signal;
extern const long __NR_io_uring_setup;
extern const long __NR_io_uring_enter;
extern const long __NR_io_uring_register;
extern const long __NR_pledge;
extern const long __NR_msyscall;
extern const long __NR_ktrace;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define __NR_exit                   SYMBOLIC(__NR_exit)
#define __NR_exit_group             SYMBOLIC(__NR_exit_group)
#define __NR_read                   SYMBOLIC(__NR_read)
#define __NR_write                  SYMBOLIC(__NR_write)
#define __NR_open                   SYMBOLIC(__NR_open)
#define __NR_close                  SYMBOLIC(__NR_close)
#define __NR_stat                   SYMBOLIC(__NR_stat)
#define __NR_fstat                  SYMBOLIC(__NR_fstat)
#define __NR_lstat                  SYMBOLIC(__NR_lstat)
#define __NR_poll                   SYMBOLIC(__NR_poll)
#define __NR_ppoll                  SYMBOLIC(__NR_ppoll)
#define __NR_lseek                  SYMBOLIC(__NR_lseek)
#define __NR_mmap                   SYMBOLIC(__NR_mmap)
#define __NR_msync                  SYMBOLIC(__NR_msync)
#define __NR_mprotect               SYMBOLIC(__NR_mprotect)
#define __NR_munmap                 SYMBOLIC(__NR_munmap)
#define __NR_sigaction              SYMBOLIC(__NR_sigaction)
#define __NR_sigprocmask            SYMBOLIC(__NR_sigprocmask)
#define __NR_ioctl                  SYMBOLIC(__NR_ioctl)
#define __NR_pread                  SYMBOLIC(__NR_pread)
#define __NR_pwrite                 SYMBOLIC(__NR_pwrite)
#define __NR_readv                  SYMBOLIC(__NR_readv)
#define __NR_writev                 SYMBOLIC(__NR_writev)
#define __NR_access                 SYMBOLIC(__NR_access)
#define __NR_pipe                   SYMBOLIC(__NR_pipe)
#define __NR_select                 SYMBOLIC(__NR_select)
#define __NR_pselect                SYMBOLIC(__NR_pselect)
#define __NR_pselect6               SYMBOLIC(__NR_pselect6)
#define __NR_sched_yield            SYMBOLIC(__NR_sched_yield)
#define __NR_mremap                 SYMBOLIC(__NR_mremap)
#define __NR_mincore                SYMBOLIC(__NR_mincore)
#define __NR_madvise                SYMBOLIC(__NR_madvise)
#define __NR_shmget                 SYMBOLIC(__NR_shmget)
#define __NR_shmat                  SYMBOLIC(__NR_shmat)
#define __NR_shmctl                 SYMBOLIC(__NR_shmctl)
#define __NR_dup                    SYMBOLIC(__NR_dup)
#define __NR_dup2                   SYMBOLIC(__NR_dup2)
#define __NR_pause                  SYMBOLIC(__NR_pause)
#define __NR_nanosleep              SYMBOLIC(__NR_nanosleep)
#define __NR_getitimer              SYMBOLIC(__NR_getitimer)
#define __NR_setitimer              SYMBOLIC(__NR_setitimer)
#define __NR_alarm                  SYMBOLIC(__NR_alarm)
#define __NR_getpid                 SYMBOLIC(__NR_getpid)
#define __NR_sendfile               SYMBOLIC(__NR_sendfile)
#define __NR_socket                 SYMBOLIC(__NR_socket)
#define __NR_connect                SYMBOLIC(__NR_connect)
#define __NR_accept                 SYMBOLIC(__NR_accept)
#define __NR_sendto                 SYMBOLIC(__NR_sendto)
#define __NR_recvfrom               SYMBOLIC(__NR_recvfrom)
#define __NR_sendmsg                SYMBOLIC(__NR_sendmsg)
#define __NR_recvmsg                SYMBOLIC(__NR_recvmsg)
#define __NR_shutdown               SYMBOLIC(__NR_shutdown)
#define __NR_bind                   SYMBOLIC(__NR_bind)
#define __NR_listen                 SYMBOLIC(__NR_listen)
#define __NR_getsockname            SYMBOLIC(__NR_getsockname)
#define __NR_getpeername            SYMBOLIC(__NR_getpeername)
#define __NR_socketpair             SYMBOLIC(__NR_socketpair)
#define __NR_setsockopt             SYMBOLIC(__NR_setsockopt)
#define __NR_getsockopt             SYMBOLIC(__NR_getsockopt)
#define __NR_fork                   SYMBOLIC(__NR_fork)
#define __NR_vfork                  SYMBOLIC(__NR_vfork)
#define __NR_posix_spawn            SYMBOLIC(__NR_posix_spawn)
#define __NR_execve                 LITERALLY(0x003b)
#define __NR_wait4                  SYMBOLIC(__NR_wait4)
#define __NR_kill                   SYMBOLIC(__NR_kill)
#define __NR_killpg                 SYMBOLIC(__NR_killpg)
#define __NR_clone                  SYMBOLIC(__NR_clone)
#define __NR_tkill                  SYMBOLIC(__NR_tkill)
#define __NR_futex                  SYMBOLIC(__NR_futex)
#define __NR_set_robust_list        SYMBOLIC(__NR_set_robust_list)
#define __NR_get_robust_list        SYMBOLIC(__NR_get_robust_list)
#define __NR_uname                  SYMBOLIC(__NR_uname)
#define __NR_semget                 SYMBOLIC(__NR_semget)
#define __NR_semop                  SYMBOLIC(__NR_semop)
#define __NR_semctl                 SYMBOLIC(__NR_semctl)
#define __NR_shmdt                  SYMBOLIC(__NR_shmdt)
#define __NR_msgget                 SYMBOLIC(__NR_msgget)
#define __NR_msgsnd                 SYMBOLIC(__NR_msgsnd)
#define __NR_msgrcv                 SYMBOLIC(__NR_msgrcv)
#define __NR_msgctl                 SYMBOLIC(__NR_msgctl)
#define __NR_fcntl                  SYMBOLIC(__NR_fcntl)
#define __NR_flock                  SYMBOLIC(__NR_flock)
#define __NR_fsync                  SYMBOLIC(__NR_fsync)
#define __NR_fdatasync              SYMBOLIC(__NR_fdatasync)
#define __NR_truncate               SYMBOLIC(__NR_truncate)
#define __NR_ftruncate              SYMBOLIC(__NR_ftruncate)
#define __NR_getcwd                 SYMBOLIC(__NR_getcwd)
#define __NR_chdir                  SYMBOLIC(__NR_chdir)
#define __NR_fchdir                 SYMBOLIC(__NR_fchdir)
#define __NR_rename                 SYMBOLIC(__NR_rename)
#define __NR_mkdir                  SYMBOLIC(__NR_mkdir)
#define __NR_rmdir                  SYMBOLIC(__NR_rmdir)
#define __NR_creat                  SYMBOLIC(__NR_creat)
#define __NR_link                   SYMBOLIC(__NR_link)
#define __NR_unlink                 SYMBOLIC(__NR_unlink)
#define __NR_symlink                SYMBOLIC(__NR_symlink)
#define __NR_readlink               SYMBOLIC(__NR_readlink)
#define __NR_chmod                  SYMBOLIC(__NR_chmod)
#define __NR_fchmod                 SYMBOLIC(__NR_fchmod)
#define __NR_chown                  SYMBOLIC(__NR_chown)
#define __NR_fchown                 SYMBOLIC(__NR_fchown)
#define __NR_lchown                 SYMBOLIC(__NR_lchown)
#define __NR_umask                  SYMBOLIC(__NR_umask)
#define __NR_gettimeofday           SYMBOLIC(__NR_gettimeofday)
#define __NR_getrlimit              SYMBOLIC(__NR_getrlimit)
#define __NR_getrusage              SYMBOLIC(__NR_getrusage)
#define __NR_sysinfo                SYMBOLIC(__NR_sysinfo)
#define __NR_times                  SYMBOLIC(__NR_times)
#define __NR_ptrace                 SYMBOLIC(__NR_ptrace)
#define __NR_syslog                 SYMBOLIC(__NR_syslog)
#define __NR_getuid                 SYMBOLIC(__NR_getuid)
#define __NR_getgid                 SYMBOLIC(__NR_getgid)
#define __NR_getppid                SYMBOLIC(__NR_getppid)
#define __NR_getpgrp                SYMBOLIC(__NR_getpgrp)
#define __NR_setsid                 SYMBOLIC(__NR_setsid)
#define __NR_getsid                 SYMBOLIC(__NR_getsid)
#define __NR_getpgid                SYMBOLIC(__NR_getpgid)
#define __NR_setpgid                SYMBOLIC(__NR_setpgid)
#define __NR_geteuid                SYMBOLIC(__NR_geteuid)
#define __NR_getegid                SYMBOLIC(__NR_getegid)
#define __NR_getgroups              SYMBOLIC(__NR_getgroups)
#define __NR_setgroups              SYMBOLIC(__NR_setgroups)
#define __NR_setreuid               SYMBOLIC(__NR_setreuid)
#define __NR_setregid               SYMBOLIC(__NR_setregid)
#define __NR_setuid                 SYMBOLIC(__NR_setuid)
#define __NR_setgid                 SYMBOLIC(__NR_setgid)
#define __NR_setresuid              SYMBOLIC(__NR_setresuid)
#define __NR_setresgid              SYMBOLIC(__NR_setresgid)
#define __NR_getresuid              SYMBOLIC(__NR_getresuid)
#define __NR_getresgid              SYMBOLIC(__NR_getresgid)
#define __NR_sigpending             SYMBOLIC(__NR_sigpending)
#define __NR_sigsuspend             SYMBOLIC(__NR_sigsuspend)
#define __NR_sigaltstack            SYMBOLIC(__NR_sigaltstack)
#define __NR_mknod                  SYMBOLIC(__NR_mknod)
#define __NR_mknodat                SYMBOLIC(__NR_mknodat)
#define __NR_mkfifo                 SYMBOLIC(__NR_mkfifo)
#define __NR_mkfifoat               SYMBOLIC(__NR_mkfifoat)
#define __NR_statfs                 SYMBOLIC(__NR_statfs)
#define __NR_fstatfs                SYMBOLIC(__NR_fstatfs)
#define __NR_getpriority            SYMBOLIC(__NR_getpriority)
#define __NR_setpriority            SYMBOLIC(__NR_setpriority)
#define __NR_mlock                  SYMBOLIC(__NR_mlock)
#define __NR_munlock                SYMBOLIC(__NR_munlock)
#define __NR_mlockall               SYMBOLIC(__NR_mlockall)
#define __NR_munlockall             SYMBOLIC(__NR_munlockall)
#define __NR_setrlimit              SYMBOLIC(__NR_setrlimit)
#define __NR_chroot                 SYMBOLIC(__NR_chroot)
#define __NR_sync                   SYMBOLIC(__NR_sync)
#define __NR_acct                   SYMBOLIC(__NR_acct)
#define __NR_settimeofday           SYMBOLIC(__NR_settimeofday)
#define __NR_mount                  SYMBOLIC(__NR_mount)
#define __NR_reboot                 SYMBOLIC(__NR_reboot)
#define __NR_quotactl               SYMBOLIC(__NR_quotactl)
#define __NR_setfsuid               SYMBOLIC(__NR_setfsuid)
#define __NR_setfsgid               SYMBOLIC(__NR_setfsgid)
#define __NR_capget                 SYMBOLIC(__NR_capget)
#define __NR_capset                 SYMBOLIC(__NR_capset)
#define __NR_sigtimedwait           SYMBOLIC(__NR_sigtimedwait)
#define __NR_rt_sigqueueinfo        SYMBOLIC(__NR_rt_sigqueueinfo)
#define __NR_personality            SYMBOLIC(__NR_personality)
#define __NR_ustat                  SYMBOLIC(__NR_ustat)
#define __NR_sysfs                  SYMBOLIC(__NR_sysfs)
#define __NR_sched_setparam         SYMBOLIC(__NR_sched_setparam)
#define __NR_sched_getparam         SYMBOLIC(__NR_sched_getparam)
#define __NR_sched_setscheduler     SYMBOLIC(__NR_sched_setscheduler)
#define __NR_sched_getscheduler     SYMBOLIC(__NR_sched_getscheduler)
#define __NR_sched_get_priority_max SYMBOLIC(__NR_sched_get_priority_max)
#define __NR_sched_get_priority_min SYMBOLIC(__NR_sched_get_priority_min)
#define __NR_sched_rr_get_interval  SYMBOLIC(__NR_sched_rr_get_interval)
#define __NR_vhangup                SYMBOLIC(__NR_vhangup)
#define __NR_modify_ldt             SYMBOLIC(__NR_modify_ldt)
#define __NR_pivot_root             SYMBOLIC(__NR_pivot_root)
#define __NR__sysctl                SYMBOLIC(__NR__sysctl)
#define __NR_prctl                  SYMBOLIC(__NR_prctl)
#define __NR_arch_prctl             SYMBOLIC(__NR_arch_prctl)
#define __NR_adjtimex               SYMBOLIC(__NR_adjtimex)
#define __NR_umount2                SYMBOLIC(__NR_umount2)
#define __NR_swapon                 SYMBOLIC(__NR_swapon)
#define __NR_swapoff                SYMBOLIC(__NR_swapoff)
#define __NR_sethostname            SYMBOLIC(__NR_sethostname)
#define __NR_setdomainname          SYMBOLIC(__NR_setdomainname)
#define __NR_iopl                   SYMBOLIC(__NR_iopl)
#define __NR_ioperm                 SYMBOLIC(__NR_ioperm)
#define __NR_init_module            SYMBOLIC(__NR_init_module)
#define __NR_delete_module          SYMBOLIC(__NR_delete_module)
#define __NR_gettid                 SYMBOLIC(__NR_gettid)
#define __NR_readahead              SYMBOLIC(__NR_readahead)
#define __NR_setxattr               SYMBOLIC(__NR_setxattr)
#define __NR_fsetxattr              SYMBOLIC(__NR_fsetxattr)
#define __NR_getxattr               SYMBOLIC(__NR_getxattr)
#define __NR_fgetxattr              SYMBOLIC(__NR_fgetxattr)
#define __NR_listxattr              SYMBOLIC(__NR_listxattr)
#define __NR_flistxattr             SYMBOLIC(__NR_flistxattr)
#define __NR_removexattr            SYMBOLIC(__NR_removexattr)
#define __NR_fremovexattr           SYMBOLIC(__NR_fremovexattr)
#define __NR_lsetxattr              SYMBOLIC(__NR_lsetxattr)
#define __NR_lgetxattr              SYMBOLIC(__NR_lgetxattr)
#define __NR_llistxattr             SYMBOLIC(__NR_llistxattr)
#define __NR_lremovexattr           SYMBOLIC(__NR_lremovexattr)
#define __NR_sched_setaffinity      SYMBOLIC(__NR_sched_setaffinity)
#define __NR_sched_getaffinity      SYMBOLIC(__NR_sched_getaffinity)
#define __NR_cpuset_getaffinity     SYMBOLIC(__NR_cpuset_getaffinity)
#define __NR_cpuset_setaffinity     SYMBOLIC(__NR_cpuset_setaffinity)
#define __NR_io_setup               SYMBOLIC(__NR_io_setup)
#define __NR_io_destroy             SYMBOLIC(__NR_io_destroy)
#define __NR_io_getevents           SYMBOLIC(__NR_io_getevents)
#define __NR_io_submit              SYMBOLIC(__NR_io_submit)
#define __NR_io_cancel              SYMBOLIC(__NR_io_cancel)
#define __NR_lookup_dcookie         SYMBOLIC(__NR_lookup_dcookie)
#define __NR_epoll_create           SYMBOLIC(__NR_epoll_create)
#define __NR_epoll_wait             SYMBOLIC(__NR_epoll_wait)
#define __NR_epoll_ctl              SYMBOLIC(__NR_epoll_ctl)
#define __NR_getdents               SYMBOLIC(__NR_getdents)
#define __NR_set_tid_address        SYMBOLIC(__NR_set_tid_address)
#define __NR_restart_syscall        SYMBOLIC(__NR_restart_syscall)
#define __NR_semtimedop             SYMBOLIC(__NR_semtimedop)
#define __NR_fadvise                SYMBOLIC(__NR_fadvise)
#define __NR_timer_create           SYMBOLIC(__NR_timer_create)
#define __NR_timer_settime          SYMBOLIC(__NR_timer_settime)
#define __NR_timer_gettime          SYMBOLIC(__NR_timer_gettime)
#define __NR_timer_getoverrun       SYMBOLIC(__NR_timer_getoverrun)
#define __NR_timer_delete           SYMBOLIC(__NR_timer_delete)
#define __NR_clock_settime          SYMBOLIC(__NR_clock_settime)
#define __NR_clock_gettime          SYMBOLIC(__NR_clock_gettime)
#define __NR_clock_getres           SYMBOLIC(__NR_clock_getres)
#define __NR_clock_nanosleep        SYMBOLIC(__NR_clock_nanosleep)
#define __NR_tgkill                 SYMBOLIC(__NR_tgkill)
#define __NR_mbind                  SYMBOLIC(__NR_mbind)
#define __NR_set_mempolicy          SYMBOLIC(__NR_set_mempolicy)
#define __NR_get_mempolicy          SYMBOLIC(__NR_get_mempolicy)
#define __NR_mq_open                SYMBOLIC(__NR_mq_open)
#define __NR_mq_unlink              SYMBOLIC(__NR_mq_unlink)
#define __NR_mq_timedsend           SYMBOLIC(__NR_mq_timedsend)
#define __NR_mq_timedreceive        SYMBOLIC(__NR_mq_timedreceive)
#define __NR_mq_notify              SYMBOLIC(__NR_mq_notify)
#define __NR_mq_getsetattr          SYMBOLIC(__NR_mq_getsetattr)
#define __NR_kexec_load             SYMBOLIC(__NR_kexec_load)
#define __NR_waitid                 SYMBOLIC(__NR_waitid)
#define __NR_add_key                SYMBOLIC(__NR_add_key)
#define __NR_request_key            SYMBOLIC(__NR_request_key)
#define __NR_keyctl                 SYMBOLIC(__NR_keyctl)
#define __NR_ioprio_set             SYMBOLIC(__NR_ioprio_set)
#define __NR_ioprio_get             SYMBOLIC(__NR_ioprio_get)
#define __NR_inotify_init           SYMBOLIC(__NR_inotify_init)
#define __NR_inotify_add_watch      SYMBOLIC(__NR_inotify_add_watch)
#define __NR_inotify_rm_watch       SYMBOLIC(__NR_inotify_rm_watch)
#define __NR_openat                 SYMBOLIC(__NR_openat)
#define __NR_mkdirat                SYMBOLIC(__NR_mkdirat)
#define __NR_fchownat               SYMBOLIC(__NR_fchownat)
#define __NR_utime                  SYMBOLIC(__NR_utime)
#define __NR_utimes                 SYMBOLIC(__NR_utimes)
#define __NR_futimesat              SYMBOLIC(__NR_futimesat)
#define __NR_futimes                SYMBOLIC(__NR_futimes)
#define __NR_futimens               SYMBOLIC(__NR_futimens)
#define __NR_fstatat                SYMBOLIC(__NR_fstatat)
#define __NR_unlinkat               SYMBOLIC(__NR_unlinkat)
#define __NR_renameat               SYMBOLIC(__NR_renameat)
#define __NR_linkat                 SYMBOLIC(__NR_linkat)
#define __NR_symlinkat              SYMBOLIC(__NR_symlinkat)
#define __NR_readlinkat             SYMBOLIC(__NR_readlinkat)
#define __NR_fchmodat               SYMBOLIC(__NR_fchmodat)
#define __NR_faccessat              SYMBOLIC(__NR_faccessat)
#define __NR_unshare                SYMBOLIC(__NR_unshare)
#define __NR_splice                 SYMBOLIC(__NR_splice)
#define __NR_tee                    SYMBOLIC(__NR_tee)
#define __NR_sync_file_range        SYMBOLIC(__NR_sync_file_range)
#define __NR_vmsplice               SYMBOLIC(__NR_vmsplice)
#define __NR_migrate_pages          SYMBOLIC(__NR_migrate_pages)
#define __NR_move_pages             SYMBOLIC(__NR_move_pages)
#define __NR_preadv                 SYMBOLIC(__NR_preadv)
#define __NR_pwritev                SYMBOLIC(__NR_pwritev)
#define __NR_utimensat              SYMBOLIC(__NR_utimensat)
#define __NR_fallocate              SYMBOLIC(__NR_fallocate)
#define __NR_posix_fallocate        SYMBOLIC(__NR_posix_fallocate)
#define __NR_accept4                SYMBOLIC(__NR_accept4)
#define __NR_dup3                   SYMBOLIC(__NR_dup3)
#define __NR_pipe2                  SYMBOLIC(__NR_pipe2)
#define __NR_epoll_pwait            SYMBOLIC(__NR_epoll_pwait)
#define __NR_epoll_create1          SYMBOLIC(__NR_epoll_create1)
#define __NR_perf_event_open        SYMBOLIC(__NR_perf_event_open)
#define __NR_inotify_init1          SYMBOLIC(__NR_inotify_init1)
#define __NR_rt_tgsigqueueinfo      SYMBOLIC(__NR_rt_tgsigqueueinfo)
#define __NR_signalfd               SYMBOLIC(__NR_signalfd)
#define __NR_signalfd4              SYMBOLIC(__NR_signalfd4)
#define __NR_eventfd                SYMBOLIC(__NR_eventfd)
#define __NR_eventfd2               SYMBOLIC(__NR_eventfd2)
#define __NR_timerfd_create         SYMBOLIC(__NR_timerfd_create)
#define __NR_timerfd_settime        SYMBOLIC(__NR_timerfd_settime)
#define __NR_timerfd_gettime        SYMBOLIC(__NR_timerfd_gettime)
#define __NR_recvmmsg               SYMBOLIC(__NR_recvmmsg)
#define __NR_fanotify_init          SYMBOLIC(__NR_fanotify_init)
#define __NR_fanotify_mark          SYMBOLIC(__NR_fanotify_mark)
#define __NR_prlimit                SYMBOLIC(__NR_prlimit)
#define __NR_name_to_handle_at      SYMBOLIC(__NR_name_to_handle_at)
#define __NR_open_by_handle_at      SYMBOLIC(__NR_open_by_handle_at)
#define __NR_clock_adjtime          SYMBOLIC(__NR_clock_adjtime)
#define __NR_syncfs                 SYMBOLIC(__NR_syncfs)
#define __NR_sendmmsg               SYMBOLIC(__NR_sendmmsg)
#define __NR_setns                  SYMBOLIC(__NR_setns)
#define __NR_getcpu                 SYMBOLIC(__NR_getcpu)
#define __NR_process_vm_readv       SYMBOLIC(__NR_process_vm_readv)
#define __NR_process_vm_writev      SYMBOLIC(__NR_process_vm_writev)
#define __NR_kcmp                   SYMBOLIC(__NR_kcmp)
#define __NR_finit_module           SYMBOLIC(__NR_finit_module)
#define __NR_sched_setattr          SYMBOLIC(__NR_sched_setattr)
#define __NR_sched_getattr          SYMBOLIC(__NR_sched_getattr)
#define __NR_renameat2              SYMBOLIC(__NR_renameat2)
#define __NR_seccomp                SYMBOLIC(__NR_seccomp)
#define __NR_getrandom              SYMBOLIC(__NR_getrandom)
#define __NR_memfd_create           SYMBOLIC(__NR_memfd_create)
#define __NR_kexec_file_load        SYMBOLIC(__NR_kexec_file_load)
#define __NR_bpf                    SYMBOLIC(__NR_bpf)
#define __NR_execveat               SYMBOLIC(__NR_execveat)
#define __NR_userfaultfd            SYMBOLIC(__NR_userfaultfd)
#define __NR_membarrier             SYMBOLIC(__NR_membarrier)
#define __NR_mlock2                 SYMBOLIC(__NR_mlock2)
#define __NR_copy_file_range        SYMBOLIC(__NR_copy_file_range)
#define __NR_preadv2                SYMBOLIC(__NR_preadv2)
#define __NR_pwritev2               SYMBOLIC(__NR_pwritev2)
#define __NR_pkey_mprotect          SYMBOLIC(__NR_pkey_mprotect)
#define __NR_pkey_alloc             SYMBOLIC(__NR_pkey_alloc)
#define __NR_pkey_free              SYMBOLIC(__NR_pkey_free)
#define __NR_statx                  SYMBOLIC(__NR_statx)
#define __NR_io_pgetevents          SYMBOLIC(__NR_io_pgetevents)
#define __NR_rseq                   SYMBOLIC(__NR_rseq)
#define __NR_pidfd_send_signal      SYMBOLIC(__NR_pidfd_send_signal)
#define __NR_io_uring_setup         SYMBOLIC(__NR_io_uring_setup)
#define __NR_io_uring_enter         SYMBOLIC(__NR_io_uring_enter)
#define __NR_pledge                 SYMBOLIC(__NR_pledge)
#define __NR_msyscall               SYMBOLIC(__NR_msyscall)
#define __NR_ktrace                 SYMBOLIC(__NR_ktrace)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_NR_H_ */
