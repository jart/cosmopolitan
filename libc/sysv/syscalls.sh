/*bin/echo   ' -*- mode:sh; indent-tabs-mode:nil; tab-width:8; coding:utf-8 -*-│
│vi: set net ft=sh ts=2 sts=2 sw=2 fenc=utf-8                               :vi│
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
#	                             GNU/Systemd┐              │   ffff │ unavailable    │
# 	                                 2.6.18+│              │   sys_ │ wrapped        │
#	                             Mac OS X┐  │              │ __sys_ │ wrapped twice  │
#	                                15.6+│  │              └─────────────────────────┘
#	                          FreeBSD┐   │  │
#	                              12+│ ┌─│──│── XnuClass{1:Mach,2:Unix}
#	                       OpenBSD┐  │ │ │  │
#	                          6.4+│  │ │ │  │
#	                     NetBSD┐  │  │ │ │  │
#	                       9.1+│  │  │ │ │  │
#	Symbol                    ┌┴┐┌┴┐┌┴┐│┬┴┐┌┴┐      Directives & Commentary
scall	sys_exit		0x00100100120010e7	globl hidden # a.k.a. exit_group
scall	sys_read		0x0030030032003000	globl hidden
scall	sys_write		0x0040040042004001	globl hidden
scall	sys_open		0x0050050052005002	globl hidden
scall	sys_close		0x0060060062006003	globl hidden
scall	__sys_stat		0x1b7026fff2152004	globl hidden # FreeBSD 11→12 fumble; use sys_fstatat(); blocked on Android
scall	__sys_fstat		0x1b80352272153005	globl hidden # needs __stat2linux()
scall	__sys_lstat		0x1b90280282154006	globl hidden # needs __stat2linux(); blocked on Android
scall	sys_poll		0x0d10fc0d120e6007	globl hidden
scall	sys_ppoll		0xfff06d221ffff10f	globl hidden # consider INTON/INTOFF tutorial in examples/unbourne.c
scall	sys_lseek		0x0c70c71de20c7008	globl hidden # netbsd+openbsd:evilpad
scall	__sys_mmap		0x0c50c51dd20c5009	globl hidden # netbsd+openbsd:pad
scall	sys_msync		0x115100041204101a	globl hidden
scall	sys_mprotect		0x04a04a04a204a00a	globl hidden
scall	sys_munmap		0x049049049204900b	globl hidden
scall	sys_sigaction		0x15402e1a0202e00d	globl hidden # rt_sigaction on Lunix; it's complicated on NetBSD
scall	sys_sigprocmask		0x125030154203000e	globl hidden # a.k.a. rt_sigprocmask, openbsd:byvalue
scall	sys_ioctl		0x0360360362036010	globl hidden
scall	sys_pread		0x0ad0ad1db2099011	globl hidden # a.k.a. pread64; netbsd+openbsd:pad
scall	sys_pwrite		0x0ae0ae1dc209a012	globl hidden # a.k.a. pwrite64; netbsd+openbsd:pad
scall	sys_readv		0x0780780782078013	globl hidden
scall	sys_writev		0x0790790792079014	globl hidden
scall	sys_access		0x0210210212021015	globl hidden
scall	__sys_pipe		0x02a10721e202a016	globl hidden # NOTE: pipe2() on FreeBSD; XNU is pipe(void)→eax:edx
scall	sys_select		0x1a104705d205d017	globl hidden
scall	pselect			0x1b406e20a218afff	globl
scall	pselect6		0xfffffffffffff10e	globl
scall	sys_sched_yield		0x15e12a14b103c018	globl hidden # swtch() on xnu
scall	__sys_mremap		0x19bffffffffff019	globl hidden
scall	mincore			0x04e04e04e204e01b	globl
scall	sys_madvise		0x04b04b04b204b01c	globl hidden
scall	shmget			0x0e71210e7210901d	globl # consider mmap
scall	shmat			0x0e40e40e4210601e	globl # consider mmap
scall	shmctl			0x1bb128200210701f	globl # consider mmap
scall	sys_dup			0x0290290292029020	globl hidden
scall	sys_dup2		0x05a05a05a205a021	globl hidden
scall	sys_pause		0xfffffffffffff022	globl hidden
scall	sys_nanosleep		0x1ae05b0f0ffff023	globl hidden
scall	sys_getitimer		0x1aa0460562056024	globl hidden
scall	sys_setitimer		0x1a90450532053026	globl hidden
scall	sys_alarm		0xfffffffffffff025	globl hidden
scall	sys_getpid		0x0140140142014027	globl hidden # netbsd returns ppid in edx
scall	sys_sendfile		0xffffff1892151028	globl hidden # Linux vs. XNU/BSD ABIs very different
scall	__sys_socket		0x18a0610612061029	globl hidden
scall	__sys_connect		0x062062062206202a	globl hidden
scall	__sys_accept		0x01e01e21d201e02b	globl hidden # accept4 on freebsd
scall	sys_sendto		0x085085085208502c	globl hidden
scall	sys_recvfrom		0x01d01d01d201d02d	globl hidden
scall	sys_sendmsg		0x01c01c01c201c02e	globl hidden
scall	sys_recvmsg		0x01b01b01b201b02f	globl hidden
scall	sys_shutdown		0x0860860862086030	globl hidden
scall	sys_bind		0x0680680682068031	globl hidden
scall	sys_listen		0x06a06a06a206a032	globl hidden
scall	__sys_getsockname	0x0200200202020033	globl hidden
scall	__sys_getpeername	0x01f01f08d201f034	globl hidden
scall	__sys_socketpair	0x0870870872087035	globl hidden
scall	sys_setsockopt		0x0690690692069036	globl hidden
scall	sys_getsockopt		0x0760760762076037	globl hidden
scall	sys_fork		0x0020020022002039	globl hidden # xnu needs eax&=~-edx bc eax always holds pid and edx is 0 for parent and 1 for child
#scall	vfork			0x042042042204203a	globl        # this syscall is from the moon so we implement it by hand in libc/calls/hefty/vfork.S
scall	sys_posix_spawn		0xfffffffff20f4fff	globl hidden # good luck figuring out how xnu defines this
scall	__sys_execve		0x03b03b03b203b03b	globl hidden
scall	__sys_wait4		0x1c100b007200703d	globl hidden
scall	sys_kill		0x02507a025202503e	globl hidden # kill(pid, sig, 1) b/c xnu
scall	sys_killpg		0xffffff092fffffff	globl hidden
scall	clone			0xfffffffffffff038	globl
scall	tkill			0xfffffffffffff0c8	globl
scall	futex			0xfff053fffffff0ca	globl
scall	set_robust_list		0xfffffffffffff111	globl
scall	get_robust_list		0xfffffffffffff112	globl
scall	sys_uname		0xffffff0a4ffff03f	globl hidden
scall	semget			0x0dd0dd0dd20ff040	globl # won't polyfill for windows
scall	semop			0x0de1220de2100041	globl # won't polyfill for windows
scall	semctl			0xfff1271fe20fe042	globl # won't polyfill for windows
scall	shmdt			0x0e60e60e62108043	globl # won't polyfill for windows
scall	msgget			0x0e10e10e12103044	globl # won't polyfill for windows
scall	msgsnd			0x0e20e20e22104045	globl # won't polyfill for windows
scall	msgrcv			0x0e30e30e32105046	globl # won't polyfill for windows
scall	msgctl			0x1bc1291ff2102047	globl # won't polyfill for windows
scall	__sys_fcntl		0x05c05c05c205c048	globl hidden
scall	sys_flock		0x0830830832083049	globl hidden
scall	sys_fsync		0x05f05f05f205f04a	globl hidden
scall	sys_fdatasync		0x0f105f22620bb04b	globl hidden # fsync() on openbsd
scall	sys_truncate		0x0c80c81df20c804c	globl hidden # netbsd+openbsd:pad
scall	sys_ftruncate		0x0c90c91e020c904d	globl hidden # netbsd+openbsd:pad
scall	sys_getcwd		0x128130146ffff04f	globl hidden
scall	sys_chdir		0x00c00c00c200c050	globl hidden
scall	sys_fchdir		0x00d00d00d200d051	globl hidden
scall	sys_rename		0x0800800802080052	globl hidden
scall	sys_mkdir		0x0880880882088053	globl hidden
scall	sys_rmdir		0x0890890892089054	globl hidden
scall	sys_creat		0xffffff008ffff055	globl hidden
scall	sys_link		0x0090090092009056	globl hidden
scall	sys_unlink		0x00a00a00a200a057	globl hidden
scall	sys_symlink		0x0390390392039058	globl hidden
scall	readlink		0x03a03a03a203a059	globl        # usually an anti-pattern
scall	sys_chmod		0x00f00f00f200f05a	globl hidden
scall	sys_fchmod		0x07c07c07c207c05b	globl hidden
scall	sys_chown		0x010010010201005c	globl hidden # impl. w/ fchownat() @asyncsignalsafe
scall	sys_fchown		0x07b07b07b207b05d	globl hidden # @asyncsignalsafe
scall	sys_lchown		0x1130fe0fe216c05e	globl hidden # impl. w/ fchownat()
scall	umask			0x03c03c03c203c05f	globl
scall	sys_gettimeofday	0x1a20430742074060	globl hidden # xnu esi/edx=0
scall	sys_getrlimit		0x0c20c20c220c2061	globl hidden
scall	__sys_getrusage		0x1bd0130752075062	globl hidden
scall	sys_sysinfo		0xfffffffffffff063	globl hidden
scall	sys_times		0xfffffffffffff064	globl hidden
scall	sys_ptrace		0x01a01a01a201a065	globl hidden
scall	syslog			0xfffffffffffff067	globl
scall	sys_getuid		0x0180180182018066	globl hidden
scall	sys_getgid		0x02f02f02f202f068	globl hidden
scall	sys_getppid		0xfff027027202706e	globl hidden # see sys_getpid()→edx for netbsd
scall	getpgrp			0x051051051205106f	globl
scall	sys_setsid		0x0930930932093070	globl hidden
scall	sys_getsid		0x11e0ff136213607c	globl hidden
scall	getpgid			0x0cf0cf0cf2097079	globl
scall	setpgid			0x052052052205206d	globl
scall	geteuid			0xfff019019201906b	globl
scall	getegid			0xfff02b02b202b06c	globl
scall	getgroups		0x04f04f04f204f073	globl
scall	setgroups		0x0500500502050074	globl
scall	setreuid		0x07e07e07e207e071	globl
scall	setregid		0x07f07f07f207f072	globl
scall	setuid			0x0170170172017069	globl
scall	setgid			0x0b50b50b520b506a	globl
scall	sys_setresuid		0xfff11a137ffff075	globl hidden # polyfilled for xnu
scall	sys_setresgid		0xfff11c138ffff077	globl hidden # polyfilled for xnu
scall	getresuid		0xfff119168ffff076	globl # semantics aren't well-defined
scall	getresgid		0xfff11b169ffff078	globl # semantics aren't well-defined
scall	sigpending		0x124034034203407f	globl
scall	sys_sigsuspend		0x12606f155206f082	globl hidden # openbsd:byvalue
scall	sigaltstack		0x1191200352035083	globl
scall	sys_mknod		0x1c200e00e200e085	globl hidden
scall	mknodat			0x1cc14022fffff103	globl # FreeBSD 12+
scall	sys_mkfifo		0x0840840842084fff	globl hidden
scall	mkfifoat		0x1cb13f1f1fffffff	globl
scall	statfs			0xfff03f22b2159089	globl
scall	fstatfs			0xfff04022c215a08a	globl
scall	sys_getpriority		0x064064064206408c	globl hidden
scall	sys_setpriority		0x060060060206008d	globl hidden # modern nice()
scall	mlock			0x0cb0cb0cb20cb095	globl
scall	munlock			0x0cc0cc0cc20cc096	globl
scall	mlockall		0x0f210f1442144097	globl
scall	munlockall		0x0f31101452145098	globl
scall	sys_setrlimit		0x0c30c30c320c30a0	globl hidden
scall	chroot			0x03d03d03d203d0a1	globl
scall	sys_sync		0xfff02402420240a2	globl hidden
scall	acct			0x03303303320330a3	globl
scall	settimeofday		0x1a304407a207a0a4	globl
scall	mount			0x19a01501520a70a5	globl
scall	reboot			0x0d003703720370a9	globl
scall	quotactl		0xfff09409420a50b3	globl
scall	setfsuid		0xfffffffffffff07a	globl
scall	setfsgid		0xfffffffffffff07b	globl
scall	capget			0xfffffffffffff07d	globl
scall	capset			0xfffffffffffff07e	globl
scall	sigtimedwait		0xffffff159ffff080	globl
scall	sys_sigqueue		0xffffff1c8fffffff	globl
scall	sys_sigqueueinfo	0x0f5ffffffffff081	globl # rt_sigqueueinfo on linux
scall	personality		0xfffffffffffff087	globl
scall	ustat			0xfffffffffffff088	globl
scall	sysfs			0xfffffffffffff08b	globl
scall	sched_setparam		0xffffff147ffff08e	globl
scall	sched_getparam		0xffffff148ffff08f	globl
scall	sched_setscheduler	0xffffff149ffff090	globl
scall	sched_getscheduler	0xffffff14affff091	globl
scall	sched_get_priority_max	0xffffff14cffff092	globl
scall	sched_get_priority_min	0xffffff14dffff093	globl
scall	sched_rr_get_interval	0xffffff14effff094	globl
scall	vhangup			0xfffffffffffff099	globl
scall	modify_ldt		0xfffffffffffff09a	globl
scall	pivot_root		0xfffffffffffff09b	globl
scall	_sysctl			0xfffffffffffff09c	globl
scall	prctl			0xfffffffffffff09d	globl
scall	sys_arch_prctl		0xfff0a50a5ffff09e	globl hidden # sysarch() on bsd
scall	adjtimex		0xfffffffffffff09f	globl
scall	umount2			0xfffffffffffff0a6	globl
scall	swapon			0xffffff05520550a7	globl
scall	swapoff			0xffffff1a8ffff0a8	globl
scall	sethostname		0xffffff058ffff0aa	globl
scall	setdomainname		0xffffff0a3ffff0ab	globl
scall	iopl			0xfffffffffffff0ac	globl
scall	ioperm			0xfffffffffffff0ad	globl
scall	init_module		0xfffffffffffff0af	globl
scall	delete_module		0xfffffffffffff0b0	globl
scall	sys_gettid		0xfffffffff211e0ba	globl hidden
scall	readahead		0xfffffffffffff0bb	globl # consider fadvise() / madvise()
scall	setxattr		0x177ffffff20ec0bc	globl
scall	fsetxattr		0x179ffffff20ed0be	globl
scall	getxattr		0x17affffff20ea0bf	globl
scall	fgetxattr		0x17cffffff20eb0c1	globl
scall	listxattr		0x17dffffff20f00c2	globl
scall	flistxattr		0x17fffffff20f10c4	globl
scall	removexattr		0x180ffffff20ee0c5	globl
scall	fremovexattr		0x182ffffff20ef0c7	globl
scall	lsetxattr		0x178ffffffffff0bd	globl
scall	lgetxattr		0x17bffffffffff0c0	globl
scall	llistxattr		0x17effffffffff0c3	globl
scall	lremovexattr		0x181ffffffffff0c6	globl
scall	sys_sched_setaffinity	0xfffffffffffff0cb	globl hidden
scall	sched_getaffinity	0xfffffffffffff0cc	globl
scall	cpuset_getaffinity	0xffffff1e7fffffff	globl
scall	cpuset_setaffinity	0xffffff1e8fffffff	globl
scall	io_setup		0xfffffffffffff0ce	globl
scall	io_destroy		0xfffffffffffff0cf	globl
scall	io_getevents		0xfffffffffffff0d0	globl
scall	io_submit		0xfffffffffffff0d1	globl
scall	io_cancel		0xfffffffffffff0d2	globl
scall	lookup_dcookie		0xfffffffffffff0d4	globl
scall	sys_epoll_create	0xfffffffffffff0d5	globl
scall	sys_epoll_wait		0xfffffffffffff0e8	globl
scall	sys_epoll_ctl		0xfffffffffffff0e9	globl
scall	getdents		0x18606311020c40d9	globl hidden # four args b/c xnu, getdirentries on xnu, 32-bit on xnu/freebsd, getdents64 on linux, 64-bit on openbsd
scall	set_tid_address		0xfffffffffffff0da	globl
scall	restart_syscall		0xfffffffffffff0db	globl
scall	semtimedop		0xfffffffffffff0dc	globl
scall	sys_fadvise		0xffffff213ffff0dd	globl hidden
scall	timer_create		0x0ebffffffffff0de	globl
scall	timer_settime		0x1beffffffffff0df	globl
scall	timer_gettime		0x1bfffffffffff0e0	globl
scall	timer_getoverrun	0x0efffffffffff0e1	globl
scall	timer_delete		0x0ecffffffffff0e2	globl
scall	ktimer_create		0xffffff0ebfffffff	globl
scall	ktimer_delete		0xffffff0ecfffffff	globl
scall	ktimer_getoverrun	0xffffff0effffffff	globl
scall	ktimer_gettime		0xffffff0eefffffff	globl
scall	ktimer_settime		0xffffff0edfffffff	globl
scall	clock_settime		0x1ac0580e9ffff0e3	globl
scall	sys_clock_gettime	0x1ab0570e8ffff0e4	globl hidden # Linux 2.6+ (c. 2003); XNU uses magic address
scall	clock_getres		0x1ad0590eaffff0e5	globl
scall	clock_nanosleep		0xffffff0f4ffff0e6	globl
scall	tgkill			0xfffffffffffff0ea	globl
scall	mbind			0xfffffffffffff0ed	globl
scall	set_mempolicy		0xfffffffffffff0ee	globl
scall	get_mempolicy		0xfffffffffffff0ef	globl
scall	mq_open			0x101ffffffffff0f0	globl # won't polyfill
scall	mq_unlink		0x103ffffffffff0f1	globl # won't polyfill
scall	mq_timedsend		0x1b0ffffffffff0f2	globl # won't polyfill
scall	mq_timedreceive		0x1b1ffffffffff0f3	globl # won't polyfill
scall	mq_notify		0x106ffffffffff0f4	globl # won't polyfill
scall	mq_getsetattr		0xfffffffffffff0f5	globl # won't polyfill
scall	kexec_load		0xfffffffffffff0f6	globl
scall	waitid			0xfffffffff20ad0f7	globl # Linux 2.6.9+
scall	add_key			0xfffffffffffff0f8	globl
scall	request_key		0xfffffffffffff0f9	globl
scall	keyctl			0xfffffffffffff0fa	globl
scall	ioprio_set		0xfffffffffffff0fb	globl
scall	ioprio_get		0xfffffffffffff0fc	globl
scall	inotify_init		0xfffffffffffff0fd	globl # wicked
scall	inotify_add_watch	0xfffffffffffff0fe	globl
scall	inotify_rm_watch	0xfffffffffffff0ff	globl
scall	__sys_openat		0x1d41411f321cf101	globl hidden # Linux 2.6.16+ (c. 2007)
scall	sys_mkdirat		0x1cd13e1f021db102	globl hidden
scall	sys_fchownat		0x1d013b1eb21d4104	globl hidden # @asyncsignalsafe
scall	sys_utime		0xfffffffffffff084	globl hidden
scall	sys_utimes		0x1a404c08a208a0eb	globl hidden
scall	sys_futimesat		0xffffff1eeffff105	globl hidden # @asyncsignalsafe
scall	sys_futimes		0x1a704d0ce208bfff	globl hidden
scall	sys_futimens		0x1d8055222fffffff	globl hidden
scall	__sys_fstatat		0x1d202a22821d6106	globl hidden # a.k.a. newfstatat(); FreeBSD 12+; needs __stat2linux()
scall	sys_unlinkat		0x1d71451f721d8107	globl hidden
scall	sys_renameat		0x1ca1431f521d1108	globl hidden
scall	sys_linkat		0x1c913d1ef21d7109	globl hidden
scall	sys_symlinkat		0x1d61441f621da10a	globl hidden
scall	sys_readlinkat		0x1d51421f421d910b	globl hidden
scall	sys_fchmodat		0x1cf13a1ea21d310c	globl hidden
scall	sys_faccessat		0x1ce1391e921d210d	globl hidden
scall	unshare			0xfffffffffffff110	globl
scall	sys_splice		0xfffffffffffff113	globl hidden # Linux 2.6.17+ (c. 2007)
scall	tee			0xfffffffffffff114	globl        # Linux 2.6.17+
scall	sys_sync_file_range	0xfffffffffffff115	globl hidden # Linux 2.6.17+
scall	sys_vmsplice		0xfffffffffffff116	globl hidden
scall	migrate_pages		0xfffffffffffff100	globl        # numa numa yay
scall	move_pages		0xfffffffffffff117	globl        # NOTE: We view Red Hat versions as "epochs" for all distros.
#──────────────────────RHEL 5.0 LIMIT────────────────────────        # ←┬─ last distro with gplv2 licensed compiler c. 2007
scall	sys_preadv		0x12110b121ffff127	globl hidden #  ├─ last distro with system v shell script init
scall	sys_pwritev		0x12210c122ffff128	globl hidden #  ├─ rob landley unleashes busybox gpl lawsuits
scall	__sys_utimensat		0x1d3054223ffff118	globl hidden #  ├─ python modules need this due to pep513
scall	fallocate		0xfffffffffffff11d	globl hidden #  ├─ end of life 2020-11-30 (extended)
scall	posix_fallocate		0xffffff212fffffff	globl hidden #  └─ cosmopolitan supports rhel5+
scall	__sys_accept4		0xfff05d21dffff120	globl hidden # Linux 2.6.28+
scall	__sys_dup3		0x1c6066fffffff124	globl hidden # Linux 2.6.27+
scall	__sys_pipe2		0x1c506521effff125	globl hidden # Linux 2.6.27+
scall	epoll_pwait		0xfffffffffffff119	globl
scall	sys_epoll_create1	0xfffffffffffff123	globl
scall	perf_event_open		0xfffffffffffff12a	globl
scall	inotify_init1		0xfffffffffffff126	globl
scall	rt_tgsigqueueinfo	0xfffffffffffff129	globl
scall	signalfd		0xfffffffffffff11a	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	signalfd4		0xfffffffffffff121	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	eventfd			0xfffffffffffff11c	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	eventfd2		0xfffffffffffff122	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	timerfd_create		0xfffffffffffff11b	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	timerfd_settime		0xfffffffffffff11e	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	timerfd_gettime		0xfffffffffffff11f	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
#──────────────────────RHEL 6.0 LIMIT──────────────────────── # ←┬─ modern glibc needs rhel6+ c. 2011
scall	recvmmsg		0x1dbffffffffff12b	globl #  ├─ end of life 2024-06-30 (extended)
scall	fanotify_init		0xfffffffffffff12c	globl #  ├─ last distro with the original gnome desktop
scall	fanotify_mark		0xfffffffffffff12d	globl #  └─ apple and google condemn the gplv3/gccrtev3
scall	prlimit			0xfffffffffffff12e	globl
scall	name_to_handle_at	0xfffffffffffff12f	globl
scall	open_by_handle_at	0xfffffffffffff130	globl
scall	clock_adjtime		0xfffffffffffff131	globl
scall	syncfs			0xfffffffffffff132	globl
scall	sendmmsg		0x1dcffffffffff133	globl
scall	setns			0xfffffffffffff134	globl
scall	getcpu			0xfffffffffffff135	globl
scall	process_vm_readv	0xfffffffffffff136	globl
scall	process_vm_writev	0xfffffffffffff137	globl
scall	kcmp			0xfffffffffffff138	globl
scall	finit_module		0xfffffffffffff139	globl
#──────────────────────RHEL 7.0 LIMIT──────────────────────── # ←┬─ anything that links or uses c++11 code needs rhel7+ c. 2014
scall	sched_setattr		0xfffffffffffff13a	globl #  ├─ desktop replaced with tablet-first gui inspired by mac os x
scall	sched_getattr		0xfffffffffffff13b	globl #  ├─ karen sandler requires systemd init and boot for tablet gui
scall	renameat2		0xfffffffffffff13c	globl #  └─ debian founder ian murdock found strangled with vacuum cord
scall	seccomp			0xfffffffffffff13d	globl
scall	sys_getrandom		0xfff00723321f413e	globl hidden  # Linux 3.17+ and getentropy() on XNU/OpenBSD, coming to NetBSD in 9.2
scall	memfd_create		0xfffffffffffff13f	globl # wut
scall	kexec_file_load		0xfffffffffffff140	globl
scall	bpf			0xfffffffffffff141	globl
scall	execveat		0xfffffffffffff142	globl
scall	userfaultfd		0xfffffffffffff143	globl # Linux 4.3+ (c. 2015)
scall	membarrier		0xfffffffffffff144	globl # Linux 4.3+ (c. 2015)
scall	mlock2			0xfffffffffffff145	globl # Linux 4.5+ (c. 2016)
scall	sys_copy_file_range	0xffffff239ffff146	globl hidden # Linux 4.5+ (c. 2016), FreeBSD 13+
scall	preadv2			0xfffffffffffff147	globl
scall	pwritev2		0xfffffffffffff148	globl
scall	pkey_mprotect		0xfffffffffffff149	globl
scall	pkey_alloc		0xfffffffffffff14a	globl
scall	pkey_free		0xfffffffffffff14b	globl
scall	statx			0xfffffffffffff14c	globl # lool https://lkml.org/lkml/2010/7/22/249
scall	io_pgetevents		0xfffffffffffff14d	globl
scall	rseq			0xfffffffffffff14e	globl # Linux 4.18+ (c. 2018)
#──────────────────────LINUX 4.18 LIMIT────────────────────── # ←┬─ last version of linux kernel buildable with only gplv2
scall	pidfd_send_signal	0xfffffffffffff1a8	globl #  ├─ linux conferences ban linux founder linus torvalds
scall	io_uring_setup		0xfffffffffffff1a9	globl #  └─ gnu founder richard stallman publicly disgraced
scall	io_uring_enter		0xfffffffffffff1aa	globl
scall	io_uring_register	0xfffffffffffff1ab	globl
#────────────────────────RHEL CLOUD────────────────────────── # ←┬─ red hat terminates community release of enterprise linux circa 2020
scall	pledge			0xfff06cffffffffff	globl #  └─ online linux services ban the president of united states of america
scall	msyscall		0xfff025ffffffffff	globl

#	The Fifth Bell System Interface, Community Edition
#	» besiyata dishmaya
#	                             GNU/Systemd┐
#	                             Mac OS X┐  │
#	                          FreeBSD┐   │  │
#	                       OpenBSD┐  │ ┌─│──│── XnuClass{1:Mach,2:Unix}
#	                     NetBSD┐  │  │ │ │  │
#	Symbol                    ┌┴┐┌┴┐┌┴┐│┬┴┐┌┴┐      Directives & Commentary
scall	ktrace			0x02d02d02dfffffff	globl
scall	kqueue			0x15810d16a216afff	globl
scall	kevent			0x1b30482302171fff	globl
scall	revoke			0x0380380382038fff	globl
scall	setlogin		0xfff0320322032fff	globl
scall	getfh			0x18b0a10a120a1fff	globl
scall	chflags			0x0220220222022fff	globl
scall	getfsstat		0xfff03e22d215bfff	globl
scall	nfssvc			0x09b09b09b209bfff	globl
scall	adjtime			0x1a508c08c208cfff	globl
scall	fchflags		0x0230230232023fff	globl
scall	__bsd_seteuid		0xfff0b70b720b7fff	globl hidden # wrapped via setreuid()
scall	__bsd_setegid		0xfff0b60b620b6fff	globl hidden # wrapped via setregid()
scall	fpathconf		0x0c00c00c020c0fff	globl
scall	fhopen			0x18c10812a20f8fff	globl
scall	unmount			0x016016016209ffff	globl
scall	issetugid		0xfff0fd0fd2147fff	globl
scall	minherit		0x1110fa0fa20fafff	globl
scall	pathconf		0x0bf0bf0bf20bffff	globl
scall	sysctl			0x0ca0ca0ca20cafff	globl
#───────────────────────XNU & FREEBSD────────────────────────
scall	ntp_adjtime		0x0b0fff0b0220ffff	globl
scall	ntp_gettime		0x1c0fff0f82210fff	globl
scall	shm_unlink		0xffffff1e3210bfff	globl
scall	shm_open		0xffffff1e2210afff	globl
scall	aio_read		0x192fff13e213efff	globl
scall	aio_suspend		0x1b6fff13b213bfff	globl
scall	aio_cancel		0x18ffff13c213cfff	globl
scall	aio_fsync		0x191fff1d12139fff	globl
scall	aio_error		0x190fff13d213dfff	globl
scall	aio_return		0x193fff13a213afff	globl
scall	aio_write		0x195fff13f213ffff	globl
scall	aio_waitcomplete	0xffffff167fffffff	globl
scall	aio_suspend_nocancel	0xfffffffff21a5fff	globl
scall	aio_mlock		0xffffff21ffffffff	globl
scall	sigwait			0xffffff1ad214afff	globl
scall	undelete		0x0cdfff0cd20cdfff	globl
scall	getlogin		0xffffff0312031fff	globl
scall	getdtablesize		0xffffff0592059fff	globl
scall	setauid			0xffffff1c02162fff	globl
scall	audit			0xffffff1bd215efff	globl
scall	auditctl		0xffffff1c52167fff	globl
scall	getaudit_addr		0xffffff1c32165fff	globl
scall	getdirentries		0xffffff22a20c4fff	globl # xnu getdirentries64 is 0x158
scall	lio_listio		0x196fff1402140fff	globl
scall	setaudit_addr		0xffffff1c42166fff	globl
scall	getauid			0xffffff1bf2161fff	globl
scall	semsys			0xffffff0a920fbfff	globl
scall	auditon			0xffffff1be215ffff	globl
scall	msgsys			0xffffff0aa20fcfff	globl
scall	shmsys			0xffffff0ab20fdfff	globl
#─────────────────────FREEBSD & OPENBSD──────────────────────
scall	fhstat			0x1c3126229fffffff	globl
scall	chflagsat		0xfff06b21cfffffff	globl
scall	profil			0x02c02c02cfffffff	globl
scall	fhstatfs		0xfff04122efffffff	globl
scall	utrace			0x1320d114ffffffff	globl
scall	closefrom		0xfff11f1fdfffffff	globl
#───────────────────────────XNU──────────────────────────────
scall	__pthread_markcancel	0xfffffffff214cfff	globl
scall	__pthread_kill		0xfffffffff2148fff	globl
scall	__pthread_fchdir	0xfffffffff215dfff	globl
scall	__pthread_sigmask	0xfffffffff2149fff	globl
scall	__pthread_chdir		0xfffffffff215cfff	globl
scall	__pthread_canceled	0xfffffffff214dfff	globl
scall	__disable_threadsignal	0xfffffffff214bfff	globl
scall	abort_with_payload	0xfffffffff2209fff	globl
scall	accept_nocancel		0xfffffffff2194fff	globl
scall	access_extended		0xfffffffff211cfff	globl
scall	audit_session_join	0xfffffffff21adfff	globl
scall	audit_session_port	0xfffffffff21b0fff	globl
scall	audit_session_self	0xfffffffff21acfff	globl
scall	bsdthread_create	0xfffffffff2168fff	globl
scall	bsdthread_ctl		0xfffffffff21defff	globl
scall	bsdthread_register	0xfffffffff216efff	globl
scall	bsdthread_terminate	0xfffffffff2169fff	globl
scall	change_fdguard_np	0xfffffffff21bcfff	globl
scall	chmod_extended		0xfffffffff211afff	globl
scall	clonefileat		0xfffffffff21cefff	globl
scall	close_nocancel		0xfffffffff218ffff	globl
scall	coalition		0xfffffffff21cafff	globl
scall	coalition_info		0xfffffffff21cbfff	globl
scall	connect_nocancel	0xfffffffff2199fff	globl
scall	connectx		0xfffffffff21bffff	globl
scall	copyfile		0xfffffffff20e3fff	globl
scall	csops			0xfffffffff20a9fff	globl
scall	csops_audittoken	0xfffffffff20aafff	globl
scall	csrctl			0xfffffffff21e3fff	globl
scall	delete			0xfffffffff20e2fff	globl
scall	disconnectx		0xfffffffff21c0fff	globl
scall	exchangedata		0xfffffffff20dffff	globl
scall	fchmod_extended		0xfffffffff211bfff	globl
scall	fclonefileat		0xfffffffff2205fff	globl
scall	fcntl_nocancel		0xfffffffff2196fff	globl
scall	ffsctl			0xfffffffff20f5fff	globl
scall	fgetattrlist		0xfffffffff20e4fff	globl
scall	fileport_makefd		0xfffffffff21affff	globl
scall	fileport_makeport	0xfffffffff21aefff	globl
scall	fmount			0xfffffffff220efff	globl
scall	fs_snapshot		0xfffffffff2206fff	globl
scall	fsctl			0xfffffffff20f2fff	globl
scall	fsetattrlist		0xfffffffff20e5fff	globl
scall	fstat_extended		0xfffffffff2119fff	globl
scall	fsync_nocancel		0xfffffffff2198fff	globl
scall	getattrlist		0xfffffffff20dcfff	globl
scall	getattrlistat		0xfffffffff21dcfff	globl
scall	getattrlistbulk		0xfffffffff21cdfff	globl
scall	getdirentriesattr	0xfffffffff20defff	globl
scall	gethostuuid		0xfffffffff208efff	globl
scall	getsgroups		0xfffffffff2120fff	globl
scall	getwgroups		0xfffffffff2122fff	globl
scall	grab_pgo_data		0xfffffffff21edfff	globl
scall	guarded_close_np	0xfffffffff21bafff	globl
scall	guarded_kqueue_np	0xfffffffff21bbfff	globl
scall	guarded_open_np		0xfffffffff21b9fff	globl
scall	guarded_pwrite_np	0xfffffffff21e6fff	globl
scall	guarded_write_np	0xfffffffff21e5fff	globl
scall	guarded_writev_np	0xfffffffff21e7fff	globl
scall	identitysvc		0xfffffffff2125fff	globl
scall	initgroups		0xfffffffff20f3fff	globl
scall	iopolicysys		0xfffffffff2142fff	globl
scall	kas_info		0xfffffffff21b7fff	globl
scall	kdebug_trace		0xfffffffff20b3fff	globl
scall	kdebug_trace_string	0xfffffffff20b2fff	globl
scall	kdebug_typefilter	0xfffffffff20b1fff	globl
scall	kevent_id		0xfffffffff2177fff	globl
scall	kevent_qos		0xfffffffff2176fff	globl
scall	ledger			0xfffffffff2175fff	globl
scall	lstat_extended		0xfffffffff2156fff	globl
scall	memorystatus_control	0xfffffffff21b8fff	globl
scall	memorystatus_get_level	0xfffffffff21c5fff	globl
scall	microstackshot		0xfffffffff21ecfff	globl
scall	mkdir_extended		0xfffffffff2124fff	globl
scall	mkfifo_extended		0xfffffffff2123fff	globl
scall	modwatch		0xfffffffff20e9fff	globl
scall	mremap_encrypted	0xfffffffff21e9fff	globl
scall	msgrcv_nocancel		0xfffffffff21a3fff	globl
scall	msgsnd_nocancel		0xfffffffff21a2fff	globl
scall	msync_nocancel		0xfffffffff2195fff	globl
scall	necp_client_action	0xfffffffff21f6fff	globl
scall	necp_match_policy	0xfffffffff21ccfff	globl
scall	necp_open		0xfffffffff21f5fff	globl
scall	necp_session_action	0xfffffffff220bfff	globl
scall	necp_session_open	0xfffffffff220afff	globl
scall	net_qos_guideline	0xfffffffff220dfff	globl
scall	netagent_trigger	0xfffffffff21eafff	globl
scall	nfsclnt			0xfffffffff20f7fff	globl
scall	open_dprotected_np	0xfffffffff20d8fff	globl
scall	open_extended		0xfffffffff2115fff	globl
scall	open_nocancel		0xfffffffff218efff	globl
scall	openat_nocancel		0xfffffffff21d0fff	globl
scall	openbyid_np		0xfffffffff21dffff	globl
scall	os_fault_with_payload	0xfffffffff2211fff	globl
scall	peeloff			0xfffffffff21c1fff	globl
scall	persona			0xfffffffff21eefff	globl
scall	pid_hibernate		0xfffffffff21b3fff	globl
scall	pid_resume		0xfffffffff21b2fff	globl
scall	pid_shutdown_sockets	0xfffffffff21b4fff	globl
scall	pid_suspend		0xfffffffff21b1fff	globl
scall	poll_nocancel		0xfffffffff21a1fff	globl
scall	pread_nocancel		0xfffffffff219efff	globl
scall	proc_info		0xfffffffff2150fff	globl
scall	proc_rlimit_control	0xfffffffff21befff	globl
scall	proc_trace_log		0xfffffffff21ddfff	globl
scall	proc_uuid_policy	0xfffffffff21c4fff	globl
scall	process_policy		0xfffffffff2143fff	globl
scall	pselect_nocancel	0xfffffffff218bfff	globl
scall	psynch_cvbroad		0xfffffffff212ffff	globl
scall	psynch_cvclrprepost	0xfffffffff2138fff	globl
scall	psynch_cvsignal		0xfffffffff2130fff	globl
scall	psynch_mutexdrop	0xfffffffff212efff	globl
scall	psynch_mutexwait	0xfffffffff212dfff	globl
scall	psynch_rw_downgrade	0xfffffffff212bfff	globl
scall	psynch_rw_longrdlock	0xfffffffff2129fff	globl
scall	psynch_rw_rdlock	0xfffffffff2132fff	globl
scall	psynch_rw_unlock	0xfffffffff2134fff	globl
scall	psynch_rw_unlock2	0xfffffffff2135fff	globl
scall	psynch_rw_upgrade	0xfffffffff212cfff	globl
scall	psynch_rw_wrlock	0xfffffffff2133fff	globl
scall	psynch_rw_yieldwrlock	0xfffffffff212afff	globl
scall	pwrite_nocancel		0xfffffffff219ffff	globl
scall	read_nocancel		0xfffffffff218cfff	globl
scall	readv_nocancel		0xfffffffff219bfff	globl
scall	recvfrom_nocancel	0xfffffffff2193fff	globl
scall	recvmsg_nocancel	0xfffffffff2191fff	globl
scall	recvmsg_x		0xfffffffff21e0fff	globl
scall	renameatx_np		0xfffffffff21e8fff	globl
scall	searchfs		0xfffffffff20e1fff	globl
scall	select_nocancel		0xfffffffff2197fff	globl
scall	sem_close		0xfffffffff210dfff	globl
scall	sem_open		0xfffffffff210cfff	globl
scall	sem_post		0xfffffffff2111fff	globl
scall	sem_trywait		0xfffffffff2110fff	globl
scall	sem_unlink		0xfffffffff210efff	globl
scall	sem_wait		0xfffffffff210ffff	globl
scall	sem_wait_nocancel	0xfffffffff21a4fff	globl
scall	sendmsg_nocancel	0xfffffffff2192fff	globl
scall	sendmsg_x		0xfffffffff21e1fff	globl
scall	sendto_nocancel		0xfffffffff219dfff	globl
scall	setattrlist		0xfffffffff20ddfff	globl
scall	setattrlistat		0xfffffffff220cfff	globl
scall	setprivexec		0xfffffffff2098fff	globl
scall	setsgroups		0xfffffffff211ffff	globl
scall	settid			0xfffffffff211dfff	globl
scall	settid_with_pid		0xfffffffff2137fff	globl
scall	setwgroups		0xfffffffff2121fff	globl
scall	sfi_ctl			0xfffffffff21c8fff	globl
scall	sfi_pidctl		0xfffffffff21c9fff	globl
scall	shared_region_check_np	0xfffffffff2126fff	globl
scall	sigsuspend_nocancel	0xfffffffff219afff	globl
scall	socket_delegate		0xfffffffff21c2fff	globl
scall	stat_extended		0xfffffffff2155fff	globl
scall	sysctlbyname		0xfffffffff2112fff	globl
scall	system_override		0xfffffffff21c6fff	globl
scall	telemetry		0xfffffffff21c3fff	globl
scall	terminate_with_payload	0xfffffffff2208fff	globl
scall	thread_selfcounts	0xfffffffff20bafff	globl
scall	thread_selfid		0xfffffffff2174fff	globl
scall	thread_selfusage	0xfffffffff21e2fff	globl
scall	ulock_wait		0xfffffffff2203fff	globl
scall	ulock_wake		0xfffffffff2204fff	globl
scall	umask_extended		0xfffffffff2116fff	globl
scall	usrctl			0xfffffffff21bdfff	globl
scall	vfs_purge		0xfffffffff21c7fff	globl
scall	vm_pressure_monitor	0xfffffffff2128fff	globl
scall	wait4_nocancel		0xfffffffff2190fff	globl
scall	waitevent		0xfffffffff20e8fff	globl
scall	waitid_nocancel		0xfffffffff21a0fff	globl
scall	watchevent		0xfffffffff20e7fff	globl
scall	work_interval_ctl	0xfffffffff21f3fff	globl
scall	workq_kernreturn	0xfffffffff2170fff	globl
scall	workq_open		0xfffffffff216ffff	globl
scall	write_nocancel		0xfffffffff218dfff	globl
scall	writev_nocancel		0xfffffffff219cfff	globl
#──────────────────────────FREEBSD───────────────────────────
scall	abort2			0xffffff1cffffffff	globl
scall	afs3_syscall		0xffffff179fffffff	globl
scall	bindat			0xffffff21afffffff	globl
scall	break			0xffffff011fffffff	globl
scall	cap_enter		0xffffff204fffffff	globl
scall	cap_fcntls_get		0xffffff219fffffff	globl
scall	cap_fcntls_limit	0xffffff218fffffff	globl
scall	cap_getmode		0xffffff205fffffff	globl
scall	cap_ioctls_get		0xffffff217fffffff	globl
scall	cap_ioctls_limit	0xffffff216fffffff	globl
scall	cap_rights_limit	0xffffff215fffffff	globl
scall	clock_getcpuclockid2	0x1e2fff0f7fffffff	globl
scall	connectat		0xffffff21bfffffff	globl
scall	cpuset			0xffffff1e4fffffff	globl
scall	cpuset_getdomain	0xffffff231fffffff	globl
scall	cpuset_getid		0xffffff1e6fffffff	globl
scall	cpuset_setdomain	0xffffff232fffffff	globl
scall	cpuset_setid		0xffffff1e5fffffff	globl
scall	eaccess			0xffffff178fffffff	globl
scall	extattr_delete_fd	0x16efff175fffffff	globl
scall	extattr_delete_file	0x16bfff166fffffff	globl
scall	extattr_delete_link	0x171fff19efffffff	globl
scall	extattr_get_fd		0x16dfff174fffffff	globl
scall	extattr_get_file	0x16afff165fffffff	globl
scall	extattr_get_link	0x170fff19dfffffff	globl
scall	extattr_list_fd		0x172fff1b5fffffff	globl
scall	extattr_list_file	0x173fff1b6fffffff	globl
scall	extattr_list_link	0x174fff1b7fffffff	globl
scall	extattr_set_fd		0x16cfff173fffffff	globl
scall	extattr_set_file	0x169fff164fffffff	globl
scall	extattr_set_link	0x16ffff19cfffffff	globl
scall	extattrctl		0x168fff163fffffff	globl
scall	fexecve			0x1d1fff1ecfffffff	globl
scall	ffclock_getcounter	0xffffff0f1fffffff	globl
scall	ffclock_getestimate	0xffffff0f3fffffff	globl
scall	ffclock_setestimate	0xffffff0f2fffffff	globl
scall	fhlink			0xffffff235fffffff	globl
scall	fhlinkat		0xffffff236fffffff	globl
scall	fhreadlink		0xffffff237fffffff	globl
scall	getaudit		0xffffff1c1fffffff	globl
scall	getcontext		0x133fff1a5fffffff	globl
#scall	getdomainname		0xffff00a2ffffffff	globl
scall	getfhat			0xffffff234fffffff	globl
scall	gethostid		0xffffff08efffffff	globl
#scall	gethostname		0xffff0057ffffffff	globl
scall	getkerninfo		0xffffff03ffffffff	globl
scall	getloginclass		0xffffff20bfffffff	globl
scall	getpagesize_freebsd	0xffffff040fffffff	globl hidden
scall	gssd_syscall		0xffffff1f9fffffff	globl
scall	jail			0xffffff152fffffff	globl
scall	jail_attach		0xffffff1b4fffffff	globl
scall	jail_get		0xffffff1fafffffff	globl
scall	jail_remove		0xffffff1fcfffffff	globl
scall	jail_set		0xffffff1fbfffffff	globl
scall	kenv			0xffffff186fffffff	globl
scall	kldfind			0xffffff132fffffff	globl
scall	kldfirstmod		0xffffff135fffffff	globl
scall	kldload			0xffffff130fffffff	globl
scall	kldnext			0xffffff133fffffff	globl
scall	kldstat			0xffffff134fffffff	globl
scall	kldsym			0xffffff151fffffff	globl
scall	kldunload		0xffffff131fffffff	globl
scall	kldunloadf		0xffffff1bcfffffff	globl
scall	kmq_notify		0xffffff1cdfffffff	globl
scall	kmq_setattr		0xffffff1cafffffff	globl
scall	kmq_timedreceive	0xffffff1cbfffffff	globl
scall	kmq_timedsend		0xffffff1ccfffffff	globl
scall	kmq_unlink		0xffffff1cefffffff	globl
scall	ksem_close		0xffffff190fffffff	globl
scall	ksem_destroy		0xffffff198fffffff	globl
scall	ksem_getvalue		0xffffff197fffffff	globl
scall	ksem_init		0xffffff194fffffff	globl
scall	ksem_open		0xffffff195fffffff	globl
scall	ksem_post		0xffffff191fffffff	globl
scall	ksem_timedwait		0xffffff1b9fffffff	globl
scall	ksem_trywait		0xffffff193fffffff	globl
scall	ksem_unlink		0xffffff196fffffff	globl
scall	ksem_wait		0xffffff192fffffff	globl
scall	lchflags		0x130fff187fffffff	globl
scall	lchmod			0x112fff112fffffff	globl
scall	lgetfh			0xffffff0a0fffffff	globl
scall	lpathconf		0x1f3fff201fffffff	globl
scall	lutimes			0x1a8fff114fffffff	globl
scall	mac_syscall		0xffffff18afffffff	globl
scall	modfind			0xffffff12ffffffff	globl
scall	modfnext		0xffffff12efffffff	globl
scall	modnext			0xffffff12cfffffff	globl
scall	modstat			0xffffff12dfffffff	globl
scall	nfstat			0xffffff117fffffff	globl
scall	nlm_syscall		0xffffff09afffffff	globl
scall	nlstat			0xffffff118fffffff	globl
scall	nmount			0xffffff17afffffff	globl
scall	nnpfs_syscall		0xffffff153fffffff	globl
scall	nstat			0xffffff116fffffff	globl
scall	pdfork			0xffffff206fffffff	globl
scall	pdgetpid		0xffffff208fffffff	globl
scall	pdkill			0xffffff207fffffff	globl
scall	sys_posix_openpt	0xffffff1f8fffffff	globl hidden
scall	procctl			0xffffff220fffffff	globl
scall	psynch_cvwait		0xfffffffff2131fff	globl
scall	quota			0xffffff095fffffff	globl
scall	rctl_add_rule		0xffffff210fffffff	globl
scall	rctl_get_limits		0xffffff20ffffffff	globl
scall	rctl_get_racct		0xffffff20dfffffff	globl
scall	rctl_get_rules		0xffffff20efffffff	globl
scall	rctl_remove_rule	0xffffff211fffffff	globl
scall	recv			0xffffff066fffffff	globl
scall	rfork			0xffffff0fbfffffff	globl
scall	rtprio			0xffffff0a6fffffff	globl
scall	rtprio_thread		0xffffff1d2fffffff	globl
#scall	send			0xffffff065fffffff	globl
scall	setaudit		0xffffff1c2fffffff	globl
scall	setcontext		0x134fff1a6fffffff	globl
scall	setfib			0xffffff0affffffff	globl
scall	sethostid		0xffffff08ffffffff	globl
scall	setloginclass		0xffffff20cfffffff	globl
scall	sigblock		0xffffff06dfffffff	globl
scall	sigsetmask		0xffffff06efffffff	globl
scall	sigstack		0xffffff070fffffff	globl
scall	sigvec			0xffffff06cfffffff	globl
scall	sigwaitinfo		0xffffff15afffffff	globl
scall	sstk			0xffffff046fffffff	globl
scall	swapcontext		0xffffff1a7fffffff	globl
scall	thr_create		0xffffff1aefffffff	globl
scall	thr_exit		0xffffff1affffffff	globl
scall	thr_kill		0xffffff1b1fffffff	globl
scall	thr_kill2		0xffffff1e1fffffff	globl
scall	thr_new			0xffffff1c7fffffff	globl
scall	thr_self		0xffffff1b0fffffff	globl
scall	thr_set_name		0xffffff1d0fffffff	globl
scall	thr_suspend		0xffffff1bafffffff	globl
scall	thr_wake		0xffffff1bbfffffff	globl
scall	uuidgen			0x163fff188fffffff	globl
scall	vadvise			0xffffff048fffffff	globl
scall	wait			0xffffff054fffffff	globl
scall	wait6			0x1e1fff214fffffff	globl
scall	yield			0xffffff141fffffff	globl
#──────────────────────────OPENBSD───────────────────────────
scall	__tfork			0xfff008ffffffffff	globl
scall	__thrsleep		0xfff05effffffffff	globl
scall	__thrwakeup		0xfff12dffffffffff	globl
scall	__threxit		0xfff12effffffffff	globl
scall	__thrsigdivert		0xfff12fffffffffff	globl
scall	__set_tcb		0xfff149ffffffffff	globl
scall	__get_tcb		0xfff14affffffffff	globl
scall	adjfreq			0xfff131ffffffffff	globl
scall	getdtablecount		0xfff012ffffffffff	globl
scall	getlogin_r		0xfff08dffffffffff	globl
scall	getrtable		0xfff137ffffffffff	globl
scall	getthrid		0xfff12bffffffffff	globl
scall	kbind			0xfff056ffffffffff	globl
scall	mquery			0xfff11effffffffff	globl # openbsd:pad
scall	obreak			0x011011ffffffffff	globl
scall	sendsyslog		0xfff070ffffffffff	globl
scall	setrtable		0xfff136ffffffffff	globl
scall	swapctl			0x10f0c1ffffffffff	globl
scall	thrkill			0xfff077ffffffffff	globl
scall	unveil			0xfff072ffffffffff	globl

#	The Fifth Bell System Interface, Community Edition
#	» beyond the pale
#	                             GNU/Systemd┐
#	                             Mac OS X┐  │
#	                          FreeBSD┐   │  │
#	                       OpenBSD┐  │ ┌─│──│── XnuClass{1:Mach,2:Unix}
#	                     NetBSD┐  │  │ │ │  │
#	Symbol                    ┌┴┐┌┴┐┌┴┐│┬┴┐┌┴┐      Directives & Commentary
scall	__mac_get_link		0xffffff19a2180fff	globl
scall	__mac_set_link		0xffffff19b2181fff	globl
scall	__mac_get_fd		0xffffff1822184fff	globl
scall	__mac_get_file		0xffffff183217efff	globl
scall	__mac_get_proc		0xffffff1802182fff	globl
scall	__mac_set_fd		0xffffff1842185fff	globl
scall	__mac_get_pid		0xffffff1992186fff	globl
scall	__mac_set_proc		0xffffff1812183fff	globl
scall	__mac_set_file		0xffffff185217ffff	globl
scall	__mac_execve		0xffffff19f217cfff	globl
scall	__acl_get_link		0xffffff1a9fffffff	globl
scall	__sigwait_nocancel	0xfffffffff21a6fff	globl
scall	__cap_rights_get	0xffffff203fffffff	globl
scall	__semwait_signal	0xfffffffff214efff	globl
scall	__acl_set_link		0xffffff1aafffffff	globl
scall	__acl_set_fd		0xffffff15efffffff	globl
scall	__old_semwait_signal	0xfffffffff2172fff	globl
scall	__setugid		0xffffff176fffffff	globl
scall	__acl_aclcheck_fd	0xffffff162fffffff	globl
scall	__acl_get_fd		0xffffff15dfffffff	globl
scall	__sysctl		0xffffff0cafffffff	globl
scall	__mac_getfsstat		0xfffffffff21aafff	globl
scall	__mac_get_mount		0xfffffffff21a9fff	globl
scall	__acl_delete_link	0xffffff1abfffffff	globl
scall	__mac_mount		0xfffffffff21a8fff	globl
scall	__acl_get_file		0xffffff15bfffffff	globl
scall	__acl_aclcheck_file	0xffffff161fffffff	globl
scall	__acl_delete_fd		0xffffff160fffffff	globl
scall	__acl_aclcheck_link	0xffffff1acfffffff	globl
scall	__mac_syscall		0xfffffffff217dfff	globl
scall	__acl_set_file		0xffffff15cfffffff	globl
scall	__acl_delete_file	0xffffff15ffffffff	globl
scall	__syscall		0xfff0c6ffffffffff	globl
scall	_umtx_op		0xffffff1c6fffffff	globl
scall	__semwait_signal_nocancel	0xfffffffff21a7fff	globl
scall	__old_semwait_signal_nocancel	0xfffffffff2173fff	globl
scall	sctp_peeloff			0xffffff1d7fffffff	globl
scall	sctp_generic_recvmsg		0xffffff1dafffffff	globl
scall	sctp_generic_sendmsg		0xffffff1d8fffffff	globl
scall	sctp_generic_sendmsg_iov	0xffffff1d9fffffff	globl
scall	shared_region_map_and_slide_np	0xfffffffff21b6fff	globl
scall	guarded_open_dprotected_np	0xfffffffff21e4fff	globl
scall	stack_snapshot_with_config	0xfffffffff21ebfff	globl
