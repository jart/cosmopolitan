/*bin/echo   ' -*- mode:sh; indent-tabs-mode:nil; tab-width:8; coding:utf-8 -*-│
│ vi: set noet ft=sh ts=8 sts=8 sw=8 fenc=utf-8                            :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
╚────────────────────────────────────────────────────────────────'>/dev/null #*/
dir=libc/sysv/calls
. libc/sysv/gen.sh

#       The Fifth Bell System Interface, Community Edition     ┌─────────────────────────┐
#	» so many numbers                                      │ legend                  │
#	                                                       ├─────────────────────────┤
#	                             GNU/Systemd┐              │   fff  │ unavailable    │
# 	                                 2.6.18+│              │   800  │ cancellable    │
#	                             Mac OS X┐  │              │   sys_ │ wrapped        │
#	                                15.6+│  │              │ __sys_ │ wrapped twice  │
#	                          FreeBSD┐   │  │              └─────────────────────────┘
#	                              12+│   │  │
#	                       OpenBSD┐  │ ┌─│──│── XnuClass{1:Mach,2:Unix}
#	                          6.4+│  │ │ │  │
#	                     NetBSD┐  │  │ │ │  │
#	                       9.1+│  │  │ │ │  │
#	Symbol                    ┌┴┐┌┴┐┌┴┐│┬┴┐┌┴┐      Arm64	Directives & Commentary
scall	sys_exit		0x00100100120010e7	0x05e	globl hidden # a.k.a. exit_group
scall	sys_read		0x8038038032803800	0x83f	globl hidden
scall	sys_write		0x8048048042804801	0x840	globl hidden
scall	sys_close		0x0060060062006003	0x039	globl hidden
scall	__sys_stat		0x1b7026fff2152004	0x04f	globl hidden # FreeBSD 11→12 fumble; use sys_fstatat(); blocked on Android
scall	__sys_fstat		0x1b80352272153005	0x050	globl hidden # needs __stat2linux()
scall	__sys_lstat		0x1b90280282154006	0xfff	globl hidden # needs __stat2linux(); blocked on Android
scall	__sys_poll		0x8d18fc8d128e6807	0xfff	globl hidden
scall	sys_ppoll		0xfff86da21ffff90f	0x849	globl hidden # consider INTON/INTOFF tutorial in examples/unbourne.c
scall	sys_lseek		0x0c70a61de20c7008	0x03e	globl hidden # netbsd:evilpad, OpenBSD 7.3+
scall	__sys_mmap		0x0c50311dd20c5009	0x0de	globl hidden # netbsd:pad, OpenBSD 7.3+
scall	sys_msync		0x915900841284181a	0x8e3	globl hidden
scall	__sys_mprotect		0x04a04a04a204a00a	0x0e2	globl hidden
scall	__sys_munmap		0x049049049204900b	0x0d7	globl hidden
scall	sys_sigaction		0x15402e1a0202e00d	0x086	globl hidden # rt_sigaction on Lunix; __sigaction_sigtramp() on NetBSD
scall	__sys_sigprocmask	0x125030154214900e	0x087	globl hidden # a.k.a. rt_sigprocmask, openbsd:byvalue, a.k.a. pthread_sigmask
scall	sys_ioctl		0x0360360362036010	0x01d	globl hidden
scall	sys_ioctl_cp		0x8368368362836810	0x81d	globl hidden # intended for TCSBRK
scall	sys_pread		0x8ad8a99db2899811	0x843	globl hidden # a.k.a. pread64; netbsd:pad, OpenBSD 7.3+
scall	sys_pwrite		0x8ae8aa9dc289a812	0x844	globl hidden # a.k.a. pwrite64; netbsd:pad, OpenBSD 7.3+
scall	sys_readv		0x8788788782878813	0x841	globl hidden
scall	sys_writev		0x8798798792879814	0x842	globl hidden
scall	__sys_pipe		0x02a10721e202a016	0x03b	globl hidden # NOTE: pipe2() on FreeBSD and Linux Aarch64; XNU is pipe(void)→eax:edx
scall	sys_select		0x9a184785d285d817	0xfff	globl hidden
scall	sys_pselect		0x9b486ea0a298a90e	0x848	globl hidden # pselect6() on gnu/systemd
scall	sys_sched_yield		0x15e12a14bf25d018	0x07c	globl hidden # select() on XNU (previously swtch() but removed in 12.4)
scall	__sys_mremap		0x19bffffffffff019	0x0d8	globl hidden
scall	sys_mincore		0x04e04e04e204e01b	0x0e8	globl hidden
scall	sys_madvise		0x04b04b04b204b01c	0x0e9	globl hidden
scall	sys_shmget		0x0e71210e7210901d	0x0c2	globl # no wrapper
scall	sys_shmat		0x0e40e40e4210601e	0x0c4	globl # no wrapper
scall	sys_shmctl		0x1bb128200210701f	0x0c3	globl # no wrapper
scall	sys_dup			0x0290290292029020	0x017	globl hidden
scall	sys_dup2		0x05a05a05a205a021	0x018	globl hidden # dup3() on linux aarch64 (doesn't behave same if oldfd==newfd)
scall	sys_pause		0xfffffffffffff022	0xfff	globl hidden
scall	sys_nanosleep		0x9ae85b8f0ffff823	0x865	globl hidden
scall	__sys_clock_nanosleep	0x9ddfff8f4ffff8e6	0x873	globl hidden
scall	sys_getitimer		0x1aa0460562056024	0x066	globl hidden
scall	sys_setitimer		0x1a90450532053026	0x067	globl hidden
scall	sys_alarm		0xfffffffffffff025	0xfff	globl hidden
scall	sys_getpid		0x0140140142014027	0x0ac	globl hidden # netbsd returns ppid in edx
scall	sys_sendfile		0xffffff1892151028	0x047	globl hidden # Linux vs. XNU/BSD ABIs very different
scall	__sys_socket		0x18a0610612061029	0x0c6	globl hidden
scall	__sys_connect		0x862862862286282a	0x8cb	globl hidden
scall	__sys_accept		0x81e81ea1d281e82b	0x8ca	globl hidden # accept4 on freebsd
scall	sys_sendto		0x885885885288582c	0x8ce	globl hidden
scall	sys_recvfrom		0x81d81d81d281d82d	0x8cf	globl hidden
scall	sys_sendmsg		0x81c81c81c281c82e	0x8d3	globl hidden
scall	sys_recvmsg		0x81b81b81b281b82f	0x8d4	globl hidden
scall	sys_shutdown		0x0860860862086030	0x0d2	globl hidden
scall	__sys_bind		0x0680680682068031	0x0c8	globl hidden
scall	sys_listen		0x06a06a06a206a032	0x0c9	globl hidden
scall	__sys_getsockname	0x0200200202020033	0x0cc	globl hidden
scall	__sys_getpeername	0x01f01f08d201f034	0x0cd	globl hidden
scall	__sys_socketpair	0x0870870872087035	0x0c7	globl hidden
scall	sys_setsockopt		0x0690690692069036	0x0d0	globl hidden
scall	sys_getsockopt		0x0760760762076037	0x0d1	globl hidden
scall	__sys_fork		0x0020020022002039	0xfff	globl hidden # xnu needs eax&=~-edx bc eax always holds pid and edx is 0 for parent and 1 for child
#scall	vfork			0x042042042204203a	0xfff	globl        # this syscall is from the moon so we implement it by hand in libc/runtime/vfork.S; probably removed from XNU in 12.5
scall	sys_posix_spawn		0x1daffffff20f4fff	0xfff	globl hidden # good luck figuring out how xnu defines this
scall	__sys_execve		0x03b03b03b203b03b	0x0dd	globl hidden
scall	__sys_wait4		0x9c180b807280783d	0x904	globl hidden
scall	sys_kill		0x02507a025202503e	0x081	globl hidden # kill(pid, sig, 1) b/c xnu
scall	sys_killpg		0x092fff092fffffff	0xfff	globl hidden
scall	sys_clone		0x11fffffffffff038	0x0dc	globl hidden
scall	sys_tkill		0x13e0771b121480c8	0x082	globl hidden # thr_kill() on FreeBSD; _lwp_kill() on NetBSD; thrkill() on OpenBSD where arg3 should be 0 or tcb; __pthread_kill() on XNU
scall	sys_tgkill		0xffffff1e1ffff0ea	0x083	globl hidden # thr_kill2() on FreeBSD
scall	sys_futex		0x0a60531c622030ca	0x062	globl hidden # raises SIGSYS on NetBSD; _umtx_op() on FreeBSD
scall	sys_futex_cp		0x8a68539c62a038ca	0x862	globl hidden # intended for futex wait ops
scall	sys_set_robust_list	0x0a7ffffffffff111	0x063	globl # no wrapper
scall	sys_get_robust_list	0x0a8ffffffffff112	0x064	globl # no wrapper
scall	sys_uname		0x0a4fff0a4ffff03f	0x0a0	globl hidden
scall	sys_semget		0x0dd0dd0dd20ff040	0x0be	globl # no wrapper
scall	sys_semop		0x0de1220de2100041	0x0c1	globl # no wrapper
scall	sys_semctl		0xfff1271fe20fe042	0x0bf	globl # no wrapper
scall	sys_shmdt		0x0e60e60e62108043	0x0c5	globl # no wrapper
scall	sys_msgget		0x0e10e10e12103044	0x0ba	globl # no wrapper
scall	sys_msgsnd		0x8e28e28e22904845	0x8bd	globl # no wrapper
scall	sys_msgrcv		0x8e38e38e32905846	0x8bc	globl # no wrapper
scall	sys_msgctl		0x1bc1291ff2102047	0x0bb	globl # no wrapper
scall	__sys_fcntl		0x05c05c05c205c048	0x019	globl hidden
scall	__sys_fcntl_cp		0x85c85c85c285c848	0x819	globl hidden # intended for F_SETLKW and F_OFD_SETLKW
scall	sys_flock		0x8838838832883849	0x820	globl hidden
scall	sys_fsync		0x85f85f85f285f84a	0x852	globl hidden
scall	sys_fdatasync		0x8f185fa2628bb84b	0x853	globl hidden # fsync() on openbsd
scall	sys_truncate		0x8c88a79df28c884c	0x82d	globl hidden # netbsd:pad, OpenBSD 7.3+
scall	sys_ftruncate		0x8c98a89e028c984d	0x82e	globl hidden # netbsd:pad, OpenBSD 7.3+
scall	sys_getcwd		0x128130146ffff04f	0x011	globl hidden
scall	sys_chdir		0x00c00c00c200c050	0x031	globl hidden
scall	sys_fchdir		0x00d00d00d200d051	0x032	globl hidden
scall	sys_unlink		0x00a00a00a200a057	0x0b5	globl hidden
scall	sys_fchmod		0x07c07c07c207c05b	0x034	globl hidden
scall	sys_fchown		0x07b07b07b207b05d	0x037	globl hidden # @asyncsignalsafe
scall	sys_umask		0x03c03c03c203c05f	0x0a6	globl hidden
scall	sys_getrlimit		0x0c20c20c220c2061	0x0a3	globl hidden
scall	__sys_getrusage		0x1bd0130752075062	0x0a5	globl hidden
scall	sys_sysinfo		0xfffffffffffff063	0x0b3	globl hidden
scall	sys_times		0xfffffffffffff064	0x099	globl hidden
scall	__sys_ptrace		0x01a01a01a201a065	0x075	globl hidden # ptrace() wrapper api is terrible
scall	sys_syslog		0xfffffffffffff067	0x074	globl hidden
scall	sys_getuid		0x0180180182018066	0x0ae	globl hidden
scall	sys_getgid		0x02f02f02f202f068	0x0b0	globl hidden
scall	sys_getppid		0xfff027027202706e	0x0ad	globl hidden # see sys_getpid()→edx for netbsd
scall	sys_setsid		0x0930930932093070	0x09d	globl hidden
scall	sys_getsid		0x11e0ff136213607c	0x09c	globl hidden
scall	sys_getpgid		0x0cf0cf0cf2097079	0x09b	globl hidden
scall	sys_setpgid		0x052052052205206d	0x09a	globl hidden
scall	sys_geteuid		0x019019019201906b	0x0af	globl hidden
scall	sys_getegid		0x02b02b02b202b06c	0x0b1	globl hidden
scall	sys_getgroups		0x04f04f04f204f073	0x09e	globl hidden
scall	sys_setgroups		0x0500500502050074	0x09f	globl hidden
scall	sys_setreuid		0x07e07e07e207e071	0x091	globl hidden
scall	sys_setregid		0x07f07f07f207f072	0x08f	globl hidden
scall	sys_setuid		0x0170170172017069	0x092	globl hidden
scall	sys_setgid		0x0b50b50b520b506a	0x090	globl hidden
scall	sys_setresuid		0xfff11a137ffff075	0x093	globl hidden # polyfilled for xnu
scall	sys_setresgid		0xfff11c138ffff077	0x095	globl hidden # polyfilled for xnu
scall	sys_getresuid		0xfff119168ffff076	0x094	globl hidden # semantics aren't well-defined
scall	sys_getresgid		0xfff11b169ffff078	0x096	globl hidden # semantics aren't well-defined
scall	sys_sigpending		0x124034157203407f	0x088	globl hidden # a.k.a. rt_sigpending on linux
scall	sys_sigsuspend		0x92686f955286f882	0x885	globl hidden # a.k.a. rt_sigsuspend on Linux; openbsd:byvalue, sigsuspend_nocancel on XNU
scall	sys_sigaltstack		0x1191200352035083	0x084	globl hidden
scall	sys_mknod		0x1c200e00e200e085	0xfff	globl hidden
scall	sys_mknodat		0x1cc14022fffff103	0x021	globl # no wrapper; FreeBSD 12+
scall	sys_mkfifo		0x0840840842084fff	0xfff	globl hidden
scall	sys_mkfifoat		0x1cb13f1f1fffffff	0xfff	globl # no wrapper
scall	sys_statfs		0x89d83fa2b2959889	0x82b	globl hidden
scall	sys_fstatfs		0x89e840a2c295a88a	0x82c	globl hidden
scall	sys_getpriority		0x064064064206408c	0x08d	globl hidden
scall	sys_setpriority		0x060060060206008d	0x08c	globl hidden # modern nice()
scall	sys_mlock		0x0cb0cb0cb20cb095	0x0e4	globl # no wrapper
scall	sys_munlock		0x0cc0cc0cc20cc096	0x0e5	globl # no wrapper
scall	sys_mlockall		0x0f210f1442144097	0x0e6	globl # no wrapper
scall	sys_munlockall		0x0f31101452145098	0x0e7	globl # no wrapper
scall	sys_setrlimit		0x0c30c30c320c30a0	0x0a4	globl hidden
scall	sys_chroot		0x03d03d03d203d0a1	0x033	globl hidden
scall	sys_sync		0x02402402420240a2	0x051	globl hidden
scall	sys_acct		0x03303303320330a3	0x059	globl # no wrapper
scall	sys_settimeofday	0x1a304407a207a0a4	0x0aa	globl # no wrapper
scall	sys_mount		0x19a01501520a70a5	0x028	globl hidden
scall	sys_unmount		0x016016016209f0a6	0xfff	globl # no wrapper; umount2() on linux
scall	sys_umount2		0x016016016209f0a6	0x027	globl # no wrapper; unmount() on bsd
scall	sys_reboot		0x0d003703720370a9	0x08e	globl hidden # two arguments b/c netbsd/sparc lool
scall	sys_quotactl		0xfff09409420a50b3	0x03c	globl # no wrapper
scall	sys_setfsuid		0xfffffffffffff07a	0x097	globl hidden
scall	sys_setfsgid		0xfffffffffffff07b	0x098	globl hidden
scall	sys_capget		0xfffffffffffff07d	0x05a	globl # no wrapper
scall	sys_capset		0xfffffffffffff07e	0x05b	globl # no wrapper
scall	sys_sigtimedwait	0x9affff959ffff880	0x889	globl hidden
scall	sys_sigqueue		0xffffff1c8fffffff	0xfff	globl hidden
scall	sys_sigqueueinfo	0x0f5ffffffffff081	0x08a	globl hidden # a.k.a. rt_sigqueueinfo on linux
scall	sys_personality		0xfffffffffffff087	0x05c	globl # no wrapper
scall	sys_ustat		0xfffffffffffff088	0xfff	globl # no wrapper
scall	sys_sysfs		0xfffffffffffff08b	0xfff	globl # no wrapper
scall	sys_sched_setparam	0x15afff147ffff08e	0x076	globl hidden
scall	sys_sched_getparam	0x15bfff148ffff08f	0x079	globl hidden
scall	sys_sched_setscheduler	0xffffff149ffff090	0x077	globl hidden
scall	sys_sched_getscheduler	0xffffff14affff091	0x078	globl hidden
scall	sys_sched_setaffinity	0x15cfff1e8ffff0cb	0x07a	globl hidden # hairy; cpuset_setaffinity on FreeBSD
scall	sys_sched_getaffinity	0x15dfff1e7ffff0cc	0x07b	globl hidden # hairy; cpuset_getaffinity on FreeBSD
scall	sys_sched_get_priority_max 0xffffff14cffff092	0x07d	globl hidden
scall	sys_sched_get_priority_min 0xffffff14dffff093	0x07e	globl hidden
scall	sys_sched_rr_get_interval 0xffffff14effff094	0x07f	globl hidden
scall	sys_vhangup		0xfffffffffffff099	0x03a	globl # no wrapper
scall	sys_modify_ldt		0xfffffffffffff09a	0xfff	globl # no wrapper
scall	sys_pivot_root		0xfffffffffffff09b	0x029	globl hidden
#scall	prctl			0xfffffffffffff09d	0x0a7	globl # wrapped manually
scall	sys_arch_prctl		0x0a50a50a5ffff09e	0xfff	globl hidden # sysarch() on bsd
scall	sys_adjtimex		0xfffffffffffff09f	0x0ab	globl # no wrapper
scall	sys_swapon		0xffffff05520550a7	0x0e0	globl # no wrapper
scall	sys_swapoff		0xffffff1a8ffff0a8	0x0e1	globl # no wrapper
scall	sys_sethostname		0xffffff058ffff0aa	0x0a1	globl # no wrapper
scall	sys_setdomainname	0xffffff0a3ffff0ab	0x0a2	globl # no wrapper
scall	sys_iopl		0xfffffffffffff0ac	0xfff	globl # no wrapper
scall	sys_ioperm		0xfffffffffffff0ad	0xfff	globl # no wrapper
scall	sys_init_module		0xfffffffffffff0af	0x069	globl # no wrapper
scall	sys_delete_module	0xfffffffffffff0b0	0x06a	globl # no wrapper
scall	__sys_gettid		0x13712b1b0101b0ba	0x0b2	globl hidden # thread_self_trap (or gettid? on xnu), _lwp_self on netbsd, thr_self on freebsd, getthrid on openbsd
scall	sys_readahead		0xfffffffffffff0bb	0x0d5	globl # no wrapper; consider fadvise() / madvise()
scall	sys_setxattr		0x177ffffff20ec0bc	0x005	globl # no wrapper
scall	sys_fsetxattr		0x179ffffff20ed0be	0x007	globl # no wrapper
scall	sys_getxattr		0x17affffff20ea0bf	0x008	globl # no wrapper
scall	sys_fgetxattr		0x17cffffff20eb0c1	0x00a	globl # no wrapper
scall	sys_listxattr		0x17dffffff20f00c2	0x00b	globl # no wrapper
scall	sys_flistxattr		0x17fffffff20f10c4	0x00d	globl # no wrapper
scall	sys_removexattr		0x180ffffff20ee0c5	0x00e	globl # no wrapper
scall	sys_fremovexattr	0x182ffffff20ef0c7	0x010	globl # no wrapper
scall	sys_lsetxattr		0x178ffffffffff0bd	0x006	globl # no wrapper
scall	sys_lgetxattr		0x17bffffffffff0c0	0x009	globl # no wrapper
scall	sys_llistxattr		0x17effffffffff0c3	0x00c	globl # no wrapper
scall	sys_lremovexattr	0x181ffffffffff0c6	0x00f	globl # no wrapper
scall	sys_io_setup		0xfffffffffffff0ce	0x000	globl # no wrapper
scall	sys_io_destroy		0xfffffffffffff0cf	0x001	globl # no wrapper
scall	sys_io_getevents	0xfffffffffffff0d0	0x004	globl # no wrapper
scall	sys_io_submit		0xfffffffffffff0d1	0x002	globl # no wrapper
scall	sys_io_cancel		0xfffffffffffff0d2	0x003	globl # no wrapper
scall	sys_lookup_dcookie	0xfffffffffffff0d4	0x012	globl # no wrapper
scall	sys_epoll_create	0xfffffffffffff0d5	0xfff	globl hidden
scall	sys_epoll_wait		0xfffffffffffff8e8	0xfff	globl hidden
scall	sys_epoll_ctl		0xfffffffffffff0e9	0x015	globl hidden
scall	sys_getdents		0x18606311020c40d9	0x03d	globl hidden # use opendir/readdir; four args b/c xnu, getdirentries on xnu, 32-bit on xnu/freebsd, a.k.a. getdents64 on linux, 64-bit on openbsd
scall	sys_set_tid_address	0xfffffffffffff0da	0x060	globl # no wrapper
scall	sys_restart_syscall	0xfffffffffffff0db	0x080	globl # no wrapper
scall	sys_semtimedop		0xfffffffffffff0dc	0x0c0	globl # no wrapper
scall	sys_fadvise		0x1a0fff213ffff0dd	0x0df	globl hidden
scall	sys_timer_create	0x0ebffffffffff0de	0x06b	globl # no wrapper
scall	sys_timer_settime	0x1beffffffffff0df	0x06e	globl # no wrapper
scall	sys_timer_gettime	0x1bfffffffffff0e0	0x06c	globl # no wrapper
scall	sys_timer_getoverrun	0x0efffffffffff0e1	0x06d	globl # no wrapper
scall	sys_timer_delete	0x0ecffffffffff0e2	0x06f	globl # no wrapper
scall	sys_ktimer_create	0xffffff0ebfffffff	0xfff	globl # no wrapper
scall	sys_ktimer_delete	0xffffff0ecfffffff	0xfff	globl # no wrapper
scall	sys_ktimer_getoverrun	0xffffff0effffffff	0xfff	globl # no wrapper
scall	sys_ktimer_gettime	0xffffff0eefffffff	0xfff	globl # no wrapper
scall	sys_ktimer_settime	0xffffff0edfffffff	0xfff	globl # no wrapper
scall	sys_clock_settime	0x1ac0580e9ffff0e3	0x070	globl hidden # no wrapper
scall	sys_clock_getres	0x1ad0590eaffff0e5	0x072	globl hidden
scall	sys_mbind		0xfffffffffffff0ed	0x0eb	globl # no wrapper; numa numa yeah
scall	set_mempolicy		0xfffffffffffff0ee	0x0ed	globl
scall	get_mempolicy		0xfffffffffffff0ef	0x0ec	globl
scall	sys_mq_open		0x101ffffffffff0f0	0x0b4	globl # won't polyfill
scall	sys_mq_unlink		0x103ffffffffff0f1	0x0b5	globl # won't polyfill
scall	sys_mq_timedsend	0x1b0ffffffffff0f2	0x0b6	globl # won't polyfill
scall	sys_mq_timedreceive	0x1b1ffffffffff0f3	0x0b7	globl # won't polyfill
scall	sys_mq_notify		0x106ffffffffff0f4	0x0b8	globl # won't polyfill
scall	sys_mq_getsetattr	0xfffffffffffff0f5	0x0b9	globl # won't polyfill
scall	sys_kexec_load		0xfffffffffffff0f6	0x068	globl # no wrapper
scall	sys_waitid		0xfffffffff28ad8f7	0x05f	globl # Linux 2.6.9+; no wrapper
scall	sys_add_key		0xfffffffffffff0f8	0x0d9	globl # no wrapper
scall	sys_request_key		0xfffffffffffff0f9	0x0da	globl # no wrapper
scall	sys_keyctl		0xfffffffffffff0fa	0x0db	globl # no wrapper
scall	sys_ioprio_set		0xfffffffffffff0fb	0x01e	globl
scall	sys_ioprio_get		0xfffffffffffff0fc	0x01f	globl
scall	sys_inotify_init	0xfffffffffffff0fd	0xfff	globl # no wrapper
scall	sys_inotify_add_watch	0xfffffffffffff0fe	0xfff	globl # no wrapper
scall	sys_inotify_rm_watch	0xfffffffffffff0ff	0xfff	globl # no wrapper
scall	__sys_openat		0x9d49419f329cf901	0x838	globl hidden # Linux 2.6.16+ (c. 2007)
scall	__sys_openat_nc		0x1d41411f321d0101	0x038	globl hidden # openat_nocancel() on xnu
scall	sys_mkdirat		0x1cd13e1f021db102	0x022	globl hidden
scall	sys_fchownat		0x1d013b1eb21d4104	0x036	globl hidden # @asyncsignalsafe
scall	sys_utime		0xfffffffffffff084	0x062	globl hidden
scall	sys_utimes		0x1a404c08a208a0eb	0x058	globl hidden
scall	sys_futimes		0x1a704d0ce208bfff	0xfff	globl hidden
scall	sys_futimens		0x1d8055222fffffff	0xfff	globl hidden
scall	__sys_fstatat		0x1d202a22821d6106	0x04f	globl hidden # a.k.a. newfstatat(); FreeBSD 12+; needs __stat2linux()
scall	sys_unlinkat		0x1d71451f721d8107	0x023	globl hidden
scall	sys_renameat		0x1ca1431f521d1108	0x026	globl hidden
scall	sys_linkat		0x1c913d1ef21d7109	0x025	globl hidden
scall	sys_symlinkat		0x1d61441f621da10a	0x024	globl hidden
scall	sys_readlinkat		0x1d51421f421d910b	0x04e	globl hidden
scall	sys_fchmodat		0x1cf13a1ea21d310c	0x035	globl hidden
scall	sys_faccessat		0x1ce1391e921d210d	0x030	globl hidden
scall	sys_unshare		0xfffffffffffff110	0x061	globl # no wrapper
scall	sys_splice		0xfffffffffffff113	0x04c	globl hidden # Linux 2.6.17+ (c. 2007)
scall	sys_tee			0xfffffffffffff114	0x04d	globl        # Linux 2.6.17+; no wrapper
scall	sys_vmsplice		0xfffffffffffff116	0x04b	globl hidden
scall	sys_migrate_pages	0xfffffffffffff100	0x0ee	globl        # no wrapper; numa numa yay
scall	sys_move_pages		0xfffffffffffff117	0x0ef	globl        # no wrapper; NOTE: We view Red Hat versions as "epochs" for all distros.
#──────────────────────RHEL 5.0 LIMIT────────────────────────────────        # ←┬─ last distro with gplv2 licensed compiler c. 2007
scall	sys_preadv		0x9218ab9212a1c927	0x845	globl hidden #  ├─ last distro with system v shell script init
scall	sys_pwritev		0x9228ac9222a1d928	0x846	globl hidden #  ├─ rob landley unleashes busybox gpl lawsuits
scall	__sys_utimensat		0x1d3054223ffff118	0x058	globl hidden #  ├─ python modules need this due to pep513
scall	sys_fallocate		0xfffffffffffff91d	0x02f	globl        #  ├─ end of life 2020-11-30 (extended)
scall	sys_posix_fallocate	0x9dffffa12fffffff	0xfff	globl hidden #  └─ cosmopolitan supports rhel5+
scall	__sys_accept4		0xfff85da1dffff920	0x8f2	globl hidden # Linux 2.6.28+
scall	__sys_dup3		0x1c6066fffffff124	0x018	globl hidden # Linux 2.6.27+
scall	__sys_pipe2		0x1c506521effff125	0x03b	globl hidden # Linux 2.6.27+
scall	sys_epoll_pwait		0xfffffffffffff919	0x816	globl hidden
scall	sys_epoll_create1	0xfffffffffffff123	0x014	globl hidden
scall	sys_perf_event_open	0xfffffffffffff12a	0x0f1	globl # no wrapper
scall	sys_inotify_init1	0xfffffffffffff126	0x01a	globl # no wrapper
scall	sys_tgsigqueueinfo	0xfffffffffffff129	0x0f0	globl # no wrapper; officially rt_tgsigqueueinfo on gnu/systemd
scall	sys_signalfd		0xfffffffffffff11a	0xfff	globl # no wrapper
scall	sys_signalfd4		0xfffffffffffff121	0x04a	globl # no wrapper
scall	sys_eventfd		0xfffffffffffff11c	0xfff	globl # no wrapper
scall	sys_eventfd2		0xfffffffffffff122	0x013	globl # no wrapper
scall	sys_timerfd_create	0xfffffffffffff11b	0x055	globl # no wrapper
scall	sys_timerfd_settime	0xfffffffffffff11e	0x056	globl # no wrapper
scall	sys_timerfd_gettime	0xfffffffffffff11f	0x057	globl # no wrapper
#──────────────────────RHEL 6.0 LIMIT──────────────────────────────── # ←┬─ modern glibc needs rhel6+ c. 2011
scall	sys_recvmmsg		0x1dbffffffffff12b	0x0f3	globl #  ├─ end of life 2024-06-30 (extended)
scall	sys_fanotify_init	0xfffffffffffff12c	0x106	globl #  ├─ last distro with the original gnome desktop
scall	sys_fanotify_mark	0xfffffffffffff12d	0x107	globl #  └─ apple and google condemn the gplv3/gccrtev3
scall	sys_prlimit		0xfffffffffffff12e	0x105	globl # a.k.a. prlimit64()
scall	sys_name_to_handle_at	0xfffffffffffff12f	0x108	globl
scall	sys_open_by_handle_at	0xfffffffffffff130	0x109	globl
scall	sys_clock_adjtime	0xfffffffffffff131	0x10a	globl # no wrapper
scall	sys_syncfs		0xfffffffffffff132	0x10b	globl # no wrapper
#scall	sendmmsg		0x1dcffffffffff133	0x10d	globl
scall	sys_setns		0xfffffffffffff134	0x10c	globl # no wrapper
scall	sys_getcpu		0xfffffffffffff135	0x0a8	globl # no wrapper
scall	sys_process_vm_readv	0xfffffffffffff136	0x10e	globl # no wrapper
scall	sys_process_vm_writev	0xfffffffffffff137	0x10f	globl # no wrapper
scall	sys_kcmp		0xfffffffffffff138	0x110	globl # no wrapper
scall	sys_finit_module	0xfffffffffffff139	0x111	globl # no wrapper
#──────────────────────RHEL 7.0 LIMIT──────────────────────────────── # ←┬─ anything that links or uses c++11 code needs rhel7+ c. 2014
scall	sys_sched_setattr	0xfffffffffffff13a	0x112	globl #  ├─ desktop replaced with tablet-first gui inspired by mac os x
scall	sys_sched_getattr	0xfffffffffffff13b	0x113	globl #  ├─ karen sandler requires systemd init and boot for tablet gui
scall	sys_renameat2		0xfffffffffffff13c	0x114	globl #  └─ debian founder ian murdock found strangled with vacuum cord
#scall	seccomp			0xfffffffffffff13d	0x115	globl # wrapped manually
scall	sys_getrandom		0xfff007a3321f493e	0x116	globl hidden  # Linux 3.17+; FreeBSD 12+; NetBSD wut; getentropy() on XNU/OpenBSD
scall	sys_memfd_create	0xfffffffffffff13f	0x117	globl hidden
scall	sys_kexec_file_load	0xfffffffffffff140	0xfff	globl # no wrapper
scall	sys_bpf			0xfffffffffffff141	0x118	globl # no wrapper
scall	sys_execveat		0xfffffffffffff142	0x119	globl # no wrapper
scall	sys_userfaultfd		0xfffffffffffff143	0x11a	globl # no wrapper; Linux 4.3+ (c. 2015)
scall	sys_membarrier		0xfffffffffffff144	0x11b	globl # no wrapper; Linux 4.3+ (c. 2015)
scall	sys_mlock2		0xfffffffffffff145	0x11c	globl # no wrapper; Linux 4.5+ (c. 2016)
scall	sys_copy_file_range	0xffffffa39ffff946	0x91d	globl hidden # Linux 4.5+ (c. 2016), FreeBSD 13+
scall	sys_preadv2		0xfffffffffffff147	0x11e	globl # no wrapper
scall	sys_pwritev2		0xfffffffffffff148	0x11f	globl # no wrapper
scall	sys_pkey_mprotect	0xfffffffffffff149	0x120	globl # no wrapper
scall	sys_pkey_alloc		0xfffffffffffff14a	0x121	globl # no wrapper
scall	sys_pkey_free		0xfffffffffffff14b	0x122	globl # no wrapper
scall	sys_statx		0xfffffffffffff14c	0x123	globl # no wrapper; lool https://lkml.org/lkml/2010/7/22/249
scall	sys_io_pgetevents	0xfffffffffffff14d	0x124	globl # no wrapper
scall	sys_rseq		0xfffffffffffff14e	0x125	globl # no wrapper; Linux 4.18+ (c. 2018)
#──────────────────────LINUX 4.18 LIMIT────────────────────────────── # ←┬─ last version of linux kernel buildable with only gplv2
scall	sys_pidfd_send_signal	0xfffffffffffff1a8	0x1a8	globl #  ├─ linux conferences ban linux founder linus torvalds
scall	sys_io_uring_setup	0xfffffffffffff1a9	0x1a9	globl #  └─ gnu founder richard stallman publicly disgraced
scall	sys_io_uring_enter	0xfffffffffffff1aa	0x1aa	globl
scall	sys_io_uring_register	0xfffffffffffff1ab	0x1ab	globl
#────────────────────────RHEL CLOUD────────────────────────────────── # ←──────┬─ red hat terminates community release of enterprise linux circa 2020
scall	sys_pledge		0xfff06cffffffffff	0xfff	globl hidden # └─ online linux services ban the president of united states of america
scall	sys_msyscall		0xfff025ffffffffff	0xfff	globl # no wrapper
scall	sys_bogus		0x00b5005002500500	0x500	globl
scall	sys_open_tree		0xfffffffffffff1ac	0x1ac	globl # no wrapper
scall	sys_move_mount		0xfffffffffffff1ad	0x1ad	globl # no wrapper
scall	sys_fsopen		0xfffffffffffff1ae	0x1ae	globl # no wrapper
scall	sys_fsconfig		0xfffffffffffff1af	0x1af	globl # no wrapper
scall	sys_fsmount		0xfffffffffffff1b0	0x1b0	globl # no wrapper
scall	sys_fspick		0xfffffffffffff1b1	0x1b1	globl # no wrapper
scall	sys_pidfd_open		0xfffffffffffff1b2	0x1b2	globl # no wrapper
scall	sys_clone3		0xfffffffffffff1b3	0x1b3	globl # no wrapper
scall	sys_close_range		0xffffff23fffff1b4	0x1b4	globl hidden # Linux 5.9
scall	sys_openat2		0xfffffffffffff1b5	0x1b5	globl hidden # Linux 5.6
scall	sys_pidfd_getfd		0xfffffffffffff1b6	0x1b6	globl # no wrapper
scall	sys_faccessat2		0xfffffffffffff1b7	0x1b7	globl hidden
scall	sys_process_madvise	0xfffffffffffff1b8	0x1b8	globl # no wrapper
scall	sys_epoll_pwait2	0xfffffffffffff1b9	0x1b9	globl # no wrapper
scall	sys_mount_setattr	0xfffffffffffff1ba	0x1ba	globl # no wrapper
scall	sys_quotactl_fd		0xfffffffffffff1bb	0xfff	globl # no wrapper
scall	sys_landlock_create_ruleset 0xfffffffffffff1bc	0x1bc	globl hidden # Linux 5.13+
scall	sys_landlock_add_rule	0xfffffffffffff1bd	0x1bd	globl hidden
scall	sys_landlock_restrict_self 0xfffffffffffff1be	0x1be	globl hidden
scall	sys_memfd_secret	0xfffffffffffff1bf	0xfff	globl # no wrapper
scall	sys_process_mrelease	0xfffffffffffff1c0	0xfff	globl # no wrapper
scall	sys_futex_waitv		0xfffffffffffff1c1	0xfff	globl # no wrapper
scall	sys_set_mempolicy_home_node 0xfffffffffffff1c2	0xfff	globl # no wrapper
scall	sys_cachestat		0xfffffffffffff1c3	0x1c3	globl # Linux 6.5+
scall	sys_fchmodat2		0xfffffffffffff1c4	0x1c4	globl # no wrapper Linux 6.6+

