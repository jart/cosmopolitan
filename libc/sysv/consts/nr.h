#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_NR_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_NR_H_
#include "libc/runtime/symbolic.h"

#define __NR_exit                    SYMBOLIC(__NR_exit)
#define __NR_exit_group              SYMBOLIC(__NR_exit_group)
#define __NR_read                    SYMBOLIC(__NR_read)
#define __NR_write                   SYMBOLIC(__NR_write)
#define __NR_open                    SYMBOLIC(__NR_open)
#define __NR_close                   SYMBOLIC(__NR_close)
#define __NR_stat                    SYMBOLIC(__NR_stat)
#define __NR_fstat                   SYMBOLIC(__NR_fstat)
#define __NR_lstat                   SYMBOLIC(__NR_lstat)
#define __NR_poll                    SYMBOLIC(__NR_poll)
#define __NR_ppoll                   SYMBOLIC(__NR_ppoll)
#define __NR_lseek                   SYMBOLIC(__NR_lseek)
#define __NR_mmap                    SYMBOLIC(__NR_mmap)
#define __NR_msync                   SYMBOLIC(__NR_msync)
#define __NR_mprotect                SYMBOLIC(__NR_mprotect)
#define __NR_munmap                  SYMBOLIC(__NR_munmap)
#define __NR_sigaction               SYMBOLIC(__NR_sigaction)
#define __NR_sigprocmask             SYMBOLIC(__NR_sigprocmask)
#define __NR_ioctl                   SYMBOLIC(__NR_ioctl)
#define __NR_pread                   SYMBOLIC(__NR_pread)
#define __NR_pwrite                  SYMBOLIC(__NR_pwrite)
#define __NR_readv                   SYMBOLIC(__NR_readv)
#define __NR_writev                  SYMBOLIC(__NR_writev)
#define __NR_access                  SYMBOLIC(__NR_access)
#define __NR_pipe                    SYMBOLIC(__NR_pipe)
#define __NR_select                  SYMBOLIC(__NR_select)
#define __NR_pselect                 SYMBOLIC(__NR_pselect)
#define __NR_pselect6                SYMBOLIC(__NR_pselect6)
#define __NR_sched_yield             SYMBOLIC(__NR_sched_yield)
#define __NR_mremap                  SYMBOLIC(__NR_mremap)
#define __NR_mincore                 SYMBOLIC(__NR_mincore)
#define __NR_madvise                 SYMBOLIC(__NR_madvise)
#define __NR_shmget                  SYMBOLIC(__NR_shmget)
#define __NR_shmat                   SYMBOLIC(__NR_shmat)
#define __NR_shmctl                  SYMBOLIC(__NR_shmctl)
#define __NR_dup                     SYMBOLIC(__NR_dup)
#define __NR_dup2                    SYMBOLIC(__NR_dup2)
#define __NR_pause                   SYMBOLIC(__NR_pause)
#define __NR_nanosleep               SYMBOLIC(__NR_nanosleep)
#define __NR_getitimer               SYMBOLIC(__NR_getitimer)
#define __NR_setitimer               SYMBOLIC(__NR_setitimer)
#define __NR_alarm                   SYMBOLIC(__NR_alarm)
#define __NR_getpid                  SYMBOLIC(__NR_getpid)
#define __NR_sendfile                SYMBOLIC(__NR_sendfile)
#define __NR_socket                  SYMBOLIC(__NR_socket)
#define __NR_connect                 SYMBOLIC(__NR_connect)
#define __NR_accept                  SYMBOLIC(__NR_accept)
#define __NR_sendto                  SYMBOLIC(__NR_sendto)
#define __NR_recvfrom                SYMBOLIC(__NR_recvfrom)
#define __NR_sendmsg                 SYMBOLIC(__NR_sendmsg)
#define __NR_recvmsg                 SYMBOLIC(__NR_recvmsg)
#define __NR_shutdown                SYMBOLIC(__NR_shutdown)
#define __NR_bind                    SYMBOLIC(__NR_bind)
#define __NR_listen                  SYMBOLIC(__NR_listen)
#define __NR_getsockname             SYMBOLIC(__NR_getsockname)
#define __NR_getpeername             SYMBOLIC(__NR_getpeername)
#define __NR_socketpair              SYMBOLIC(__NR_socketpair)
#define __NR_setsockopt              SYMBOLIC(__NR_setsockopt)
#define __NR_getsockopt              SYMBOLIC(__NR_getsockopt)
#define __NR_fork                    SYMBOLIC(__NR_fork)
#define __NR_vfork                   SYMBOLIC(__NR_vfork)
#define __NR_posix_spawn             SYMBOLIC(__NR_posix_spawn)
#define __NR_execve                  SYMBOLIC(__NR_execve)
#define __NR_wait4                   SYMBOLIC(__NR_wait4)
#define __NR_kill                    SYMBOLIC(__NR_kill)
#define __NR_killpg                  SYMBOLIC(__NR_killpg)
#define __NR_clone                   SYMBOLIC(__NR_clone)
#define __NR_tkill                   SYMBOLIC(__NR_tkill)
#define __NR_futex                   SYMBOLIC(__NR_futex)
#define __NR_set_robust_list         SYMBOLIC(__NR_set_robust_list)
#define __NR_get_robust_list         SYMBOLIC(__NR_get_robust_list)
#define __NR_uname                   SYMBOLIC(__NR_uname)
#define __NR_semget                  SYMBOLIC(__NR_semget)
#define __NR_semop                   SYMBOLIC(__NR_semop)
#define __NR_semctl                  SYMBOLIC(__NR_semctl)
#define __NR_shmdt                   SYMBOLIC(__NR_shmdt)
#define __NR_msgget                  SYMBOLIC(__NR_msgget)
#define __NR_msgsnd                  SYMBOLIC(__NR_msgsnd)
#define __NR_msgrcv                  SYMBOLIC(__NR_msgrcv)
#define __NR_msgctl                  SYMBOLIC(__NR_msgctl)
#define __NR_fcntl                   SYMBOLIC(__NR_fcntl)
#define __NR_flock                   SYMBOLIC(__NR_flock)
#define __NR_fsync                   SYMBOLIC(__NR_fsync)
#define __NR_fdatasync               SYMBOLIC(__NR_fdatasync)
#define __NR_truncate                SYMBOLIC(__NR_truncate)
#define __NR_ftruncate               SYMBOLIC(__NR_ftruncate)
#define __NR_getcwd                  SYMBOLIC(__NR_getcwd)
#define __NR_chdir                   SYMBOLIC(__NR_chdir)
#define __NR_fchdir                  SYMBOLIC(__NR_fchdir)
#define __NR_rename                  SYMBOLIC(__NR_rename)
#define __NR_mkdir                   SYMBOLIC(__NR_mkdir)
#define __NR_rmdir                   SYMBOLIC(__NR_rmdir)
#define __NR_creat                   SYMBOLIC(__NR_creat)
#define __NR_link                    SYMBOLIC(__NR_link)
#define __NR_unlink                  SYMBOLIC(__NR_unlink)
#define __NR_symlink                 SYMBOLIC(__NR_symlink)
#define __NR_readlink                SYMBOLIC(__NR_readlink)
#define __NR_chmod                   SYMBOLIC(__NR_chmod)
#define __NR_fchmod                  SYMBOLIC(__NR_fchmod)
#define __NR_chown                   SYMBOLIC(__NR_chown)
#define __NR_fchown                  SYMBOLIC(__NR_fchown)
#define __NR_lchown                  SYMBOLIC(__NR_lchown)
#define __NR_umask                   SYMBOLIC(__NR_umask)
#define __NR_gettimeofday            SYMBOLIC(__NR_gettimeofday)
#define __NR_getrlimit               SYMBOLIC(__NR_getrlimit)
#define __NR_getrusage               SYMBOLIC(__NR_getrusage)
#define __NR_sysinfo                 SYMBOLIC(__NR_sysinfo)
#define __NR_times                   SYMBOLIC(__NR_times)
#define __NR_ptrace                  SYMBOLIC(__NR_ptrace)
#define __NR_syslog                  SYMBOLIC(__NR_syslog)
#define __NR_getuid                  SYMBOLIC(__NR_getuid)
#define __NR_getgid                  SYMBOLIC(__NR_getgid)
#define __NR_getppid                 SYMBOLIC(__NR_getppid)
#define __NR_getpgrp                 SYMBOLIC(__NR_getpgrp)
#define __NR_setsid                  SYMBOLIC(__NR_setsid)
#define __NR_getsid                  SYMBOLIC(__NR_getsid)
#define __NR_getpgid                 SYMBOLIC(__NR_getpgid)
#define __NR_setpgid                 SYMBOLIC(__NR_setpgid)
#define __NR_geteuid                 SYMBOLIC(__NR_geteuid)
#define __NR_getegid                 SYMBOLIC(__NR_getegid)
#define __NR_getgroups               SYMBOLIC(__NR_getgroups)
#define __NR_setgroups               SYMBOLIC(__NR_setgroups)
#define __NR_setreuid                SYMBOLIC(__NR_setreuid)
#define __NR_setregid                SYMBOLIC(__NR_setregid)
#define __NR_setuid                  SYMBOLIC(__NR_setuid)
#define __NR_setgid                  SYMBOLIC(__NR_setgid)
#define __NR_setresuid               SYMBOLIC(__NR_setresuid)
#define __NR_setresgid               SYMBOLIC(__NR_setresgid)
#define __NR_getresuid               SYMBOLIC(__NR_getresuid)
#define __NR_getresgid               SYMBOLIC(__NR_getresgid)
#define __NR_sigpending              SYMBOLIC(__NR_sigpending)
#define __NR_sigsuspend              SYMBOLIC(__NR_sigsuspend)
#define __NR_sigaltstack             SYMBOLIC(__NR_sigaltstack)
#define __NR_mknod                   SYMBOLIC(__NR_mknod)
#define __NR_mknodat                 SYMBOLIC(__NR_mknodat)
#define __NR_mkfifo                  SYMBOLIC(__NR_mkfifo)
#define __NR_mkfifoat                SYMBOLIC(__NR_mkfifoat)
#define __NR_statfs                  SYMBOLIC(__NR_statfs)
#define __NR_fstatfs                 SYMBOLIC(__NR_fstatfs)
#define __NR_getpriority             SYMBOLIC(__NR_getpriority)
#define __NR_setpriority             SYMBOLIC(__NR_setpriority)
#define __NR_mlock                   SYMBOLIC(__NR_mlock)
#define __NR_munlock                 SYMBOLIC(__NR_munlock)
#define __NR_mlockall                SYMBOLIC(__NR_mlockall)
#define __NR_munlockall              SYMBOLIC(__NR_munlockall)
#define __NR_setrlimit               SYMBOLIC(__NR_setrlimit)
#define __NR_chroot                  SYMBOLIC(__NR_chroot)
#define __NR_sync                    SYMBOLIC(__NR_sync)
#define __NR_acct                    SYMBOLIC(__NR_acct)
#define __NR_settimeofday            SYMBOLIC(__NR_settimeofday)
#define __NR_mount                   SYMBOLIC(__NR_mount)
#define __NR_reboot                  SYMBOLIC(__NR_reboot)
#define __NR_quotactl                SYMBOLIC(__NR_quotactl)
#define __NR_setfsuid                SYMBOLIC(__NR_setfsuid)
#define __NR_setfsgid                SYMBOLIC(__NR_setfsgid)
#define __NR_capget                  SYMBOLIC(__NR_capget)
#define __NR_capset                  SYMBOLIC(__NR_capset)
#define __NR_sigtimedwait            SYMBOLIC(__NR_sigtimedwait)
#define __NR_rt_sigqueueinfo         SYMBOLIC(__NR_rt_sigqueueinfo)
#define __NR_personality             SYMBOLIC(__NR_personality)
#define __NR_ustat                   SYMBOLIC(__NR_ustat)
#define __NR_sysfs                   SYMBOLIC(__NR_sysfs)
#define __NR_sched_setparam          SYMBOLIC(__NR_sched_setparam)
#define __NR_sched_getparam          SYMBOLIC(__NR_sched_getparam)
#define __NR_sched_setscheduler      SYMBOLIC(__NR_sched_setscheduler)
#define __NR_sched_getscheduler      SYMBOLIC(__NR_sched_getscheduler)
#define __NR_sched_get_priority_max  SYMBOLIC(__NR_sched_get_priority_max)
#define __NR_sched_get_priority_min  SYMBOLIC(__NR_sched_get_priority_min)
#define __NR_sched_rr_get_interval   SYMBOLIC(__NR_sched_rr_get_interval)
#define __NR_vhangup                 SYMBOLIC(__NR_vhangup)
#define __NR_modify_ldt              SYMBOLIC(__NR_modify_ldt)
#define __NR_pivot_root              SYMBOLIC(__NR_pivot_root)
#define __NR__sysctl                 SYMBOLIC(__NR__sysctl)
#define __NR_prctl                   SYMBOLIC(__NR_prctl)
#define __NR_arch_prctl              SYMBOLIC(__NR_arch_prctl)
#define __NR_adjtimex                SYMBOLIC(__NR_adjtimex)
#define __NR_umount2                 SYMBOLIC(__NR_umount2)
#define __NR_swapon                  SYMBOLIC(__NR_swapon)
#define __NR_swapoff                 SYMBOLIC(__NR_swapoff)
#define __NR_sethostname             SYMBOLIC(__NR_sethostname)
#define __NR_setdomainname           SYMBOLIC(__NR_setdomainname)
#define __NR_iopl                    SYMBOLIC(__NR_iopl)
#define __NR_ioperm                  SYMBOLIC(__NR_ioperm)
#define __NR_init_module             SYMBOLIC(__NR_init_module)
#define __NR_delete_module           SYMBOLIC(__NR_delete_module)
#define __NR_gettid                  SYMBOLIC(__NR_gettid)
#define __NR_readahead               SYMBOLIC(__NR_readahead)
#define __NR_setxattr                SYMBOLIC(__NR_setxattr)
#define __NR_fsetxattr               SYMBOLIC(__NR_fsetxattr)
#define __NR_getxattr                SYMBOLIC(__NR_getxattr)
#define __NR_fgetxattr               SYMBOLIC(__NR_fgetxattr)
#define __NR_listxattr               SYMBOLIC(__NR_listxattr)
#define __NR_flistxattr              SYMBOLIC(__NR_flistxattr)
#define __NR_removexattr             SYMBOLIC(__NR_removexattr)
#define __NR_fremovexattr            SYMBOLIC(__NR_fremovexattr)
#define __NR_lsetxattr               SYMBOLIC(__NR_lsetxattr)
#define __NR_lgetxattr               SYMBOLIC(__NR_lgetxattr)
#define __NR_llistxattr              SYMBOLIC(__NR_llistxattr)
#define __NR_lremovexattr            SYMBOLIC(__NR_lremovexattr)
#define __NR_sched_setaffinity       SYMBOLIC(__NR_sched_setaffinity)
#define __NR_sched_getaffinity       SYMBOLIC(__NR_sched_getaffinity)
#define __NR_cpuset_getaffinity      SYMBOLIC(__NR_cpuset_getaffinity)
#define __NR_cpuset_setaffinity      SYMBOLIC(__NR_cpuset_setaffinity)
#define __NR_io_setup                SYMBOLIC(__NR_io_setup)
#define __NR_io_destroy              SYMBOLIC(__NR_io_destroy)
#define __NR_io_getevents            SYMBOLIC(__NR_io_getevents)
#define __NR_io_submit               SYMBOLIC(__NR_io_submit)
#define __NR_io_cancel               SYMBOLIC(__NR_io_cancel)
#define __NR_lookup_dcookie          SYMBOLIC(__NR_lookup_dcookie)
#define __NR_epoll_create            SYMBOLIC(__NR_epoll_create)
#define __NR_epoll_wait              SYMBOLIC(__NR_epoll_wait)
#define __NR_epoll_ctl               SYMBOLIC(__NR_epoll_ctl)
#define __NR_getdents                SYMBOLIC(__NR_getdents)
#define __NR_set_tid_address         SYMBOLIC(__NR_set_tid_address)
#define __NR_restart_syscall         SYMBOLIC(__NR_restart_syscall)
#define __NR_semtimedop              SYMBOLIC(__NR_semtimedop)
#define __NR_fadvise                 SYMBOLIC(__NR_fadvise)
#define __NR_timer_create            SYMBOLIC(__NR_timer_create)
#define __NR_timer_settime           SYMBOLIC(__NR_timer_settime)
#define __NR_timer_gettime           SYMBOLIC(__NR_timer_gettime)
#define __NR_timer_getoverrun        SYMBOLIC(__NR_timer_getoverrun)
#define __NR_timer_delete            SYMBOLIC(__NR_timer_delete)
#define __NR_clock_settime           SYMBOLIC(__NR_clock_settime)
#define __NR_clock_gettime           SYMBOLIC(__NR_clock_gettime)
#define __NR_clock_getres            SYMBOLIC(__NR_clock_getres)
#define __NR_clock_nanosleep         SYMBOLIC(__NR_clock_nanosleep)
#define __NR_tgkill                  SYMBOLIC(__NR_tgkill)
#define __NR_mbind                   SYMBOLIC(__NR_mbind)
#define __NR_set_mempolicy           SYMBOLIC(__NR_set_mempolicy)
#define __NR_get_mempolicy           SYMBOLIC(__NR_get_mempolicy)
#define __NR_mq_open                 SYMBOLIC(__NR_mq_open)
#define __NR_mq_unlink               SYMBOLIC(__NR_mq_unlink)
#define __NR_mq_timedsend            SYMBOLIC(__NR_mq_timedsend)
#define __NR_mq_timedreceive         SYMBOLIC(__NR_mq_timedreceive)
#define __NR_mq_notify               SYMBOLIC(__NR_mq_notify)
#define __NR_mq_getsetattr           SYMBOLIC(__NR_mq_getsetattr)
#define __NR_kexec_load              SYMBOLIC(__NR_kexec_load)
#define __NR_waitid                  SYMBOLIC(__NR_waitid)
#define __NR_add_key                 SYMBOLIC(__NR_add_key)
#define __NR_request_key             SYMBOLIC(__NR_request_key)
#define __NR_keyctl                  SYMBOLIC(__NR_keyctl)
#define __NR_ioprio_set              SYMBOLIC(__NR_ioprio_set)
#define __NR_ioprio_get              SYMBOLIC(__NR_ioprio_get)
#define __NR_inotify_init            SYMBOLIC(__NR_inotify_init)
#define __NR_inotify_add_watch       SYMBOLIC(__NR_inotify_add_watch)
#define __NR_inotify_rm_watch        SYMBOLIC(__NR_inotify_rm_watch)
#define __NR_openat                  SYMBOLIC(__NR_openat)
#define __NR_mkdirat                 SYMBOLIC(__NR_mkdirat)
#define __NR_fchownat                SYMBOLIC(__NR_fchownat)
#define __NR_utime                   SYMBOLIC(__NR_utime)
#define __NR_utimes                  SYMBOLIC(__NR_utimes)
#define __NR_futimesat               SYMBOLIC(__NR_futimesat)
#define __NR_futimes                 SYMBOLIC(__NR_futimes)
#define __NR_futimens                SYMBOLIC(__NR_futimens)
#define __NR_fstatat                 SYMBOLIC(__NR_fstatat)
#define __NR_unlinkat                SYMBOLIC(__NR_unlinkat)
#define __NR_renameat                SYMBOLIC(__NR_renameat)
#define __NR_linkat                  SYMBOLIC(__NR_linkat)
#define __NR_symlinkat               SYMBOLIC(__NR_symlinkat)
#define __NR_readlinkat              SYMBOLIC(__NR_readlinkat)
#define __NR_fchmodat                SYMBOLIC(__NR_fchmodat)
#define __NR_faccessat               SYMBOLIC(__NR_faccessat)
#define __NR_unshare                 SYMBOLIC(__NR_unshare)
#define __NR_splice                  SYMBOLIC(__NR_splice)
#define __NR_tee                     SYMBOLIC(__NR_tee)
#define __NR_sync_file_range         SYMBOLIC(__NR_sync_file_range)
#define __NR_vmsplice                SYMBOLIC(__NR_vmsplice)
#define __NR_migrate_pages           SYMBOLIC(__NR_migrate_pages)
#define __NR_move_pages              SYMBOLIC(__NR_move_pages)
#define __NR_preadv                  SYMBOLIC(__NR_preadv)
#define __NR_pwritev                 SYMBOLIC(__NR_pwritev)
#define __NR_utimensat               SYMBOLIC(__NR_utimensat)
#define __NR_fallocate               SYMBOLIC(__NR_fallocate)
#define __NR_posix_fallocate         SYMBOLIC(__NR_posix_fallocate)
#define __NR_accept4                 SYMBOLIC(__NR_accept4)
#define __NR_dup3                    SYMBOLIC(__NR_dup3)
#define __NR_pipe2                   SYMBOLIC(__NR_pipe2)
#define __NR_epoll_pwait             SYMBOLIC(__NR_epoll_pwait)
#define __NR_epoll_create1           SYMBOLIC(__NR_epoll_create1)
#define __NR_perf_event_open         SYMBOLIC(__NR_perf_event_open)
#define __NR_inotify_init1           SYMBOLIC(__NR_inotify_init1)
#define __NR_rt_tgsigqueueinfo       SYMBOLIC(__NR_rt_tgsigqueueinfo)
#define __NR_signalfd                SYMBOLIC(__NR_signalfd)
#define __NR_signalfd4               SYMBOLIC(__NR_signalfd4)
#define __NR_eventfd                 SYMBOLIC(__NR_eventfd)
#define __NR_eventfd2                SYMBOLIC(__NR_eventfd2)
#define __NR_timerfd_create          SYMBOLIC(__NR_timerfd_create)
#define __NR_timerfd_settime         SYMBOLIC(__NR_timerfd_settime)
#define __NR_timerfd_gettime         SYMBOLIC(__NR_timerfd_gettime)
#define __NR_recvmmsg                SYMBOLIC(__NR_recvmmsg)
#define __NR_fanotify_init           SYMBOLIC(__NR_fanotify_init)
#define __NR_fanotify_mark           SYMBOLIC(__NR_fanotify_mark)
#define __NR_prlimit                 SYMBOLIC(__NR_prlimit)
#define __NR_name_to_handle_at       SYMBOLIC(__NR_name_to_handle_at)
#define __NR_open_by_handle_at       SYMBOLIC(__NR_open_by_handle_at)
#define __NR_clock_adjtime           SYMBOLIC(__NR_clock_adjtime)
#define __NR_syncfs                  SYMBOLIC(__NR_syncfs)
#define __NR_sendmmsg                SYMBOLIC(__NR_sendmmsg)
#define __NR_setns                   SYMBOLIC(__NR_setns)
#define __NR_getcpu                  SYMBOLIC(__NR_getcpu)
#define __NR_process_vm_readv        SYMBOLIC(__NR_process_vm_readv)
#define __NR_process_vm_writev       SYMBOLIC(__NR_process_vm_writev)
#define __NR_kcmp                    SYMBOLIC(__NR_kcmp)
#define __NR_finit_module            SYMBOLIC(__NR_finit_module)
#define __NR_sched_setattr           SYMBOLIC(__NR_sched_setattr)
#define __NR_sched_getattr           SYMBOLIC(__NR_sched_getattr)
#define __NR_renameat2               SYMBOLIC(__NR_renameat2)
#define __NR_seccomp                 SYMBOLIC(__NR_seccomp)
#define __NR_getrandom               SYMBOLIC(__NR_getrandom)
#define __NR_memfd_create            SYMBOLIC(__NR_memfd_create)
#define __NR_kexec_file_load         SYMBOLIC(__NR_kexec_file_load)
#define __NR_bpf                     SYMBOLIC(__NR_bpf)
#define __NR_execveat                SYMBOLIC(__NR_execveat)
#define __NR_userfaultfd             SYMBOLIC(__NR_userfaultfd)
#define __NR_membarrier              SYMBOLIC(__NR_membarrier)
#define __NR_mlock2                  SYMBOLIC(__NR_mlock2)
#define __NR_copy_file_range         SYMBOLIC(__NR_copy_file_range)
#define __NR_preadv2                 SYMBOLIC(__NR_preadv2)
#define __NR_pwritev2                SYMBOLIC(__NR_pwritev2)
#define __NR_pkey_mprotect           SYMBOLIC(__NR_pkey_mprotect)
#define __NR_pkey_alloc              SYMBOLIC(__NR_pkey_alloc)
#define __NR_pkey_free               SYMBOLIC(__NR_pkey_free)
#define __NR_statx                   SYMBOLIC(__NR_statx)
#define __NR_io_pgetevents           SYMBOLIC(__NR_io_pgetevents)
#define __NR_rseq                    SYMBOLIC(__NR_rseq)
#define __NR_pidfd_send_signal       SYMBOLIC(__NR_pidfd_send_signal)
#define __NR_io_uring_setup          SYMBOLIC(__NR_io_uring_setup)
#define __NR_io_uring_enter          SYMBOLIC(__NR_io_uring_enter)
#define __NR_io_uring_register       SYMBOLIC(__NR_io_uring_register)
#define __NR_pledge                  SYMBOLIC(__NR_pledge)
#define __NR_ktrace                  SYMBOLIC(__NR_ktrace)
#define __NR_kqueue                  SYMBOLIC(__NR_kqueue)
#define __NR_kevent                  SYMBOLIC(__NR_kevent)
#define __NR_revoke                  SYMBOLIC(__NR_revoke)
#define __NR_setlogin                SYMBOLIC(__NR_setlogin)
#define __NR_getfh                   SYMBOLIC(__NR_getfh)
#define __NR_chflags                 SYMBOLIC(__NR_chflags)
#define __NR_getfsstat               SYMBOLIC(__NR_getfsstat)
#define __NR_nfssvc                  SYMBOLIC(__NR_nfssvc)
#define __NR_adjtime                 SYMBOLIC(__NR_adjtime)
#define __NR_fchflags                SYMBOLIC(__NR_fchflags)
#define __NR_seteuid                 SYMBOLIC(__NR_seteuid)
#define __NR_setegid                 SYMBOLIC(__NR_setegid)
#define __NR_fpathconf               SYMBOLIC(__NR_fpathconf)
#define __NR_fhopen                  SYMBOLIC(__NR_fhopen)
#define __NR_unmount                 SYMBOLIC(__NR_unmount)
#define __NR_issetugid               SYMBOLIC(__NR_issetugid)
#define __NR_minherit                SYMBOLIC(__NR_minherit)
#define __NR_pathconf                SYMBOLIC(__NR_pathconf)
#define __NR_sysctl                  SYMBOLIC(__NR_sysctl)
#define __NR_ntp_adjtime             SYMBOLIC(__NR_ntp_adjtime)
#define __NR_ntp_gettime             SYMBOLIC(__NR_ntp_gettime)
#define __NR_shm_unlink              SYMBOLIC(__NR_shm_unlink)
#define __NR_shm_open                SYMBOLIC(__NR_shm_open)
#define __NR_aio_read                SYMBOLIC(__NR_aio_read)
#define __NR_aio_suspend             SYMBOLIC(__NR_aio_suspend)
#define __NR_aio_cancel              SYMBOLIC(__NR_aio_cancel)
#define __NR_aio_fsync               SYMBOLIC(__NR_aio_fsync)
#define __NR_aio_error               SYMBOLIC(__NR_aio_error)
#define __NR_aio_return              SYMBOLIC(__NR_aio_return)
#define __NR_aio_write               SYMBOLIC(__NR_aio_write)
#define __NR_aio_waitcomplete        SYMBOLIC(__NR_aio_waitcomplete)
#define __NR_aio_suspend_nocancel    SYMBOLIC(__NR_aio_suspend_nocancel)
#define __NR_aio_mlock               SYMBOLIC(__NR_aio_mlock)
#define __NR_sigwait                 SYMBOLIC(__NR_sigwait)
#define __NR_undelete                SYMBOLIC(__NR_undelete)
#define __NR_getlogin                SYMBOLIC(__NR_getlogin)
#define __NR_getdtablesize           SYMBOLIC(__NR_getdtablesize)
#define __NR_setauid                 SYMBOLIC(__NR_setauid)
#define __NR_audit                   SYMBOLIC(__NR_audit)
#define __NR_auditctl                SYMBOLIC(__NR_auditctl)
#define __NR_getaudit_addr           SYMBOLIC(__NR_getaudit_addr)
#define __NR_getdirentries           SYMBOLIC(__NR_getdirentries)
#define __NR_lio_listio              SYMBOLIC(__NR_lio_listio)
#define __NR_setaudit_addr           SYMBOLIC(__NR_setaudit_addr)
#define __NR_getauid                 SYMBOLIC(__NR_getauid)
#define __NR_semsys                  SYMBOLIC(__NR_semsys)
#define __NR_auditon                 SYMBOLIC(__NR_auditon)
#define __NR_msgsys                  SYMBOLIC(__NR_msgsys)
#define __NR_shmsys                  SYMBOLIC(__NR_shmsys)
#define __NR_fhstat                  SYMBOLIC(__NR_fhstat)
#define __NR_chflagsat               SYMBOLIC(__NR_chflagsat)
#define __NR_profil                  SYMBOLIC(__NR_profil)
#define __NR_fhstatfs                SYMBOLIC(__NR_fhstatfs)
#define __NR_utrace                  SYMBOLIC(__NR_utrace)
#define __NR_closefrom               SYMBOLIC(__NR_closefrom)
#define __NR_pthread_markcancel      SYMBOLIC(__NR_pthread_markcancel)
#define __NR_pthread_kill            SYMBOLIC(__NR_pthread_kill)
#define __NR_pthread_fchdir          SYMBOLIC(__NR_pthread_fchdir)
#define __NR_pthread_sigmask         SYMBOLIC(__NR_pthread_sigmask)
#define __NR_pthread_chdir           SYMBOLIC(__NR_pthread_chdir)
#define __NR_pthread_canceled        SYMBOLIC(__NR_pthread_canceled)
#define __NR_disable_threadsignal    SYMBOLIC(__NR_disable_threadsignal)
#define __NR_abort_with_payload      SYMBOLIC(__NR_abort_with_payload)
#define __NR_accept_nocancel         SYMBOLIC(__NR_accept_nocancel)
#define __NR_access_extended         SYMBOLIC(__NR_access_extended)
#define __NR_audit_session_join      SYMBOLIC(__NR_audit_session_join)
#define __NR_audit_session_port      SYMBOLIC(__NR_audit_session_port)
#define __NR_audit_session_self      SYMBOLIC(__NR_audit_session_self)
#define __NR_bsdthread_create        SYMBOLIC(__NR_bsdthread_create)
#define __NR_bsdthread_ctl           SYMBOLIC(__NR_bsdthread_ctl)
#define __NR_bsdthread_register      SYMBOLIC(__NR_bsdthread_register)
#define __NR_bsdthread_terminate     SYMBOLIC(__NR_bsdthread_terminate)
#define __NR_change_fdguard_np       SYMBOLIC(__NR_change_fdguard_np)
#define __NR_chmod_extended          SYMBOLIC(__NR_chmod_extended)
#define __NR_clonefileat             SYMBOLIC(__NR_clonefileat)
#define __NR_close_nocancel          SYMBOLIC(__NR_close_nocancel)
#define __NR_coalition               SYMBOLIC(__NR_coalition)
#define __NR_coalition_info          SYMBOLIC(__NR_coalition_info)
#define __NR_connect_nocancel        SYMBOLIC(__NR_connect_nocancel)
#define __NR_connectx                SYMBOLIC(__NR_connectx)
#define __NR_copyfile                SYMBOLIC(__NR_copyfile)
#define __NR_csops                   SYMBOLIC(__NR_csops)
#define __NR_csops_audittoken        SYMBOLIC(__NR_csops_audittoken)
#define __NR_csrctl                  SYMBOLIC(__NR_csrctl)
#define __NR_delete                  SYMBOLIC(__NR_delete)
#define __NR_disconnectx             SYMBOLIC(__NR_disconnectx)
#define __NR_exchangedata            SYMBOLIC(__NR_exchangedata)
#define __NR_fchmod_extended         SYMBOLIC(__NR_fchmod_extended)
#define __NR_fclonefileat            SYMBOLIC(__NR_fclonefileat)
#define __NR_fcntl_nocancel          SYMBOLIC(__NR_fcntl_nocancel)
#define __NR_ffsctl                  SYMBOLIC(__NR_ffsctl)
#define __NR_fgetattrlist            SYMBOLIC(__NR_fgetattrlist)
#define __NR_fileport_makefd         SYMBOLIC(__NR_fileport_makefd)
#define __NR_fileport_makeport       SYMBOLIC(__NR_fileport_makeport)
#define __NR_fmount                  SYMBOLIC(__NR_fmount)
#define __NR_fs_snapshot             SYMBOLIC(__NR_fs_snapshot)
#define __NR_fsctl                   SYMBOLIC(__NR_fsctl)
#define __NR_fsetattrlist            SYMBOLIC(__NR_fsetattrlist)
#define __NR_fstat_extended          SYMBOLIC(__NR_fstat_extended)
#define __NR_fsync_nocancel          SYMBOLIC(__NR_fsync_nocancel)
#define __NR_getattrlist             SYMBOLIC(__NR_getattrlist)
#define __NR_getattrlistat           SYMBOLIC(__NR_getattrlistat)
#define __NR_getattrlistbulk         SYMBOLIC(__NR_getattrlistbulk)
#define __NR_getdirentriesattr       SYMBOLIC(__NR_getdirentriesattr)
#define __NR_gethostuuid             SYMBOLIC(__NR_gethostuuid)
#define __NR_getsgroups              SYMBOLIC(__NR_getsgroups)
#define __NR_getwgroups              SYMBOLIC(__NR_getwgroups)
#define __NR_grab_pgo_data           SYMBOLIC(__NR_grab_pgo_data)
#define __NR_guarded_close_np        SYMBOLIC(__NR_guarded_close_np)
#define __NR_guarded_kqueue_np       SYMBOLIC(__NR_guarded_kqueue_np)
#define __NR_guarded_open_np         SYMBOLIC(__NR_guarded_open_np)
#define __NR_guarded_pwrite_np       SYMBOLIC(__NR_guarded_pwrite_np)
#define __NR_guarded_write_np        SYMBOLIC(__NR_guarded_write_np)
#define __NR_guarded_writev_np       SYMBOLIC(__NR_guarded_writev_np)
#define __NR_identitysvc             SYMBOLIC(__NR_identitysvc)
#define __NR_initgroups              SYMBOLIC(__NR_initgroups)
#define __NR_iopolicysys             SYMBOLIC(__NR_iopolicysys)
#define __NR_kas_info                SYMBOLIC(__NR_kas_info)
#define __NR_kdebug_trace            SYMBOLIC(__NR_kdebug_trace)
#define __NR_kdebug_trace_string     SYMBOLIC(__NR_kdebug_trace_string)
#define __NR_kdebug_typefilter       SYMBOLIC(__NR_kdebug_typefilter)
#define __NR_kevent_id               SYMBOLIC(__NR_kevent_id)
#define __NR_kevent_qos              SYMBOLIC(__NR_kevent_qos)
#define __NR_ledger                  SYMBOLIC(__NR_ledger)
#define __NR_lstat_extended          SYMBOLIC(__NR_lstat_extended)
#define __NR_memorystatus_control    SYMBOLIC(__NR_memorystatus_control)
#define __NR_memorystatus_get_level  SYMBOLIC(__NR_memorystatus_get_level)
#define __NR_microstackshot          SYMBOLIC(__NR_microstackshot)
#define __NR_mkdir_extended          SYMBOLIC(__NR_mkdir_extended)
#define __NR_mkfifo_extended         SYMBOLIC(__NR_mkfifo_extended)
#define __NR_modwatch                SYMBOLIC(__NR_modwatch)
#define __NR_mremap_encrypted        SYMBOLIC(__NR_mremap_encrypted)
#define __NR_msgrcv_nocancel         SYMBOLIC(__NR_msgrcv_nocancel)
#define __NR_msgsnd_nocancel         SYMBOLIC(__NR_msgsnd_nocancel)
#define __NR_msync_nocancel          SYMBOLIC(__NR_msync_nocancel)
#define __NR_necp_client_action      SYMBOLIC(__NR_necp_client_action)
#define __NR_necp_match_policy       SYMBOLIC(__NR_necp_match_policy)
#define __NR_necp_open               SYMBOLIC(__NR_necp_open)
#define __NR_necp_session_action     SYMBOLIC(__NR_necp_session_action)
#define __NR_necp_session_open       SYMBOLIC(__NR_necp_session_open)
#define __NR_net_qos_guideline       SYMBOLIC(__NR_net_qos_guideline)
#define __NR_netagent_trigger        SYMBOLIC(__NR_netagent_trigger)
#define __NR_nfsclnt                 SYMBOLIC(__NR_nfsclnt)
#define __NR_open_dprotected_np      SYMBOLIC(__NR_open_dprotected_np)
#define __NR_open_extended           SYMBOLIC(__NR_open_extended)
#define __NR_open_nocancel           SYMBOLIC(__NR_open_nocancel)
#define __NR_openat_nocancel         SYMBOLIC(__NR_openat_nocancel)
#define __NR_openbyid_np             SYMBOLIC(__NR_openbyid_np)
#define __NR_os_fault_with_payload   SYMBOLIC(__NR_os_fault_with_payload)
#define __NR_peeloff                 SYMBOLIC(__NR_peeloff)
#define __NR_persona                 SYMBOLIC(__NR_persona)
#define __NR_pid_hibernate           SYMBOLIC(__NR_pid_hibernate)
#define __NR_pid_resume              SYMBOLIC(__NR_pid_resume)
#define __NR_pid_shutdown_sockets    SYMBOLIC(__NR_pid_shutdown_sockets)
#define __NR_pid_suspend             SYMBOLIC(__NR_pid_suspend)
#define __NR_poll_nocancel           SYMBOLIC(__NR_poll_nocancel)
#define __NR_pread_nocancel          SYMBOLIC(__NR_pread_nocancel)
#define __NR_proc_info               SYMBOLIC(__NR_proc_info)
#define __NR_proc_rlimit_control     SYMBOLIC(__NR_proc_rlimit_control)
#define __NR_proc_trace_log          SYMBOLIC(__NR_proc_trace_log)
#define __NR_proc_uuid_policy        SYMBOLIC(__NR_proc_uuid_policy)
#define __NR_process_policy          SYMBOLIC(__NR_process_policy)
#define __NR_pselect_nocancel        SYMBOLIC(__NR_pselect_nocancel)
#define __NR_psynch_cvbroad          SYMBOLIC(__NR_psynch_cvbroad)
#define __NR_psynch_cvclrprepost     SYMBOLIC(__NR_psynch_cvclrprepost)
#define __NR_psynch_cvsignal         SYMBOLIC(__NR_psynch_cvsignal)
#define __NR_psynch_mutexdrop        SYMBOLIC(__NR_psynch_mutexdrop)
#define __NR_psynch_mutexwait        SYMBOLIC(__NR_psynch_mutexwait)
#define __NR_psynch_rw_downgrade     SYMBOLIC(__NR_psynch_rw_downgrade)
#define __NR_psynch_rw_longrdlock    SYMBOLIC(__NR_psynch_rw_longrdlock)
#define __NR_psynch_rw_rdlock        SYMBOLIC(__NR_psynch_rw_rdlock)
#define __NR_psynch_rw_unlock        SYMBOLIC(__NR_psynch_rw_unlock)
#define __NR_psynch_rw_unlock2       SYMBOLIC(__NR_psynch_rw_unlock2)
#define __NR_psynch_rw_upgrade       SYMBOLIC(__NR_psynch_rw_upgrade)
#define __NR_psynch_rw_wrlock        SYMBOLIC(__NR_psynch_rw_wrlock)
#define __NR_psynch_rw_yieldwrlock   SYMBOLIC(__NR_psynch_rw_yieldwrlock)
#define __NR_pwrite_nocancel         SYMBOLIC(__NR_pwrite_nocancel)
#define __NR_read_nocancel           SYMBOLIC(__NR_read_nocancel)
#define __NR_readv_nocancel          SYMBOLIC(__NR_readv_nocancel)
#define __NR_recvfrom_nocancel       SYMBOLIC(__NR_recvfrom_nocancel)
#define __NR_recvmsg_nocancel        SYMBOLIC(__NR_recvmsg_nocancel)
#define __NR_recvmsg_x               SYMBOLIC(__NR_recvmsg_x)
#define __NR_renameatx_np            SYMBOLIC(__NR_renameatx_np)
#define __NR_searchfs                SYMBOLIC(__NR_searchfs)
#define __NR_select_nocancel         SYMBOLIC(__NR_select_nocancel)
#define __NR_sem_close               SYMBOLIC(__NR_sem_close)
#define __NR_sem_open                SYMBOLIC(__NR_sem_open)
#define __NR_sem_post                SYMBOLIC(__NR_sem_post)
#define __NR_sem_trywait             SYMBOLIC(__NR_sem_trywait)
#define __NR_sem_unlink              SYMBOLIC(__NR_sem_unlink)
#define __NR_sem_wait                SYMBOLIC(__NR_sem_wait)
#define __NR_sem_wait_nocancel       SYMBOLIC(__NR_sem_wait_nocancel)
#define __NR_sendmsg_nocancel        SYMBOLIC(__NR_sendmsg_nocancel)
#define __NR_sendmsg_x               SYMBOLIC(__NR_sendmsg_x)
#define __NR_sendto_nocancel         SYMBOLIC(__NR_sendto_nocancel)
#define __NR_setattrlist             SYMBOLIC(__NR_setattrlist)
#define __NR_setattrlistat           SYMBOLIC(__NR_setattrlistat)
#define __NR_setprivexec             SYMBOLIC(__NR_setprivexec)
#define __NR_setsgroups              SYMBOLIC(__NR_setsgroups)
#define __NR_settid                  SYMBOLIC(__NR_settid)
#define __NR_settid_with_pid         SYMBOLIC(__NR_settid_with_pid)
#define __NR_setwgroups              SYMBOLIC(__NR_setwgroups)
#define __NR_sfi_ctl                 SYMBOLIC(__NR_sfi_ctl)
#define __NR_sfi_pidctl              SYMBOLIC(__NR_sfi_pidctl)
#define __NR_shared_region_check_np  SYMBOLIC(__NR_shared_region_check_np)
#define __NR_sigsuspend_nocancel     SYMBOLIC(__NR_sigsuspend_nocancel)
#define __NR_socket_delegate         SYMBOLIC(__NR_socket_delegate)
#define __NR_stat_extended           SYMBOLIC(__NR_stat_extended)
#define __NR_sysctlbyname            SYMBOLIC(__NR_sysctlbyname)
#define __NR_system_override         SYMBOLIC(__NR_system_override)
#define __NR_telemetry               SYMBOLIC(__NR_telemetry)
#define __NR_terminate_with_payload  SYMBOLIC(__NR_terminate_with_payload)
#define __NR_thread_selfcounts       SYMBOLIC(__NR_thread_selfcounts)
#define __NR_thread_selfid           SYMBOLIC(__NR_thread_selfid)
#define __NR_thread_selfusage        SYMBOLIC(__NR_thread_selfusage)
#define __NR_ulock_wait              SYMBOLIC(__NR_ulock_wait)
#define __NR_ulock_wake              SYMBOLIC(__NR_ulock_wake)
#define __NR_umask_extended          SYMBOLIC(__NR_umask_extended)
#define __NR_usrctl                  SYMBOLIC(__NR_usrctl)
#define __NR_vfs_purge               SYMBOLIC(__NR_vfs_purge)
#define __NR_vm_pressure_monitor     SYMBOLIC(__NR_vm_pressure_monitor)
#define __NR_wait4_nocancel          SYMBOLIC(__NR_wait4_nocancel)
#define __NR_waitevent               SYMBOLIC(__NR_waitevent)
#define __NR_waitid_nocancel         SYMBOLIC(__NR_waitid_nocancel)
#define __NR_watchevent              SYMBOLIC(__NR_watchevent)
#define __NR_work_interval_ctl       SYMBOLIC(__NR_work_interval_ctl)
#define __NR_workq_kernreturn        SYMBOLIC(__NR_workq_kernreturn)
#define __NR_workq_open              SYMBOLIC(__NR_workq_open)
#define __NR_write_nocancel          SYMBOLIC(__NR_write_nocancel)
#define __NR_writev_nocancel         SYMBOLIC(__NR_writev_nocancel)
#define __NR_abort2                  SYMBOLIC(__NR_abort2)
#define __NR_afs3_syscall            SYMBOLIC(__NR_afs3_syscall)
#define __NR_bindat                  SYMBOLIC(__NR_bindat)
#define __NR_break                   SYMBOLIC(__NR_break)
#define __NR_cap_enter               SYMBOLIC(__NR_cap_enter)
#define __NR_cap_fcntls_get          SYMBOLIC(__NR_cap_fcntls_get)
#define __NR_cap_fcntls_limit        SYMBOLIC(__NR_cap_fcntls_limit)
#define __NR_cap_getmode             SYMBOLIC(__NR_cap_getmode)
#define __NR_cap_ioctls_get          SYMBOLIC(__NR_cap_ioctls_get)
#define __NR_cap_ioctls_limit        SYMBOLIC(__NR_cap_ioctls_limit)
#define __NR_cap_rights_limit        SYMBOLIC(__NR_cap_rights_limit)
#define __NR_clock_getcpuclockid2    SYMBOLIC(__NR_clock_getcpuclockid2)
#define __NR_connectat               SYMBOLIC(__NR_connectat)
#define __NR_cpuset                  SYMBOLIC(__NR_cpuset)
#define __NR_cpuset_getdomain        SYMBOLIC(__NR_cpuset_getdomain)
#define __NR_cpuset_getid            SYMBOLIC(__NR_cpuset_getid)
#define __NR_cpuset_setdomain        SYMBOLIC(__NR_cpuset_setdomain)
#define __NR_cpuset_setid            SYMBOLIC(__NR_cpuset_setid)
#define __NR_eaccess                 SYMBOLIC(__NR_eaccess)
#define __NR_extattr_delete_fd       SYMBOLIC(__NR_extattr_delete_fd)
#define __NR_extattr_delete_file     SYMBOLIC(__NR_extattr_delete_file)
#define __NR_extattr_delete_link     SYMBOLIC(__NR_extattr_delete_link)
#define __NR_extattr_get_fd          SYMBOLIC(__NR_extattr_get_fd)
#define __NR_extattr_get_file        SYMBOLIC(__NR_extattr_get_file)
#define __NR_extattr_get_link        SYMBOLIC(__NR_extattr_get_link)
#define __NR_extattr_list_fd         SYMBOLIC(__NR_extattr_list_fd)
#define __NR_extattr_list_file       SYMBOLIC(__NR_extattr_list_file)
#define __NR_extattr_list_link       SYMBOLIC(__NR_extattr_list_link)
#define __NR_extattr_set_fd          SYMBOLIC(__NR_extattr_set_fd)
#define __NR_extattr_set_file        SYMBOLIC(__NR_extattr_set_file)
#define __NR_extattr_set_link        SYMBOLIC(__NR_extattr_set_link)
#define __NR_extattrctl              SYMBOLIC(__NR_extattrctl)
#define __NR_fexecve                 SYMBOLIC(__NR_fexecve)
#define __NR_ffclock_getcounter      SYMBOLIC(__NR_ffclock_getcounter)
#define __NR_ffclock_getestimate     SYMBOLIC(__NR_ffclock_getestimate)
#define __NR_ffclock_setestimate     SYMBOLIC(__NR_ffclock_setestimate)
#define __NR_fhlink                  SYMBOLIC(__NR_fhlink)
#define __NR_fhlinkat                SYMBOLIC(__NR_fhlinkat)
#define __NR_fhreadlink              SYMBOLIC(__NR_fhreadlink)
#define __NR_getaudit                SYMBOLIC(__NR_getaudit)
#define __NR_getcontext              SYMBOLIC(__NR_getcontext)
#define __NR_getfhat                 SYMBOLIC(__NR_getfhat)
#define __NR_gethostid               SYMBOLIC(__NR_gethostid)
#define __NR_getkerninfo             SYMBOLIC(__NR_getkerninfo)
#define __NR_getloginclass           SYMBOLIC(__NR_getloginclass)
#define __NR_getpagesize             SYMBOLIC(__NR_getpagesize)
#define __NR_gssd_syscall            SYMBOLIC(__NR_gssd_syscall)
#define __NR_jail                    SYMBOLIC(__NR_jail)
#define __NR_jail_attach             SYMBOLIC(__NR_jail_attach)
#define __NR_jail_get                SYMBOLIC(__NR_jail_get)
#define __NR_jail_remove             SYMBOLIC(__NR_jail_remove)
#define __NR_jail_set                SYMBOLIC(__NR_jail_set)
#define __NR_kenv                    SYMBOLIC(__NR_kenv)
#define __NR_kldfind                 SYMBOLIC(__NR_kldfind)
#define __NR_kldfirstmod             SYMBOLIC(__NR_kldfirstmod)
#define __NR_kldload                 SYMBOLIC(__NR_kldload)
#define __NR_kldnext                 SYMBOLIC(__NR_kldnext)
#define __NR_kldstat                 SYMBOLIC(__NR_kldstat)
#define __NR_kldsym                  SYMBOLIC(__NR_kldsym)
#define __NR_kldunload               SYMBOLIC(__NR_kldunload)
#define __NR_kldunloadf              SYMBOLIC(__NR_kldunloadf)
#define __NR_kmq_notify              SYMBOLIC(__NR_kmq_notify)
#define __NR_kmq_setattr             SYMBOLIC(__NR_kmq_setattr)
#define __NR_kmq_timedreceive        SYMBOLIC(__NR_kmq_timedreceive)
#define __NR_kmq_timedsend           SYMBOLIC(__NR_kmq_timedsend)
#define __NR_kmq_unlink              SYMBOLIC(__NR_kmq_unlink)
#define __NR_ksem_close              SYMBOLIC(__NR_ksem_close)
#define __NR_ksem_destroy            SYMBOLIC(__NR_ksem_destroy)
#define __NR_ksem_getvalue           SYMBOLIC(__NR_ksem_getvalue)
#define __NR_ksem_init               SYMBOLIC(__NR_ksem_init)
#define __NR_ksem_open               SYMBOLIC(__NR_ksem_open)
#define __NR_ksem_post               SYMBOLIC(__NR_ksem_post)
#define __NR_ksem_timedwait          SYMBOLIC(__NR_ksem_timedwait)
#define __NR_ksem_trywait            SYMBOLIC(__NR_ksem_trywait)
#define __NR_ksem_unlink             SYMBOLIC(__NR_ksem_unlink)
#define __NR_ksem_wait               SYMBOLIC(__NR_ksem_wait)
#define __NR_ktimer_create           SYMBOLIC(__NR_ktimer_create)
#define __NR_ktimer_delete           SYMBOLIC(__NR_ktimer_delete)
#define __NR_ktimer_getoverrun       SYMBOLIC(__NR_ktimer_getoverrun)
#define __NR_ktimer_gettime          SYMBOLIC(__NR_ktimer_gettime)
#define __NR_ktimer_settime          SYMBOLIC(__NR_ktimer_settime)
#define __NR_lchflags                SYMBOLIC(__NR_lchflags)
#define __NR_lchmod                  SYMBOLIC(__NR_lchmod)
#define __NR_lgetfh                  SYMBOLIC(__NR_lgetfh)
#define __NR_lpathconf               SYMBOLIC(__NR_lpathconf)
#define __NR_lutimes                 SYMBOLIC(__NR_lutimes)
#define __NR_mac_syscall             SYMBOLIC(__NR_mac_syscall)
#define __NR_modfind                 SYMBOLIC(__NR_modfind)
#define __NR_modfnext                SYMBOLIC(__NR_modfnext)
#define __NR_modnext                 SYMBOLIC(__NR_modnext)
#define __NR_modstat                 SYMBOLIC(__NR_modstat)
#define __NR_nfstat                  SYMBOLIC(__NR_nfstat)
#define __NR_nlm_syscall             SYMBOLIC(__NR_nlm_syscall)
#define __NR_nlstat                  SYMBOLIC(__NR_nlstat)
#define __NR_nmount                  SYMBOLIC(__NR_nmount)
#define __NR_nnpfs_syscall           SYMBOLIC(__NR_nnpfs_syscall)
#define __NR_nstat                   SYMBOLIC(__NR_nstat)
#define __NR_pdfork                  SYMBOLIC(__NR_pdfork)
#define __NR_pdgetpid                SYMBOLIC(__NR_pdgetpid)
#define __NR_pdkill                  SYMBOLIC(__NR_pdkill)
#define __NR_posix_openpt            SYMBOLIC(__NR_posix_openpt)
#define __NR_procctl                 SYMBOLIC(__NR_procctl)
#define __NR_psynch_cvwait           SYMBOLIC(__NR_psynch_cvwait)
#define __NR_quota                   SYMBOLIC(__NR_quota)
#define __NR_rctl_add_rule           SYMBOLIC(__NR_rctl_add_rule)
#define __NR_rctl_get_limits         SYMBOLIC(__NR_rctl_get_limits)
#define __NR_rctl_get_racct          SYMBOLIC(__NR_rctl_get_racct)
#define __NR_rctl_get_rules          SYMBOLIC(__NR_rctl_get_rules)
#define __NR_rctl_remove_rule        SYMBOLIC(__NR_rctl_remove_rule)
#define __NR_recv                    SYMBOLIC(__NR_recv)
#define __NR_rfork                   SYMBOLIC(__NR_rfork)
#define __NR_rtprio                  SYMBOLIC(__NR_rtprio)
#define __NR_rtprio_thread           SYMBOLIC(__NR_rtprio_thread)
#define __NR_send                    SYMBOLIC(__NR_send)
#define __NR_setaudit                SYMBOLIC(__NR_setaudit)
#define __NR_setcontext              SYMBOLIC(__NR_setcontext)
#define __NR_setfib                  SYMBOLIC(__NR_setfib)
#define __NR_sethostid               SYMBOLIC(__NR_sethostid)
#define __NR_setloginclass           SYMBOLIC(__NR_setloginclass)
#define __NR_sigblock                SYMBOLIC(__NR_sigblock)
#define __NR_sigqueue                SYMBOLIC(__NR_sigqueue)
#define __NR_sigsetmask              SYMBOLIC(__NR_sigsetmask)
#define __NR_sigstack                SYMBOLIC(__NR_sigstack)
#define __NR_sigvec                  SYMBOLIC(__NR_sigvec)
#define __NR_sigwaitinfo             SYMBOLIC(__NR_sigwaitinfo)
#define __NR_sstk                    SYMBOLIC(__NR_sstk)
#define __NR_swapcontext             SYMBOLIC(__NR_swapcontext)
#define __NR_thr_create              SYMBOLIC(__NR_thr_create)
#define __NR_thr_exit                SYMBOLIC(__NR_thr_exit)
#define __NR_thr_kill                SYMBOLIC(__NR_thr_kill)
#define __NR_thr_kill2               SYMBOLIC(__NR_thr_kill2)
#define __NR_thr_new                 SYMBOLIC(__NR_thr_new)
#define __NR_thr_self                SYMBOLIC(__NR_thr_self)
#define __NR_thr_set_name            SYMBOLIC(__NR_thr_set_name)
#define __NR_thr_suspend             SYMBOLIC(__NR_thr_suspend)
#define __NR_thr_wake                SYMBOLIC(__NR_thr_wake)
#define __NR_uuidgen                 SYMBOLIC(__NR_uuidgen)
#define __NR_vadvise                 SYMBOLIC(__NR_vadvise)
#define __NR_wait                    SYMBOLIC(__NR_wait)
#define __NR_wait6                   SYMBOLIC(__NR_wait6)
#define __NR_yield                   SYMBOLIC(__NR_yield)
#define __NR_tfork                   SYMBOLIC(__NR_tfork)
#define __NR_thrsleep                SYMBOLIC(__NR_thrsleep)
#define __NR_thrwakeup               SYMBOLIC(__NR_thrwakeup)
#define __NR_threxit                 SYMBOLIC(__NR_threxit)
#define __NR_thrsigdivert            SYMBOLIC(__NR_thrsigdivert)
#define __NR_set_tcb                 SYMBOLIC(__NR_set_tcb)
#define __NR_get_tcb                 SYMBOLIC(__NR_get_tcb)
#define __NR_adjfreq                 SYMBOLIC(__NR_adjfreq)
#define __NR_getdtablecount          SYMBOLIC(__NR_getdtablecount)
#define __NR_getlogin_r              SYMBOLIC(__NR_getlogin_r)
#define __NR_getrtable               SYMBOLIC(__NR_getrtable)
#define __NR_getthrid                SYMBOLIC(__NR_getthrid)
#define __NR_kbind                   SYMBOLIC(__NR_kbind)
#define __NR_mquery                  SYMBOLIC(__NR_mquery)
#define __NR_obreak                  SYMBOLIC(__NR_obreak)
#define __NR_sendsyslog              SYMBOLIC(__NR_sendsyslog)
#define __NR_setrtable               SYMBOLIC(__NR_setrtable)
#define __NR_swapctl                 SYMBOLIC(__NR_swapctl)
#define __NR_thrkill                 SYMBOLIC(__NR_thrkill)
#define __NR_unveil                  SYMBOLIC(__NR_unveil)
#define __NR_mac_get_link            SYMBOLIC(__NR_mac_get_link)
#define __NR_mac_set_link            SYMBOLIC(__NR_mac_set_link)
#define __NR_mac_get_fd              SYMBOLIC(__NR_mac_get_fd)
#define __NR_mac_get_file            SYMBOLIC(__NR_mac_get_file)
#define __NR_mac_get_proc            SYMBOLIC(__NR_mac_get_proc)
#define __NR_mac_set_fd              SYMBOLIC(__NR_mac_set_fd)
#define __NR_mac_get_pid             SYMBOLIC(__NR_mac_get_pid)
#define __NR_mac_set_proc            SYMBOLIC(__NR_mac_set_proc)
#define __NR_mac_set_file            SYMBOLIC(__NR_mac_set_file)
#define __NR_mac_execve              SYMBOLIC(__NR_mac_execve)
#define __NR_acl_get_link            SYMBOLIC(__NR_acl_get_link)
#define __NR_sigwait_nocancel        SYMBOLIC(__NR_sigwait_nocancel)
#define __NR_cap_rights_get          SYMBOLIC(__NR_cap_rights_get)
#define __NR_semwait_signal          SYMBOLIC(__NR_semwait_signal)
#define __NR_acl_set_link            SYMBOLIC(__NR_acl_set_link)
#define __NR_acl_set_fd              SYMBOLIC(__NR_acl_set_fd)
#define __NR_old_semwait_signal      SYMBOLIC(__NR_old_semwait_signal)
#define __NR_setugid                 SYMBOLIC(__NR_setugid)
#define __NR_acl_aclcheck_fd         SYMBOLIC(__NR_acl_aclcheck_fd)
#define __NR_acl_get_fd              SYMBOLIC(__NR_acl_get_fd)
#define __NR___sysctl                SYMBOLIC(__NR___sysctl)
#define __NR_mac_getfsstat           SYMBOLIC(__NR_mac_getfsstat)
#define __NR_mac_get_mount           SYMBOLIC(__NR_mac_get_mount)
#define __NR_acl_delete_link         SYMBOLIC(__NR_acl_delete_link)
#define __NR_mac_mount               SYMBOLIC(__NR_mac_mount)
#define __NR_acl_get_file            SYMBOLIC(__NR_acl_get_file)
#define __NR_acl_aclcheck_file       SYMBOLIC(__NR_acl_aclcheck_file)
#define __NR_acl_delete_fd           SYMBOLIC(__NR_acl_delete_fd)
#define __NR_acl_aclcheck_link       SYMBOLIC(__NR_acl_aclcheck_link)
#define __NR___mac_syscall           SYMBOLIC(__NR___mac_syscall)
#define __NR_acl_set_file            SYMBOLIC(__NR_acl_set_file)
#define __NR_acl_delete_file         SYMBOLIC(__NR_acl_delete_file)
#define __NR_syscall                 SYMBOLIC(__NR_syscall)
#define __NR__umtx_op                SYMBOLIC(__NR__umtx_op)
#define __NR_semwait_signal_nocancel SYMBOLIC(__NR_semwait_signal_nocancel)
#define __NR_old_semwait_signal_nocancel \
  SYMBOLIC(__NR_old_semwait_signal_nocancel)
#define __NR_sctp_peeloff             SYMBOLIC(__NR_sctp_peeloff)
#define __NR_sctp_generic_recvmsg     SYMBOLIC(__NR_sctp_generic_recvmsg)
#define __NR_sctp_generic_sendmsg     SYMBOLIC(__NR_sctp_generic_sendmsg)
#define __NR_sctp_generic_sendmsg_iov SYMBOLIC(__NR_sctp_generic_sendmsg_iov)
#define __NR_shared_region_map_and_slide_np \
  SYMBOLIC(__NR_shared_region_map_and_slide_np)
#define __NR_guarded_open_dprotected_np \
  SYMBOLIC(__NR_guarded_open_dprotected_np)
#define __NR_stack_snapshot_with_config \
  SYMBOLIC(__NR_stack_snapshot_with_config)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long __NR_exit;
hidden extern const long __NR_exit_group;
hidden extern const long __NR_read;
hidden extern const long __NR_write;
hidden extern const long __NR_open;
hidden extern const long __NR_close;
hidden extern const long __NR_stat;
hidden extern const long __NR_fstat;
hidden extern const long __NR_lstat;
hidden extern const long __NR_poll;
hidden extern const long __NR_ppoll;
hidden extern const long __NR_lseek;
hidden extern const long __NR_mmap;
hidden extern const long __NR_msync;
hidden extern const long __NR_mprotect;
hidden extern const long __NR_munmap;
hidden extern const long __NR_sigaction;
hidden extern const long __NR_sigprocmask;
hidden extern const long __NR_ioctl;
hidden extern const long __NR_pread;
hidden extern const long __NR_pwrite;
hidden extern const long __NR_readv;
hidden extern const long __NR_writev;
hidden extern const long __NR_access;
hidden extern const long __NR_pipe;
hidden extern const long __NR_select;
hidden extern const long __NR_pselect;
hidden extern const long __NR_pselect6;
hidden extern const long __NR_sched_yield;
hidden extern const long __NR_mremap;
hidden extern const long __NR_mincore;
hidden extern const long __NR_madvise;
hidden extern const long __NR_shmget;
hidden extern const long __NR_shmat;
hidden extern const long __NR_shmctl;
hidden extern const long __NR_dup;
hidden extern const long __NR_dup2;
hidden extern const long __NR_pause;
hidden extern const long __NR_nanosleep;
hidden extern const long __NR_getitimer;
hidden extern const long __NR_setitimer;
hidden extern const long __NR_alarm;
hidden extern const long __NR_getpid;
hidden extern const long __NR_sendfile;
hidden extern const long __NR_socket;
hidden extern const long __NR_connect;
hidden extern const long __NR_accept;
hidden extern const long __NR_sendto;
hidden extern const long __NR_recvfrom;
hidden extern const long __NR_sendmsg;
hidden extern const long __NR_recvmsg;
hidden extern const long __NR_shutdown;
hidden extern const long __NR_bind;
hidden extern const long __NR_listen;
hidden extern const long __NR_getsockname;
hidden extern const long __NR_getpeername;
hidden extern const long __NR_socketpair;
hidden extern const long __NR_setsockopt;
hidden extern const long __NR_getsockopt;
hidden extern const long __NR_fork;
hidden extern const long __NR_vfork;
hidden extern const long __NR_posix_spawn;
hidden extern const long __NR_execve;
hidden extern const long __NR_wait4;
hidden extern const long __NR_kill;
hidden extern const long __NR_killpg;
hidden extern const long __NR_clone;
hidden extern const long __NR_tkill;
hidden extern const long __NR_futex;
hidden extern const long __NR_set_robust_list;
hidden extern const long __NR_get_robust_list;
hidden extern const long __NR_uname;
hidden extern const long __NR_semget;
hidden extern const long __NR_semop;
hidden extern const long __NR_semctl;
hidden extern const long __NR_shmdt;
hidden extern const long __NR_msgget;
hidden extern const long __NR_msgsnd;
hidden extern const long __NR_msgrcv;
hidden extern const long __NR_msgctl;
hidden extern const long __NR_fcntl;
hidden extern const long __NR_flock;
hidden extern const long __NR_fsync;
hidden extern const long __NR_fdatasync;
hidden extern const long __NR_truncate;
hidden extern const long __NR_ftruncate;
hidden extern const long __NR_getcwd;
hidden extern const long __NR_chdir;
hidden extern const long __NR_fchdir;
hidden extern const long __NR_rename;
hidden extern const long __NR_mkdir;
hidden extern const long __NR_rmdir;
hidden extern const long __NR_creat;
hidden extern const long __NR_link;
hidden extern const long __NR_unlink;
hidden extern const long __NR_symlink;
hidden extern const long __NR_readlink;
hidden extern const long __NR_chmod;
hidden extern const long __NR_fchmod;
hidden extern const long __NR_chown;
hidden extern const long __NR_fchown;
hidden extern const long __NR_lchown;
hidden extern const long __NR_umask;
hidden extern const long __NR_gettimeofday;
hidden extern const long __NR_getrlimit;
hidden extern const long __NR_getrusage;
hidden extern const long __NR_sysinfo;
hidden extern const long __NR_times;
hidden extern const long __NR_ptrace;
hidden extern const long __NR_syslog;
hidden extern const long __NR_getuid;
hidden extern const long __NR_getgid;
hidden extern const long __NR_getppid;
hidden extern const long __NR_getpgrp;
hidden extern const long __NR_setsid;
hidden extern const long __NR_getsid;
hidden extern const long __NR_getpgid;
hidden extern const long __NR_setpgid;
hidden extern const long __NR_geteuid;
hidden extern const long __NR_getegid;
hidden extern const long __NR_getgroups;
hidden extern const long __NR_setgroups;
hidden extern const long __NR_setreuid;
hidden extern const long __NR_setregid;
hidden extern const long __NR_setuid;
hidden extern const long __NR_setgid;
hidden extern const long __NR_setresuid;
hidden extern const long __NR_setresgid;
hidden extern const long __NR_getresuid;
hidden extern const long __NR_getresgid;
hidden extern const long __NR_sigpending;
hidden extern const long __NR_sigsuspend;
hidden extern const long __NR_sigaltstack;
hidden extern const long __NR_mknod;
hidden extern const long __NR_mknodat;
hidden extern const long __NR_mkfifo;
hidden extern const long __NR_mkfifoat;
hidden extern const long __NR_statfs;
hidden extern const long __NR_fstatfs;
hidden extern const long __NR_getpriority;
hidden extern const long __NR_setpriority;
hidden extern const long __NR_mlock;
hidden extern const long __NR_munlock;
hidden extern const long __NR_mlockall;
hidden extern const long __NR_munlockall;
hidden extern const long __NR_setrlimit;
hidden extern const long __NR_chroot;
hidden extern const long __NR_sync;
hidden extern const long __NR_acct;
hidden extern const long __NR_settimeofday;
hidden extern const long __NR_mount;
hidden extern const long __NR_reboot;
hidden extern const long __NR_quotactl;
hidden extern const long __NR_setfsuid;
hidden extern const long __NR_setfsgid;
hidden extern const long __NR_capget;
hidden extern const long __NR_capset;
hidden extern const long __NR_sigtimedwait;
hidden extern const long __NR_rt_sigqueueinfo;
hidden extern const long __NR_personality;
hidden extern const long __NR_ustat;
hidden extern const long __NR_sysfs;
hidden extern const long __NR_sched_setparam;
hidden extern const long __NR_sched_getparam;
hidden extern const long __NR_sched_setscheduler;
hidden extern const long __NR_sched_getscheduler;
hidden extern const long __NR_sched_get_priority_max;
hidden extern const long __NR_sched_get_priority_min;
hidden extern const long __NR_sched_rr_get_interval;
hidden extern const long __NR_vhangup;
hidden extern const long __NR_modify_ldt;
hidden extern const long __NR_pivot_root;
hidden extern const long __NR__sysctl;
hidden extern const long __NR_prctl;
hidden extern const long __NR_arch_prctl;
hidden extern const long __NR_adjtimex;
hidden extern const long __NR_umount2;
hidden extern const long __NR_swapon;
hidden extern const long __NR_swapoff;
hidden extern const long __NR_sethostname;
hidden extern const long __NR_setdomainname;
hidden extern const long __NR_iopl;
hidden extern const long __NR_ioperm;
hidden extern const long __NR_init_module;
hidden extern const long __NR_delete_module;
hidden extern const long __NR_gettid;
hidden extern const long __NR_readahead;
hidden extern const long __NR_setxattr;
hidden extern const long __NR_fsetxattr;
hidden extern const long __NR_getxattr;
hidden extern const long __NR_fgetxattr;
hidden extern const long __NR_listxattr;
hidden extern const long __NR_flistxattr;
hidden extern const long __NR_removexattr;
hidden extern const long __NR_fremovexattr;
hidden extern const long __NR_lsetxattr;
hidden extern const long __NR_lgetxattr;
hidden extern const long __NR_llistxattr;
hidden extern const long __NR_lremovexattr;
hidden extern const long __NR_sched_setaffinity;
hidden extern const long __NR_sched_getaffinity;
hidden extern const long __NR_cpuset_getaffinity;
hidden extern const long __NR_cpuset_setaffinity;
hidden extern const long __NR_io_setup;
hidden extern const long __NR_io_destroy;
hidden extern const long __NR_io_getevents;
hidden extern const long __NR_io_submit;
hidden extern const long __NR_io_cancel;
hidden extern const long __NR_lookup_dcookie;
hidden extern const long __NR_epoll_create;
hidden extern const long __NR_epoll_wait;
hidden extern const long __NR_epoll_ctl;
hidden extern const long __NR_getdents;
hidden extern const long __NR_set_tid_address;
hidden extern const long __NR_restart_syscall;
hidden extern const long __NR_semtimedop;
hidden extern const long __NR_fadvise;
hidden extern const long __NR_timer_create;
hidden extern const long __NR_timer_settime;
hidden extern const long __NR_timer_gettime;
hidden extern const long __NR_timer_getoverrun;
hidden extern const long __NR_timer_delete;
hidden extern const long __NR_clock_settime;
hidden extern const long __NR_clock_gettime;
hidden extern const long __NR_clock_getres;
hidden extern const long __NR_clock_nanosleep;
hidden extern const long __NR_tgkill;
hidden extern const long __NR_mbind;
hidden extern const long __NR_set_mempolicy;
hidden extern const long __NR_get_mempolicy;
hidden extern const long __NR_mq_open;
hidden extern const long __NR_mq_unlink;
hidden extern const long __NR_mq_timedsend;
hidden extern const long __NR_mq_timedreceive;
hidden extern const long __NR_mq_notify;
hidden extern const long __NR_mq_getsetattr;
hidden extern const long __NR_kexec_load;
hidden extern const long __NR_waitid;
hidden extern const long __NR_add_key;
hidden extern const long __NR_request_key;
hidden extern const long __NR_keyctl;
hidden extern const long __NR_ioprio_set;
hidden extern const long __NR_ioprio_get;
hidden extern const long __NR_inotify_init;
hidden extern const long __NR_inotify_add_watch;
hidden extern const long __NR_inotify_rm_watch;
hidden extern const long __NR_openat;
hidden extern const long __NR_mkdirat;
hidden extern const long __NR_fchownat;
hidden extern const long __NR_utime;
hidden extern const long __NR_utimes;
hidden extern const long __NR_futimesat;
hidden extern const long __NR_futimes;
hidden extern const long __NR_futimens;
hidden extern const long __NR_fstatat;
hidden extern const long __NR_unlinkat;
hidden extern const long __NR_renameat;
hidden extern const long __NR_linkat;
hidden extern const long __NR_symlinkat;
hidden extern const long __NR_readlinkat;
hidden extern const long __NR_fchmodat;
hidden extern const long __NR_faccessat;
hidden extern const long __NR_unshare;
hidden extern const long __NR_splice;
hidden extern const long __NR_tee;
hidden extern const long __NR_sync_file_range;
hidden extern const long __NR_vmsplice;
hidden extern const long __NR_migrate_pages;
hidden extern const long __NR_move_pages;
hidden extern const long __NR_preadv;
hidden extern const long __NR_pwritev;
hidden extern const long __NR_utimensat;
hidden extern const long __NR_fallocate;
hidden extern const long __NR_posix_fallocate;
hidden extern const long __NR_accept4;
hidden extern const long __NR_dup3;
hidden extern const long __NR_pipe2;
hidden extern const long __NR_epoll_pwait;
hidden extern const long __NR_epoll_create1;
hidden extern const long __NR_perf_event_open;
hidden extern const long __NR_inotify_init1;
hidden extern const long __NR_rt_tgsigqueueinfo;
hidden extern const long __NR_signalfd;
hidden extern const long __NR_signalfd4;
hidden extern const long __NR_eventfd;
hidden extern const long __NR_eventfd2;
hidden extern const long __NR_timerfd_create;
hidden extern const long __NR_timerfd_settime;
hidden extern const long __NR_timerfd_gettime;
hidden extern const long __NR_recvmmsg;
hidden extern const long __NR_fanotify_init;
hidden extern const long __NR_fanotify_mark;
hidden extern const long __NR_prlimit;
hidden extern const long __NR_name_to_handle_at;
hidden extern const long __NR_open_by_handle_at;
hidden extern const long __NR_clock_adjtime;
hidden extern const long __NR_syncfs;
hidden extern const long __NR_sendmmsg;
hidden extern const long __NR_setns;
hidden extern const long __NR_getcpu;
hidden extern const long __NR_process_vm_readv;
hidden extern const long __NR_process_vm_writev;
hidden extern const long __NR_kcmp;
hidden extern const long __NR_finit_module;
hidden extern const long __NR_sched_setattr;
hidden extern const long __NR_sched_getattr;
hidden extern const long __NR_renameat2;
hidden extern const long __NR_seccomp;
hidden extern const long __NR_getrandom;
hidden extern const long __NR_memfd_create;
hidden extern const long __NR_kexec_file_load;
hidden extern const long __NR_bpf;
hidden extern const long __NR_execveat;
hidden extern const long __NR_userfaultfd;
hidden extern const long __NR_membarrier;
hidden extern const long __NR_mlock2;
hidden extern const long __NR_copy_file_range;
hidden extern const long __NR_preadv2;
hidden extern const long __NR_pwritev2;
hidden extern const long __NR_pkey_mprotect;
hidden extern const long __NR_pkey_alloc;
hidden extern const long __NR_pkey_free;
hidden extern const long __NR_statx;
hidden extern const long __NR_io_pgetevents;
hidden extern const long __NR_rseq;
hidden extern const long __NR_pidfd_send_signal;
hidden extern const long __NR_io_uring_setup;
hidden extern const long __NR_io_uring_enter;
hidden extern const long __NR_io_uring_register;
hidden extern const long __NR_pledge;
hidden extern const long __NR_ktrace;
hidden extern const long __NR_kqueue;
hidden extern const long __NR_kevent;
hidden extern const long __NR_revoke;
hidden extern const long __NR_setlogin;
hidden extern const long __NR_getfh;
hidden extern const long __NR_chflags;
hidden extern const long __NR_getfsstat;
hidden extern const long __NR_nfssvc;
hidden extern const long __NR_adjtime;
hidden extern const long __NR_fchflags;
hidden extern const long __NR_seteuid;
hidden extern const long __NR_setegid;
hidden extern const long __NR_fpathconf;
hidden extern const long __NR_fhopen;
hidden extern const long __NR_unmount;
hidden extern const long __NR_issetugid;
hidden extern const long __NR_minherit;
hidden extern const long __NR_pathconf;
hidden extern const long __NR_sysctl;
hidden extern const long __NR_ntp_adjtime;
hidden extern const long __NR_ntp_gettime;
hidden extern const long __NR_shm_unlink;
hidden extern const long __NR_shm_open;
hidden extern const long __NR_aio_read;
hidden extern const long __NR_aio_suspend;
hidden extern const long __NR_aio_cancel;
hidden extern const long __NR_aio_fsync;
hidden extern const long __NR_aio_error;
hidden extern const long __NR_aio_return;
hidden extern const long __NR_aio_write;
hidden extern const long __NR_aio_waitcomplete;
hidden extern const long __NR_aio_suspend_nocancel;
hidden extern const long __NR_aio_mlock;
hidden extern const long __NR_sigwait;
hidden extern const long __NR_undelete;
hidden extern const long __NR_getlogin;
hidden extern const long __NR_getdtablesize;
hidden extern const long __NR_setauid;
hidden extern const long __NR_audit;
hidden extern const long __NR_auditctl;
hidden extern const long __NR_getaudit_addr;
hidden extern const long __NR_getdirentries;
hidden extern const long __NR_lio_listio;
hidden extern const long __NR_setaudit_addr;
hidden extern const long __NR_getauid;
hidden extern const long __NR_semsys;
hidden extern const long __NR_auditon;
hidden extern const long __NR_msgsys;
hidden extern const long __NR_shmsys;
hidden extern const long __NR_fhstat;
hidden extern const long __NR_chflagsat;
hidden extern const long __NR_profil;
hidden extern const long __NR_fhstatfs;
hidden extern const long __NR_utrace;
hidden extern const long __NR_closefrom;
hidden extern const long __NR_pthread_markcancel;
hidden extern const long __NR_pthread_kill;
hidden extern const long __NR_pthread_fchdir;
hidden extern const long __NR_pthread_sigmask;
hidden extern const long __NR_pthread_chdir;
hidden extern const long __NR_pthread_canceled;
hidden extern const long __NR_disable_threadsignal;
hidden extern const long __NR_abort_with_payload;
hidden extern const long __NR_accept_nocancel;
hidden extern const long __NR_access_extended;
hidden extern const long __NR_audit_session_join;
hidden extern const long __NR_audit_session_port;
hidden extern const long __NR_audit_session_self;
hidden extern const long __NR_bsdthread_create;
hidden extern const long __NR_bsdthread_ctl;
hidden extern const long __NR_bsdthread_register;
hidden extern const long __NR_bsdthread_terminate;
hidden extern const long __NR_change_fdguard_np;
hidden extern const long __NR_chmod_extended;
hidden extern const long __NR_clonefileat;
hidden extern const long __NR_close_nocancel;
hidden extern const long __NR_coalition;
hidden extern const long __NR_coalition_info;
hidden extern const long __NR_connect_nocancel;
hidden extern const long __NR_connectx;
hidden extern const long __NR_copyfile;
hidden extern const long __NR_csops;
hidden extern const long __NR_csops_audittoken;
hidden extern const long __NR_csrctl;
hidden extern const long __NR_delete;
hidden extern const long __NR_disconnectx;
hidden extern const long __NR_exchangedata;
hidden extern const long __NR_fchmod_extended;
hidden extern const long __NR_fclonefileat;
hidden extern const long __NR_fcntl_nocancel;
hidden extern const long __NR_ffsctl;
hidden extern const long __NR_fgetattrlist;
hidden extern const long __NR_fileport_makefd;
hidden extern const long __NR_fileport_makeport;
hidden extern const long __NR_fmount;
hidden extern const long __NR_fs_snapshot;
hidden extern const long __NR_fsctl;
hidden extern const long __NR_fsetattrlist;
hidden extern const long __NR_fstat_extended;
hidden extern const long __NR_fsync_nocancel;
hidden extern const long __NR_getattrlist;
hidden extern const long __NR_getattrlistat;
hidden extern const long __NR_getattrlistbulk;
hidden extern const long __NR_getdirentriesattr;
hidden extern const long __NR_gethostuuid;
hidden extern const long __NR_getsgroups;
hidden extern const long __NR_getwgroups;
hidden extern const long __NR_grab_pgo_data;
hidden extern const long __NR_guarded_close_np;
hidden extern const long __NR_guarded_kqueue_np;
hidden extern const long __NR_guarded_open_np;
hidden extern const long __NR_guarded_pwrite_np;
hidden extern const long __NR_guarded_write_np;
hidden extern const long __NR_guarded_writev_np;
hidden extern const long __NR_identitysvc;
hidden extern const long __NR_initgroups;
hidden extern const long __NR_iopolicysys;
hidden extern const long __NR_kas_info;
hidden extern const long __NR_kdebug_trace;
hidden extern const long __NR_kdebug_trace_string;
hidden extern const long __NR_kdebug_typefilter;
hidden extern const long __NR_kevent_id;
hidden extern const long __NR_kevent_qos;
hidden extern const long __NR_ledger;
hidden extern const long __NR_lstat_extended;
hidden extern const long __NR_memorystatus_control;
hidden extern const long __NR_memorystatus_get_level;
hidden extern const long __NR_microstackshot;
hidden extern const long __NR_mkdir_extended;
hidden extern const long __NR_mkfifo_extended;
hidden extern const long __NR_modwatch;
hidden extern const long __NR_mremap_encrypted;
hidden extern const long __NR_msgrcv_nocancel;
hidden extern const long __NR_msgsnd_nocancel;
hidden extern const long __NR_msync_nocancel;
hidden extern const long __NR_necp_client_action;
hidden extern const long __NR_necp_match_policy;
hidden extern const long __NR_necp_open;
hidden extern const long __NR_necp_session_action;
hidden extern const long __NR_necp_session_open;
hidden extern const long __NR_net_qos_guideline;
hidden extern const long __NR_netagent_trigger;
hidden extern const long __NR_nfsclnt;
hidden extern const long __NR_open_dprotected_np;
hidden extern const long __NR_open_extended;
hidden extern const long __NR_open_nocancel;
hidden extern const long __NR_openat_nocancel;
hidden extern const long __NR_openbyid_np;
hidden extern const long __NR_os_fault_with_payload;
hidden extern const long __NR_peeloff;
hidden extern const long __NR_persona;
hidden extern const long __NR_pid_hibernate;
hidden extern const long __NR_pid_resume;
hidden extern const long __NR_pid_shutdown_sockets;
hidden extern const long __NR_pid_suspend;
hidden extern const long __NR_poll_nocancel;
hidden extern const long __NR_pread_nocancel;
hidden extern const long __NR_proc_info;
hidden extern const long __NR_proc_rlimit_control;
hidden extern const long __NR_proc_trace_log;
hidden extern const long __NR_proc_uuid_policy;
hidden extern const long __NR_process_policy;
hidden extern const long __NR_pselect_nocancel;
hidden extern const long __NR_psynch_cvbroad;
hidden extern const long __NR_psynch_cvclrprepost;
hidden extern const long __NR_psynch_cvsignal;
hidden extern const long __NR_psynch_mutexdrop;
hidden extern const long __NR_psynch_mutexwait;
hidden extern const long __NR_psynch_rw_downgrade;
hidden extern const long __NR_psynch_rw_longrdlock;
hidden extern const long __NR_psynch_rw_rdlock;
hidden extern const long __NR_psynch_rw_unlock;
hidden extern const long __NR_psynch_rw_unlock2;
hidden extern const long __NR_psynch_rw_upgrade;
hidden extern const long __NR_psynch_rw_wrlock;
hidden extern const long __NR_psynch_rw_yieldwrlock;
hidden extern const long __NR_pwrite_nocancel;
hidden extern const long __NR_read_nocancel;
hidden extern const long __NR_readv_nocancel;
hidden extern const long __NR_recvfrom_nocancel;
hidden extern const long __NR_recvmsg_nocancel;
hidden extern const long __NR_recvmsg_x;
hidden extern const long __NR_renameatx_np;
hidden extern const long __NR_searchfs;
hidden extern const long __NR_select_nocancel;
hidden extern const long __NR_sem_close;
hidden extern const long __NR_sem_open;
hidden extern const long __NR_sem_post;
hidden extern const long __NR_sem_trywait;
hidden extern const long __NR_sem_unlink;
hidden extern const long __NR_sem_wait;
hidden extern const long __NR_sem_wait_nocancel;
hidden extern const long __NR_sendmsg_nocancel;
hidden extern const long __NR_sendmsg_x;
hidden extern const long __NR_sendto_nocancel;
hidden extern const long __NR_setattrlist;
hidden extern const long __NR_setattrlistat;
hidden extern const long __NR_setprivexec;
hidden extern const long __NR_setsgroups;
hidden extern const long __NR_settid;
hidden extern const long __NR_settid_with_pid;
hidden extern const long __NR_setwgroups;
hidden extern const long __NR_sfi_ctl;
hidden extern const long __NR_sfi_pidctl;
hidden extern const long __NR_shared_region_check_np;
hidden extern const long __NR_sigsuspend_nocancel;
hidden extern const long __NR_socket_delegate;
hidden extern const long __NR_stat_extended;
hidden extern const long __NR_sysctlbyname;
hidden extern const long __NR_system_override;
hidden extern const long __NR_telemetry;
hidden extern const long __NR_terminate_with_payload;
hidden extern const long __NR_thread_selfcounts;
hidden extern const long __NR_thread_selfid;
hidden extern const long __NR_thread_selfusage;
hidden extern const long __NR_ulock_wait;
hidden extern const long __NR_ulock_wake;
hidden extern const long __NR_umask_extended;
hidden extern const long __NR_usrctl;
hidden extern const long __NR_vfs_purge;
hidden extern const long __NR_vm_pressure_monitor;
hidden extern const long __NR_wait4_nocancel;
hidden extern const long __NR_waitevent;
hidden extern const long __NR_waitid_nocancel;
hidden extern const long __NR_watchevent;
hidden extern const long __NR_work_interval_ctl;
hidden extern const long __NR_workq_kernreturn;
hidden extern const long __NR_workq_open;
hidden extern const long __NR_write_nocancel;
hidden extern const long __NR_writev_nocancel;
hidden extern const long __NR_abort2;
hidden extern const long __NR_afs3_syscall;
hidden extern const long __NR_bindat;
hidden extern const long __NR_break;
hidden extern const long __NR_cap_enter;
hidden extern const long __NR_cap_fcntls_get;
hidden extern const long __NR_cap_fcntls_limit;
hidden extern const long __NR_cap_getmode;
hidden extern const long __NR_cap_ioctls_get;
hidden extern const long __NR_cap_ioctls_limit;
hidden extern const long __NR_cap_rights_limit;
hidden extern const long __NR_clock_getcpuclockid2;
hidden extern const long __NR_connectat;
hidden extern const long __NR_cpuset;
hidden extern const long __NR_cpuset_getdomain;
hidden extern const long __NR_cpuset_getid;
hidden extern const long __NR_cpuset_setdomain;
hidden extern const long __NR_cpuset_setid;
hidden extern const long __NR_eaccess;
hidden extern const long __NR_extattr_delete_fd;
hidden extern const long __NR_extattr_delete_file;
hidden extern const long __NR_extattr_delete_link;
hidden extern const long __NR_extattr_get_fd;
hidden extern const long __NR_extattr_get_file;
hidden extern const long __NR_extattr_get_link;
hidden extern const long __NR_extattr_list_fd;
hidden extern const long __NR_extattr_list_file;
hidden extern const long __NR_extattr_list_link;
hidden extern const long __NR_extattr_set_fd;
hidden extern const long __NR_extattr_set_file;
hidden extern const long __NR_extattr_set_link;
hidden extern const long __NR_extattrctl;
hidden extern const long __NR_fexecve;
hidden extern const long __NR_ffclock_getcounter;
hidden extern const long __NR_ffclock_getestimate;
hidden extern const long __NR_ffclock_setestimate;
hidden extern const long __NR_fhlink;
hidden extern const long __NR_fhlinkat;
hidden extern const long __NR_fhreadlink;
hidden extern const long __NR_getaudit;
hidden extern const long __NR_getcontext;
hidden extern const long __NR_getfhat;
hidden extern const long __NR_gethostid;
hidden extern const long __NR_getkerninfo;
hidden extern const long __NR_getloginclass;
hidden extern const long __NR_getpagesize;
hidden extern const long __NR_gssd_syscall;
hidden extern const long __NR_jail;
hidden extern const long __NR_jail_attach;
hidden extern const long __NR_jail_get;
hidden extern const long __NR_jail_remove;
hidden extern const long __NR_jail_set;
hidden extern const long __NR_kenv;
hidden extern const long __NR_kldfind;
hidden extern const long __NR_kldfirstmod;
hidden extern const long __NR_kldload;
hidden extern const long __NR_kldnext;
hidden extern const long __NR_kldstat;
hidden extern const long __NR_kldsym;
hidden extern const long __NR_kldunload;
hidden extern const long __NR_kldunloadf;
hidden extern const long __NR_kmq_notify;
hidden extern const long __NR_kmq_setattr;
hidden extern const long __NR_kmq_timedreceive;
hidden extern const long __NR_kmq_timedsend;
hidden extern const long __NR_kmq_unlink;
hidden extern const long __NR_ksem_close;
hidden extern const long __NR_ksem_destroy;
hidden extern const long __NR_ksem_getvalue;
hidden extern const long __NR_ksem_init;
hidden extern const long __NR_ksem_open;
hidden extern const long __NR_ksem_post;
hidden extern const long __NR_ksem_timedwait;
hidden extern const long __NR_ksem_trywait;
hidden extern const long __NR_ksem_unlink;
hidden extern const long __NR_ksem_wait;
hidden extern const long __NR_ktimer_create;
hidden extern const long __NR_ktimer_delete;
hidden extern const long __NR_ktimer_getoverrun;
hidden extern const long __NR_ktimer_gettime;
hidden extern const long __NR_ktimer_settime;
hidden extern const long __NR_lchflags;
hidden extern const long __NR_lchmod;
hidden extern const long __NR_lgetfh;
hidden extern const long __NR_lpathconf;
hidden extern const long __NR_lutimes;
hidden extern const long __NR_mac_syscall;
hidden extern const long __NR_modfind;
hidden extern const long __NR_modfnext;
hidden extern const long __NR_modnext;
hidden extern const long __NR_modstat;
hidden extern const long __NR_nfstat;
hidden extern const long __NR_nlm_syscall;
hidden extern const long __NR_nlstat;
hidden extern const long __NR_nmount;
hidden extern const long __NR_nnpfs_syscall;
hidden extern const long __NR_nstat;
hidden extern const long __NR_pdfork;
hidden extern const long __NR_pdgetpid;
hidden extern const long __NR_pdkill;
hidden extern const long __NR_posix_openpt;
hidden extern const long __NR_procctl;
hidden extern const long __NR_psynch_cvwait;
hidden extern const long __NR_quota;
hidden extern const long __NR_rctl_add_rule;
hidden extern const long __NR_rctl_get_limits;
hidden extern const long __NR_rctl_get_racct;
hidden extern const long __NR_rctl_get_rules;
hidden extern const long __NR_rctl_remove_rule;
hidden extern const long __NR_recv;
hidden extern const long __NR_rfork;
hidden extern const long __NR_rtprio;
hidden extern const long __NR_rtprio_thread;
hidden extern const long __NR_send;
hidden extern const long __NR_setaudit;
hidden extern const long __NR_setcontext;
hidden extern const long __NR_setfib;
hidden extern const long __NR_sethostid;
hidden extern const long __NR_setloginclass;
hidden extern const long __NR_sigblock;
hidden extern const long __NR_sigqueue;
hidden extern const long __NR_sigsetmask;
hidden extern const long __NR_sigstack;
hidden extern const long __NR_sigvec;
hidden extern const long __NR_sigwaitinfo;
hidden extern const long __NR_sstk;
hidden extern const long __NR_swapcontext;
hidden extern const long __NR_thr_create;
hidden extern const long __NR_thr_exit;
hidden extern const long __NR_thr_kill;
hidden extern const long __NR_thr_kill2;
hidden extern const long __NR_thr_new;
hidden extern const long __NR_thr_self;
hidden extern const long __NR_thr_set_name;
hidden extern const long __NR_thr_suspend;
hidden extern const long __NR_thr_wake;
hidden extern const long __NR_uuidgen;
hidden extern const long __NR_vadvise;
hidden extern const long __NR_wait;
hidden extern const long __NR_wait6;
hidden extern const long __NR_yield;
hidden extern const long __NR_tfork;
hidden extern const long __NR_thrsleep;
hidden extern const long __NR_thrwakeup;
hidden extern const long __NR_threxit;
hidden extern const long __NR_thrsigdivert;
hidden extern const long __NR_set_tcb;
hidden extern const long __NR_get_tcb;
hidden extern const long __NR_adjfreq;
hidden extern const long __NR_getdtablecount;
hidden extern const long __NR_getlogin_r;
hidden extern const long __NR_getrtable;
hidden extern const long __NR_getthrid;
hidden extern const long __NR_kbind;
hidden extern const long __NR_mquery;
hidden extern const long __NR_obreak;
hidden extern const long __NR_sendsyslog;
hidden extern const long __NR_setrtable;
hidden extern const long __NR_swapctl;
hidden extern const long __NR_thrkill;
hidden extern const long __NR_unveil;
hidden extern const long __NR_mac_get_link;
hidden extern const long __NR_mac_set_link;
hidden extern const long __NR_mac_get_fd;
hidden extern const long __NR_mac_get_file;
hidden extern const long __NR_mac_get_proc;
hidden extern const long __NR_mac_set_fd;
hidden extern const long __NR_mac_get_pid;
hidden extern const long __NR_mac_set_proc;
hidden extern const long __NR_mac_set_file;
hidden extern const long __NR_mac_execve;
hidden extern const long __NR_acl_get_link;
hidden extern const long __NR_sigwait_nocancel;
hidden extern const long __NR_cap_rights_get;
hidden extern const long __NR_semwait_signal;
hidden extern const long __NR_acl_set_link;
hidden extern const long __NR_acl_set_fd;
hidden extern const long __NR_old_semwait_signal;
hidden extern const long __NR_setugid;
hidden extern const long __NR_acl_aclcheck_fd;
hidden extern const long __NR_acl_get_fd;
hidden extern const long __NR___sysctl;
hidden extern const long __NR_mac_getfsstat;
hidden extern const long __NR_mac_get_mount;
hidden extern const long __NR_acl_delete_link;
hidden extern const long __NR_mac_mount;
hidden extern const long __NR_acl_get_file;
hidden extern const long __NR_acl_aclcheck_file;
hidden extern const long __NR_acl_delete_fd;
hidden extern const long __NR_acl_aclcheck_link;
hidden extern const long __NR___mac_syscall;
hidden extern const long __NR_acl_set_file;
hidden extern const long __NR_acl_delete_file;
hidden extern const long __NR_syscall;
hidden extern const long __NR__umtx_op;
hidden extern const long __NR_semwait_signal_nocancel;
hidden extern const long __NR_old_semwait_signal_nocancel;
hidden extern const long __NR_sctp_peeloff;
hidden extern const long __NR_sctp_generic_recvmsg;
hidden extern const long __NR_sctp_generic_sendmsg;
hidden extern const long __NR_sctp_generic_sendmsg_iov;
hidden extern const long __NR_shared_region_map_and_slide_np;
hidden extern const long __NR_guarded_open_dprotected_np;
hidden extern const long __NR_stack_snapshot_with_config;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_NR_H_ */