#	The Fifth Bell System Interface, Community Edition
#	» besiyata dishmaya
#	                             GNU/Systemd┐
#	                             Mac OS X┐  │
#	                          FreeBSD┐   │  │
#	                       OpenBSD┐  │ ┌─│──│── XnuClass{1:Mach,2:Unix}
#	                     NetBSD┐  │  │ │ │  │
#	Symbol                    ┌┴┐┌┴┐┌┴┐│┬┴┐┌┴┐      Arm64	Type	Directives & Commentary
scall	sys_ktrace		0x02d02d02dfffffff	0xfff	globl # no wrapper
scall	sys_kqueue		0x15810d16a216afff	0xfff	globl # no wrapper
scall	sys_kevent		0x1b30482302171fff	0xfff	globl # no wrapper
scall	sys_revoke		0x0380380382038fff	0xfff	globl # no wrapper
scall	sys_setlogin		0xfff0320322032fff	0xfff	globl # no wrapper
scall	sys_getfh		0x18b0a10a120a1fff	0xfff	globl # no wrapper
scall	sys_chflags		0x0220220222022fff	0xfff	globl # no wrapper
scall	sys_getfsstat		0xfff03e22d215bfff	0xfff	globl # no wrapper
scall	sys_nfssvc		0x09b09b09b209bfff	0xfff	globl # no wrapper
scall	sys_adjtime		0x1a508c08c208cfff	0xfff	globl # no wrapper
scall	sys_fchflags		0x0230230232023fff	0xfff	globl # no wrapper
scall	__bsd_seteuid		0xfff0b70b720b7fff	0xfff	globl hidden # wrapped via setreuid()
scall	__bsd_setegid		0xfff0b60b620b6fff	0xfff	globl hidden # wrapped via setregid()
scall	sys_fpathconf		0x0c00c00c020c0fff	0xfff	globl # no wrapper
scall	sys_fhopen		0x18c10812a20f8fff	0xfff	globl # no wrapper
scall	sys_issetugid		0x1310fd0fd2147fff	0xfff	globl hidden
scall	sys_minherit		0x1110fa0fa20fafff	0xfff	globl # no wrapper
scall	sys_pathconf		0x0bf0bf0bf20bffff	0xfff	globl # no wrapper
scall	sys_sysctl		0x0ca0ca0ca20cafff	0xfff	globl # no wrapper
#──────────────────XNU & FREEBSD & NETBSD────────────────────────────────────
scall	sys_sem_init		0x0f7fff194fffffff	0xfff	globl
scall	sys_sem_destroy		0x0fffff198fffffff	0xfff	globl
scall	sys_sem_open		0x0f8fff195210cfff	0xfff	globl
scall	sys_sem_close		0x0fafff190210dfff	0xfff	globl
scall	sys_sem_unlink		0x0f9fff196210efff	0xfff	globl
scall	sys_sem_post		0x0fbfff1912111fff	0xfff	globl
scall	sys_sem_wait		0x8fcfff992290ffff	0xfff	globl
scall	sys_sem_trywait		0x0fdfff1932110fff	0xfff	globl
scall	sys_sem_timedwait	0x900fff9b9fffffff	0xfff	globl
scall	sys_sem_wait_nocancel	0xfffffffff21a4fff	0xfff	globl # no wrapper
scall	sys_sem_getvalue	0x0fefff197fffffff	0xfff	globl
#───────────────────────XNU & FREEBSD────────────────────────────────────────
scall	sys_ntp_adjtime		0x0b0fff0b0220ffff	0xfff	globl # no wrapper
scall	sys_ntp_gettime		0x1c0fff0f82210fff	0xfff	globl # no wrapper
scall	sys_shm_unlink		0xffffff1e3210bfff	0xfff	globl # no wrapper
scall	sys_shm_open		0xffffff1e2210afff	0xfff	globl # no wrapper
scall	sys_aio_read		0x192fff13e213efff	0xfff	globl # no wrapper
scall	sys_aio_suspend		0x1b6fff13b213bfff	0xfff	globl # no wrapper
scall	sys_aio_cancel		0x18ffff13c213cfff	0xfff	globl # no wrapper
scall	sys_aio_fsync		0x191fff1d12139fff	0xfff	globl # no wrapper
scall	sys_aio_error		0x190fff13d213dfff	0xfff	globl # no wrapper
scall	sys_aio_return		0x193fff13a213afff	0xfff	globl # no wrapper
scall	sys_aio_write		0x195fff13f213ffff	0xfff	globl # no wrapper
scall	sys_aio_waitcomplete	0xffffff167fffffff	0xfff	globl # no wrapper
scall	sys_aio_suspend_nocancel 0xfffffffff21a5fff	0xfff	globl # no wrapper
scall	sys_aio_mlock		0xffffff21ffffffff	0xfff	globl # no wrapper
scall	sys_sigwait		0xffffff1ad214afff	0xfff	globl # no wrapper
scall	sys_undelete		0x0cdfff0cd20cdfff	0xfff	globl # no wrapper
scall	sys_getlogin		0x03108d0312031fff	0xfff	globl hidden
scall	sys_getdtablesize	0xffffff0592059fff	0xfff	globl # no wrapper
scall	sys_setauid		0xffffff1c02162fff	0xfff	globl # no wrapper
scall	sys_audit		0xffffff1bd215efff	0xfff	globl # no wrapper
scall	sys_auditctl		0xffffff1c52167fff	0xfff	globl # no wrapper
scall	sys_getaudit_addr	0xffffff1c32165fff	0xfff	globl # no wrapper
scall	sys_getdirentries	0xffffff22a20c4fff	0xfff	globl # xnu getdirentries64 is 0x158
scall	sys_lio_listio		0x196fff1402140fff	0xfff	globl # no wrapper
scall	sys_setaudit_addr	0xffffff1c42166fff	0xfff	globl # no wrapper
scall	sys_getauid		0xffffff1bf2161fff	0xfff	globl # no wrapper
scall	sys_semsys		0xffffff0a920fbfff	0xfff	globl # no wrapper
scall	sys_auditon		0xffffff1be215ffff	0xfff	globl # no wrapper
scall	sys_msgsys		0xffffff0aa20fcfff	0xfff	globl # no wrapper
scall	sys_shmsys		0xffffff0ab20fdfff	0xfff	globl # no wrapper
#─────────────────────FREEBSD & OPENBSD──────────────────────────────────────
scall	sys_fhstat		0x1c3126229fffffff	0xfff	globl # no wrapper
scall	sys_chflagsat		0xfff06b21cfffffff	0xfff	globl # no wrapper
scall	sys_profil		0x02c02c02cfffffff	0xfff	globl # no wrapper
scall	sys_fhstatfs		0xfff04122efffffff	0xfff	globl # no wrapper
scall	sys_utrace		0x1320d114ffffffff	0xfff	globl # no wrapper
scall	sys_closefrom		0xfff11f1fdfffffff	0xfff	globl hidden
#───────────────────────────XNU──────────────────────────────────────────────
#scall	__pthread_markcancel	0xfffffffff214cfff	0xfff	globl
#scall	__pthread_kill		0xfffffffff2148fff	0xfff	globl
#scall	__pthread_fchdir	0xfffffffff215dfff	0xfff	globl
#scall	__pthread_sigmask	0xfffffffff2149fff	0xfff	globl
#scall	__pthread_chdir		0xfffffffff215cfff	0xfff	globl
#scall	__pthread_canceled	0xfffffffff214dfff	0xfff	globl
#scall	__disable_threadsignal	0xfffffffff214bfff	0xfff	globl
#scall	abort_with_payload	0xfffffffff2209fff	0xfff	globl
#scall	accept_nocancel		0xfffffffff2194fff	0xfff	globl
#scall	access_extended		0xfffffffff211cfff	0xfff	globl
#scall	audit_session_join	0xfffffffff21adfff	0xfff	globl
#scall	audit_session_port	0xfffffffff21b0fff	0xfff	globl
#scall	audit_session_self	0xfffffffff21acfff	0xfff	globl
#scall	sys_bsdthread_create	0xfffffffff2168fff	0xfff	globl
#scall	bsdthread_ctl		0xfffffffff21defff	0xfff	globl
scall	sys_bsdthread_register	0xfffffffff216efff	0xfff	globl hidden
#scall	bsdthread_terminate	0xfffffffff2169fff	0xfff	globl
#scall	change_fdguard_np	0xfffffffff21bcfff	0xfff	globl
#scall	chmod_extended		0xfffffffff211afff	0xfff	globl
#scall	clonefileat		0xfffffffff21cefff	0xfff	globl
#scall	close_nocancel		0xfffffffff218ffff	0xfff	globl
#scall	coalition		0xfffffffff21cafff	0xfff	globl
#scall	coalition_info		0xfffffffff21cbfff	0xfff	globl
#scall	connect_nocancel	0xfffffffff2199fff	0xfff	globl
#scall	connectx		0xfffffffff21bffff	0xfff	globl
#scall	copyfile		0xfffffffff20e3fff	0xfff	globl
#scall	csops			0xfffffffff20a9fff	0xfff	globl
#scall	csops_audittoken	0xfffffffff20aafff	0xfff	globl
#scall	csrctl			0xfffffffff21e3fff	0xfff	globl
#scall	delete			0xfffffffff20e2fff	0xfff	globl
#scall	disconnectx		0xfffffffff21c0fff	0xfff	globl
#scall	exchangedata		0xfffffffff20dffff	0xfff	globl
#scall	fchmod_extended		0xfffffffff211bfff	0xfff	globl
#scall	fclonefileat		0xfffffffff2205fff	0xfff	globl
#scall	fcntl_nocancel		0xfffffffff2196fff	0xfff	globl
#scall	ffsctl			0xfffffffff20f5fff	0xfff	globl
#scall	fgetattrlist		0xfffffffff20e4fff	0xfff	globl
#scall	fileport_makefd		0xfffffffff21affff	0xfff	globl
#scall	fileport_makeport	0xfffffffff21aefff	0xfff	globl
#scall	fmount			0xfffffffff220efff	0xfff	globl
#scall	fs_snapshot		0xfffffffff2206fff	0xfff	globl
#scall	fsctl			0xfffffffff20f2fff	0xfff	globl
#scall	fsetattrlist		0xfffffffff20e5fff	0xfff	globl
#scall	fstat_extended		0xfffffffff2119fff	0xfff	globl
#scall	fsync_nocancel		0xfffffffff2198fff	0xfff	globl
#scall	getattrlist		0xfffffffff20dcfff	0xfff	globl
#scall	getattrlistat		0xfffffffff21dcfff	0xfff	globl
#scall	getattrlistbulk		0xfffffffff21cdfff	0xfff	globl
#scall	getdirentriesattr	0xfffffffff20defff	0xfff	globl
#scall	gethostuuid		0xfffffffff208efff	0xfff	globl
#scall	getsgroups		0xfffffffff2120fff	0xfff	globl
#scall	getwgroups		0xfffffffff2122fff	0xfff	globl
#scall	grab_pgo_data		0xfffffffff21edfff	0xfff	globl
#scall	guarded_close_np	0xfffffffff21bafff	0xfff	globl
#scall	guarded_kqueue_np	0xfffffffff21bbfff	0xfff	globl
#scall	guarded_open_np		0xfffffffff21b9fff	0xfff	globl
#scall	guarded_pwrite_np	0xfffffffff21e6fff	0xfff	globl
#scall	guarded_write_np	0xfffffffff21e5fff	0xfff	globl
#scall	guarded_writev_np	0xfffffffff21e7fff	0xfff	globl
#scall	identitysvc		0xfffffffff2125fff	0xfff	globl
#scall	initgroups		0xfffffffff20f3fff	0xfff	globl
#scall	iopolicysys		0xfffffffff2142fff	0xfff	globl
#scall	kas_info		0xfffffffff21b7fff	0xfff	globl
#scall	kdebug_trace		0xfffffffff20b3fff	0xfff	globl
#scall	kdebug_trace_string	0xfffffffff20b2fff	0xfff	globl
#scall	kdebug_typefilter	0xfffffffff20b1fff	0xfff	globl
#scall	kevent_id		0xfffffffff2177fff	0xfff	globl
#scall	kevent_qos		0xfffffffff2176fff	0xfff	globl
#scall	ledger			0xfffffffff2175fff	0xfff	globl
#scall	lstat_extended		0xfffffffff2156fff	0xfff	globl
#scall	memorystatus_control	0xfffffffff21b8fff	0xfff	globl
#scall	memorystatus_get_level	0xfffffffff21c5fff	0xfff	globl
#scall	microstackshot		0xfffffffff21ecfff	0xfff	globl
#scall	mkdir_extended		0xfffffffff2124fff	0xfff	globl
#scall	mkfifo_extended		0xfffffffff2123fff	0xfff	globl
#scall	modwatch		0xfffffffff20e9fff	0xfff	globl
#scall	mremap_encrypted	0xfffffffff21e9fff	0xfff	globl
#scall	msgrcv_nocancel		0xfffffffff21a3fff	0xfff	globl
#scall	msgsnd_nocancel		0xfffffffff21a2fff	0xfff	globl
#scall	msync_nocancel		0xfffffffff2195fff	0xfff	globl
#scall	necp_client_action	0xfffffffff21f6fff	0xfff	globl
#scall	necp_match_policy	0xfffffffff21ccfff	0xfff	globl
#scall	necp_open		0xfffffffff21f5fff	0xfff	globl
#scall	necp_session_action	0xfffffffff220bfff	0xfff	globl
#scall	necp_session_open	0xfffffffff220afff	0xfff	globl
#scall	net_qos_guideline	0xfffffffff220dfff	0xfff	globl
#scall	netagent_trigger	0xfffffffff21eafff	0xfff	globl
#scall	nfsclnt			0xfffffffff20f7fff	0xfff	globl
#scall	open_dprotected_np	0xfffffffff20d8fff	0xfff	globl
#scall	open_extended		0xfffffffff2115fff	0xfff	globl
#scall	open_nocancel		0xfffffffff218efff	0xfff	globl
#scall	openat_nocancel		0xfffffffff21d0fff	0xfff	globl
#scall	openbyid_np		0xfffffffff21dffff	0xfff	globl
#scall	os_fault_with_payload	0xfffffffff2211fff	0xfff	globl
#scall	peeloff			0xfffffffff21c1fff	0xfff	globl
#scall	persona			0xfffffffff21eefff	0xfff	globl
#scall	pid_hibernate		0xfffffffff21b3fff	0xfff	globl
#scall	pid_resume		0xfffffffff21b2fff	0xfff	globl
#scall	pid_shutdown_sockets	0xfffffffff21b4fff	0xfff	globl
#scall	pid_suspend		0xfffffffff21b1fff	0xfff	globl
#scall	poll_nocancel		0xfffffffff21a1fff	0xfff	globl
#scall	pread_nocancel		0xfffffffff219efff	0xfff	globl
#scall	proc_info		0xfffffffff2150fff	0xfff	globl
#scall	proc_rlimit_control	0xfffffffff21befff	0xfff	globl
#scall	proc_trace_log		0xfffffffff21ddfff	0xfff	globl
#scall	proc_uuid_policy	0xfffffffff21c4fff	0xfff	globl
#scall	process_policy		0xfffffffff2143fff	0xfff	globl
#scall	pselect_nocancel	0xfffffffff218bfff	0xfff	globl
#scall	psynch_cvbroad		0xfffffffff212ffff	0xfff	globl
#scall	psynch_cvclrprepost	0xfffffffff2138fff	0xfff	globl
#scall	psynch_cvsignal		0xfffffffff2130fff	0xfff	globl
#scall	psynch_mutexdrop	0xfffffffff212efff	0xfff	globl
#scall	psynch_mutexwait	0xfffffffff212dfff	0xfff	globl
#scall	psynch_rw_downgrade	0xfffffffff212bfff	0xfff	globl
#scall	psynch_rw_longrdlock	0xfffffffff2129fff	0xfff	globl
#scall	psynch_rw_rdlock	0xfffffffff2132fff	0xfff	globl
#scall	psynch_rw_unlock	0xfffffffff2134fff	0xfff	globl
#scall	psynch_rw_unlock2	0xfffffffff2135fff	0xfff	globl
#scall	psynch_rw_upgrade	0xfffffffff212cfff	0xfff	globl
#scall	psynch_rw_wrlock	0xfffffffff2133fff	0xfff	globl
#scall	psynch_rw_yieldwrlock	0xfffffffff212afff	0xfff	globl
#scall	pwrite_nocancel		0xfffffffff219ffff	0xfff	globl
#scall	read_nocancel		0xfffffffff218cfff	0xfff	globl
#scall	readv_nocancel		0xfffffffff219bfff	0xfff	globl
#scall	recvfrom_nocancel	0xfffffffff2193fff	0xfff	globl
#scall	recvmsg_nocancel	0xfffffffff2191fff	0xfff	globl
#scall	recvmsg_x		0xfffffffff21e0fff	0xfff	globl
#scall	renameatx_np		0xfffffffff21e8fff	0xfff	globl
#scall	searchfs		0xfffffffff20e1fff	0xfff	globl
#scall	select_nocancel		0xfffffffff2197fff	0xfff	globl
#scall	sendmsg_nocancel	0xfffffffff2192fff	0xfff	globl
#scall	sendmsg_x		0xfffffffff21e1fff	0xfff	globl
#scall	sendto_nocancel		0xfffffffff219dfff	0xfff	globl
#scall	setattrlist		0xfffffffff20ddfff	0xfff	globl
#scall	setattrlistat		0xfffffffff220cfff	0xfff	globl
#scall	setprivexec		0xfffffffff2098fff	0xfff	globl
#scall	setsgroups		0xfffffffff211ffff	0xfff	globl
#scall	settid			0xfffffffff211dfff	0xfff	globl
#scall	settid_with_pid		0xfffffffff2137fff	0xfff	globl
#scall	setwgroups		0xfffffffff2121fff	0xfff	globl
#scall	sfi_ctl			0xfffffffff21c8fff	0xfff	globl
#scall	sfi_pidctl		0xfffffffff21c9fff	0xfff	globl
#scall	shared_region_check_np	0xfffffffff2126fff	0xfff	globl
#scall	sigsuspend_nocancel	0xfffffffff219afff	0xfff	globl
#scall	socket_delegate		0xfffffffff21c2fff	0xfff	globl
#scall	stat_extended		0xfffffffff2155fff	0xfff	globl
#scall	sysctlbyname		0xfffffffff2112fff	0xfff	globl
#scall	system_override		0xfffffffff21c6fff	0xfff	globl
#scall	telemetry		0xfffffffff21c3fff	0xfff	globl
#scall	terminate_with_payload	0xfffffffff2208fff	0xfff	globl
#scall	thread_selfcounts	0xfffffffff20bafff	0xfff	globl
#scall	thread_selfid		0xfffffffff2174fff	0xfff	globl
#scall	thread_selfusage	0xfffffffff21e2fff	0xfff	globl
#scall	ulock_wait		0xfffffffff2203fff	0xfff	globl
#scall	ulock_wake		0xfffffffff2204fff	0xfff	globl
#scall	umask_extended		0xfffffffff2116fff	0xfff	globl
#scall	usrctl			0xfffffffff21bdfff	0xfff	globl
#scall	vfs_purge		0xfffffffff21c7fff	0xfff	globl
#scall	vm_pressure_monitor	0xfffffffff2128fff	0xfff	globl
#scall	wait4_nocancel		0xfffffffff2190fff	0xfff	globl
#scall	waitevent		0xfffffffff20e8fff	0xfff	globl
#scall	waitid_nocancel		0xfffffffff21a0fff	0xfff	globl
#scall	watchevent		0xfffffffff20e7fff	0xfff	globl
#scall	work_interval_ctl	0xfffffffff21f3fff	0xfff	globl
#scall	workq_kernreturn	0xfffffffff2170fff	0xfff	globl
#scall	workq_open		0xfffffffff216ffff	0xfff	globl
#scall	write_nocancel		0xfffffffff218dfff	0xfff	globl
#scall	writev_nocancel		0xfffffffff219cfff	0xfff	globl
#──────────────────────────FREEBSD───────────────────────────────────────────
#scall	sys_umtx_op		0xffffff1c6fffffff	0xfff	globl
#scall	abort2			0xffffff1cffffffff	0xfff	globl
#scall	afs3_syscall		0xffffff179fffffff	0xfff	globl
#scall	bindat			0xffffff21afffffff	0xfff	globl
#scall	cap_enter		0xffffff204fffffff	0xfff	globl
#scall	cap_fcntls_get		0xffffff219fffffff	0xfff	globl
#scall	cap_fcntls_limit	0xffffff218fffffff	0xfff	globl
#scall	cap_getmode		0xffffff205fffffff	0xfff	globl
#scall	cap_ioctls_get		0xffffff217fffffff	0xfff	globl
#scall	cap_ioctls_limit	0xffffff216fffffff	0xfff	globl
#scall	cap_rights_limit	0xffffff215fffffff	0xfff	globl
#scall	clock_getcpuclockid2	0x1e2fff0f7fffffff	0xfff	globl
#scall	connectat		0xffffff21bfffffff	0xfff	globl
#scall	cpuset			0xffffff1e4fffffff	0xfff	globl
#scall	cpuset_getdomain	0xffffff231fffffff	0xfff	globl
#scall	cpuset_getid		0xffffff1e6fffffff	0xfff	globl
#scall	cpuset_setdomain	0xffffff232fffffff	0xfff	globl
#scall	cpuset_setid		0xffffff1e5fffffff	0xfff	globl
#scall	eaccess			0xffffff178fffffff	0xfff	globl
#scall	extattr_delete_fd	0x16efff175fffffff	0xfff	globl
#scall	extattr_delete_file	0x16bfff166fffffff	0xfff	globl
#scall	extattr_delete_link	0x171fff19efffffff	0xfff	globl
#scall	extattr_get_fd		0x16dfff174fffffff	0xfff	globl
#scall	extattr_get_file	0x16afff165fffffff	0xfff	globl
#scall	extattr_get_link	0x170fff19dfffffff	0xfff	globl
#scall	extattr_list_fd		0x172fff1b5fffffff	0xfff	globl
#scall	extattr_list_file	0x173fff1b6fffffff	0xfff	globl
#scall	extattr_list_link	0x174fff1b7fffffff	0xfff	globl
#scall	extattr_set_fd		0x16cfff173fffffff	0xfff	globl
#scall	extattr_set_file	0x169fff164fffffff	0xfff	globl
#scall	extattr_set_link	0x16ffff19cfffffff	0xfff	globl
#scall	extattrctl		0x168fff163fffffff	0xfff	globl
scall	sys_fexecve		0xffffff1ecfffffff	0xfff	globl hidden
#scall	ffclock_getcounter	0xffffff0f1fffffff	0xfff	globl
#scall	ffclock_getestimate	0xffffff0f3fffffff	0xfff	globl
#scall	ffclock_setestimate	0xffffff0f2fffffff	0xfff	globl
#scall	fhlink			0xffffff235fffffff	0xfff	globl
#scall	fhlinkat		0xffffff236fffffff	0xfff	globl
#scall	fhreadlink		0xffffff237fffffff	0xfff	globl
#scall	getaudit		0xffffff1c1fffffff	0xfff	globl
scall	sys_getcontext		0x133fff1a5fffffff	0xfff	globl hidden
#scall	getdomainname		0xffff00a2ffffffff	0xfff	globl
#scall	getfhat			0xffffff234fffffff	0xfff	globl
#scall	gethostid		0xffffff08efffffff	0xfff	globl
#scall	gethostname		0xffff0057ffffffff	0xfff	globl
#scall	getkerninfo		0xffffff03ffffffff	0xfff	globl
#scall	getloginclass		0xffffff20bfffffff	0xfff	globl
scall	getpagesize_freebsd	0xffffff040fffffff	0xfff	globl hidden
#scall	gssd_syscall		0xffffff1f9fffffff	0xfff	globl
#scall	jail			0xffffff152fffffff	0xfff	globl
#scall	jail_attach		0xffffff1b4fffffff	0xfff	globl
#scall	jail_get		0xffffff1fafffffff	0xfff	globl
#scall	jail_remove		0xffffff1fcfffffff	0xfff	globl
#scall	jail_set		0xffffff1fbfffffff	0xfff	globl
#scall	kenv			0xffffff186fffffff	0xfff	globl
#scall	kldfind			0xffffff132fffffff	0xfff	globl
#scall	kldfirstmod		0xffffff135fffffff	0xfff	globl
#scall	kldload			0xffffff130fffffff	0xfff	globl
#scall	kldnext			0xffffff133fffffff	0xfff	globl
#scall	kldstat			0xffffff134fffffff	0xfff	globl
#scall	kldsym			0xffffff151fffffff	0xfff	globl
#scall	kldunload		0xffffff131fffffff	0xfff	globl
#scall	kldunloadf		0xffffff1bcfffffff	0xfff	globl
#scall	kmq_notify		0xffffff1cdfffffff	0xfff	globl
#scall	kmq_setattr		0xffffff1cafffffff	0xfff	globl
#scall	kmq_timedreceive	0xffffff1cbfffffff	0xfff	globl
#scall	kmq_timedsend		0xffffff1ccfffffff	0xfff	globl
#scall	kmq_unlink		0xffffff1cefffffff	0xfff	globl
#scall	lchflags		0x130fff187fffffff	0xfff	globl
#scall	lchmod			0x112fff112fffffff	0xfff	globl
#scall	lgetfh			0xffffff0a0fffffff	0xfff	globl
#scall	lpathconf		0x1f3fff201fffffff	0xfff	globl
scall	sys_lutimes		0x1a8fff114fffffff	0xfff	globl hidden
#scall	mac_syscall		0xffffff18afffffff	0xfff	globl
#scall	modfind			0xffffff12ffffffff	0xfff	globl
#scall	modfnext		0xffffff12efffffff	0xfff	globl
#scall	modnext			0xffffff12cfffffff	0xfff	globl
#scall	modstat			0xffffff12dfffffff	0xfff	globl
#scall	nfstat			0xffffff117fffffff	0xfff	globl
#scall	nlm_syscall		0xffffff09afffffff	0xfff	globl
#scall	nlstat			0xffffff118fffffff	0xfff	globl
#scall	nmount			0xffffff17afffffff	0xfff	globl
#scall	nnpfs_syscall		0xffffff153fffffff	0xfff	globl
#scall	nstat			0xffffff116fffffff	0xfff	globl
#scall	pdfork			0xffffff206fffffff	0xfff	globl
#scall	pdgetpid		0xffffff208fffffff	0xfff	globl
#scall	pdkill			0xffffff207fffffff	0xfff	globl
scall	sys_posix_openpt	0xffffff1f8fffffff	0xfff	globl hidden
#scall	procctl			0xffffff220fffffff	0xfff	globl
#scall	psynch_cvwait		0xfffffffff2131fff	0xfff	globl
#scall	quota			0xffffff095fffffff	0xfff	globl
#scall	rctl_add_rule		0xffffff210fffffff	0xfff	globl
#scall	rctl_get_limits		0xffffff20ffffffff	0xfff	globl
#scall	rctl_get_racct		0xffffff20dfffffff	0xfff	globl
#scall	rctl_get_rules		0xffffff20efffffff	0xfff	globl
#scall	rctl_remove_rule	0xffffff211fffffff	0xfff	globl
#scall	recv			0xffffff066fffffff	0xfff	globl
#scall	rfork			0xffffff0fbfffffff	0xfff	globl
#scall	rtprio			0xffffff0a6fffffff	0xfff	globl
scall	sys_rtprio_thread	0xffffff1d2fffffff	0xfff	globl # no wrapper
#scall	send			0xffffff065fffffff	0xfff	globl
#scall	setaudit		0xffffff1c2fffffff	0xfff	globl
#scall	setcontext		0x134fff1a6fffffff	0xfff	globl
#scall	setfib			0xffffff0affffffff	0xfff	globl
#scall	sethostid		0xffffff08ffffffff	0xfff	globl
#scall	setloginclass		0xffffff20cfffffff	0xfff	globl
#scall	sigblock		0xffffff06dfffffff	0xfff	globl
#scall	sigsetmask		0xffffff06efffffff	0xfff	globl
#scall	sigstack		0xffffff070fffffff	0xfff	globl
#scall	sigvec			0xffffff06cfffffff	0xfff	globl
#scall	sigwaitinfo		0xffffff15afffffff	0xfff	globl
#scall	sstk			0xffffff046fffffff	0xfff	globl
#scall	swapcontext		0xffffff1a7fffffff	0xfff	globl
#scall	thr_create		0xffffff1aefffffff	0xfff	globl
#scall	thr_exit		0xffffff1affffffff	0xfff	globl
#scall	thr_kill		0xffffff1b1fffffff	0xfff	globl
#scall	thr_kill2		0xffffff1e1fffffff	0xfff	globl
#scall	thr_new			0xffffff1c7fffffff	0xfff	globl
#scall	thr_self		0xffffff1b0fffffff	0xfff	globl
#scall	thr_set_name		0xffffff1d0fffffff	0xfff	globl
#scall	thr_suspend		0xffffff1bafffffff	0xfff	globl
#scall	thr_wake		0xffffff1bbfffffff	0xfff	globl
#scall	uuidgen			0x163fff188fffffff	0xfff	globl
#scall	vadvise			0xffffff048fffffff	0xfff	globl
#scall	wait			0xffffff054fffffff	0xfff	globl
#scall	wait6			0x1e1fff214fffffff	0xfff	globl
#scall	yield			0xffffff141fffffff	0xfff	globl
#──────────────────────────OPENBSD───────────────────────────────────────────
#scall	__thrsleep		0xfff05effffffffff	0xfff	globl
#scall	__thrwakeup		0xfff12dffffffffff	0xfff	globl
#scall	__threxit		0xfff12effffffffff	0xfff	globl
#scall	__thrsigdivert		0xfff12fffffffffff	0xfff	globl
#scall	__set_tcb		0xfff149ffffffffff	0xfff	globl
#scall	__get_tcb		0xfff14affffffffff	0xfff	globl
#scall	adjfreq			0xfff131ffffffffff	0xfff	globl
#scall	getdtablecount		0xfff012ffffffffff	0xfff	globl
#scall	getlogin_r		0xfff08dffffffffff	0xfff	globl
#scall	getrtable		0xfff137ffffffffff	0xfff	globl
#scall	getthrid		0xfff12bffffffffff	0xfff	globl
#scall	kbind			0xfff056ffffffffff	0xfff	globl
#scall	mquery			0xfff11effffffffff	0xfff	globl # openbsd:pad (todo)
#scall	obreak			0x011011ffffffffff	0xfff	globl
#scall	sendsyslog		0xfff070ffffffffff	0xfff	globl
#scall	setrtable		0xfff136ffffffffff	0xfff	globl
#scall	swapctl			0x10f0c1ffffffffff	0xfff	globl
#scall	thrkill			0xfff077ffffffffff	0xfff	globl
scall	sys_unveil		0xfff072ffffffffff	0xfff	globl hidden
#──────────────────────────NETBSD────────────────────────────────────────────
#scall	_lwp_create		0x135fffffffffffff	0xfff	globl # int _lwp_create(const struct ucontext_netbsd *ucp, uint64_t flags, int *new_lwp)
#scall	_lwp_exit		0x136fffffffffffff	0xfff	globl # int _lwp_exit(void)
#scall	_lwp_self		0x137fffffffffffff	0xfff	globl # int _lwp_self(void)
#scall	_lwp_wait		0x138fffffffffffff	0xfff	globl # int _lwp_wait(int wait_for, int *departed)
#scall	_lwp_suspend		0x139fffffffffffff	0xfff	globl # int _lwp_suspend(int target)
#scall	_lwp_continue		0x13afffffffffffff	0xfff	globl # int _lwp_continue(int target)
#scall	_lwp_wakeup		0x13bfffffffffffff	0xfff	globl # int _lwp_wakeup(int target)
#scall	_lwp_getprivate		0x13cfffffffffffff	0xfff	globl # void *_lwp_getprivate(void)
#scall	_lwp_setprivate		0x13dfffffffffffff	0xfff	globl # int _lwp_setprivate(void *ptr)
#scall	_lwp_kill		0x13efffffffffffff	0xfff	globl # int _lwp_kill(int target, int signo)
#scall	_lwp_detach		0x13ffffffffffffff	0xfff	globl # int _lwp_park(int clock_id, int flags, struct timespec *ts, int unpark, const void *hint, const void *unparkhint)
#scall	_lwp_park		0x1defffffffffffff	0xfff	globl # int _lwp_park(int clock_id, int flags, struct timespec *ts, int unpark, const void *hint, const void *unparkhint)
#scall	_lwp_unpark		0x141fffffffffffff	0xfff	globl # int _lwp_unpark_all(int target, const void *hint)
#scall	_lwp_unpark_all		0x142fffffffffffff	0xfff	globl # int _lwp_unpark_all(const int *targets, size_t ntargets, const void *hint)
#scall	_lwp_setname		0x143fffffffffffff	0xfff	globl # int _lwp_setname(int target, const char *name)
#scall	_lwp_getname		0x144fffffffffffff	0xfff	globl # int _lwp_getname(int target, char *name, size_t len)
#scall	_lwp_ctl		0x145fffffffffffff	0xfff	globl # int _lwp_ctl(int features, struct lwpctl **address)
