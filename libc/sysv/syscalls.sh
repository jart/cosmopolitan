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
#	                                 Systemd┐              │ ffff │ unavailable      │
# 	                                 2.6.18+│              │    $ │ wrapped          │
#	                            Mac OS X┐   │              │   __ │ wrapped twice    │
#	                               15.6+│   │              └─────────────────────────┘
#	                         FreeBSD┐   │   │
#	                             12+│ ┌─│───│── XnuClass{1:Mach,2:Unix}
#	                     OpenBSD┐   │ │ │   │
#	                         6.4│   │ │ │   │
#	Symbol                    ┌─┴┐┌─┴┐│┬┴┐┌─┴┐      Directives & Commentary
scall	'exit$sysv'		0x00010001200100e7	globl hidden # a.k.a. exit_group
scall	'read$sysv'		0x0003000320030000	globl hidden
scall	'write$sysv'		0x0004000420040001	globl hidden
scall	'open$sysv'		0x0005000520050002	globl hidden
scall	'close$sysv'		0x0006000620060003	globl hidden
scall	'__stat$sysv'		0x0026ffff21520004	globl hidden # FreeBSD 11→12 fumble; use fstatat$sysv(); blocked on Android
scall	'__fstat$sysv'		0x0035022721530005	globl hidden # needs stat2linux()
scall	'__lstat$sysv'		0x0028002821540006	globl hidden # needs stat2linux(); blocked on Android
scall	'poll$sysv'		0x00fc00d120e60007	globl hidden
scall	'ppoll$sysv'		0x006d0221ffff010f	globl hidden # consider INTON/INTOFF tutorial in examples/unbourne.c
scall	'__lseek$sysv'		0x00c701de20c70008	globl hidden # openbsd:evilpad
scall	'__mmap$sysv'		0x00c501dd20c50009	globl hidden # openbsd:pad
scall	'msync$sysv'		0x010000412041001a	globl hidden
scall	'mprotect$sysv'		0x004a004a204a000a	globl hidden
scall	'munmap$sysv'		0x004900492049000b	globl hidden
scall	'sigaction$sysv'	0x002e01a0202e000d	globl hidden # rt_sigaction on Lunix
scall	'sigprocmask$sysv'	0x003001542030000e	globl hidden # a.k.a. rt_sigprocmask
scall	'ioctl$sysv'		0x0036003620360010	globl hidden
scall	'__pread$sysv'		0x00ad01db20990011	globl hidden # a.k.a. pread64; openbsd:pad
scall	'__pwrite$sysv'		0x00ae01dc209a0012	globl hidden # a.k.a. pwrite64; openbsd:pad
scall	'readv$sysv'		0x0078007820780013	globl hidden
scall	'writev$sysv'		0x0079007920790014	globl hidden
scall	'access$sysv'		0x0021002120210015	globl hidden
scall	'__pipe$sysv'		0x0107021e202a0016	globl hidden # NOTE: pipe2() on FreeBSD; XNU is pipe(void)→eax:edx
scall	'select$sysv'		0x0047005d205d0017	globl hidden
scall	pselect			0x006e020a218affff	globl
scall	pselect6		0xffffffffffff010e	globl
scall	'sched_yield$sysv'	0x012a014b103c0018	globl hidden # swtch() on xnu
scall	'mremap$sysv'		0xffffffffffff0019	globl hidden
scall	mincore			0x004e004e204e001b	globl
scall	'madvise$sysv'		0x004b004b204b001c	globl hidden
scall	shmget			0x012100e72109001d	globl # consider mmap
scall	shmat			0x00e400e42106001e	globl # consider mmap
scall	shmctl			0x012802002107001f	globl # consider mmap
scall	'dup$sysv'		0x0029002920290020	globl hidden
scall	'dup2$sysv'		0x005a005a205a0021	globl hidden
scall	'pause$sysv'		0xffffffffffff0022	globl hidden
scall	'nanosleep$sysv'	0x005b00f0ffff0023	globl hidden
scall	'getitimer$sysv'	0x0046005620560024	globl hidden
scall	'setitimer$sysv'	0x0045005320530026	globl hidden
scall	'alarm$sysv'		0xffffffffffff0025	globl hidden
scall	'getpid$sysv'		0x0014001420140027	globl hidden
scall	'sendfile$sysv'		0xffff018921510028	globl hidden # Linux vs. XNU/BSD ABIs very different
scall	'__socket$sysv'		0x0061006120610029	globl hidden
scall	'__connect$sysv'	0x006200622062002a	globl hidden
scall	'__accept$sysv'		0x001e0063201e002b	globl hidden
scall	'sendto$sysv'		0x008500852085002c	globl hidden
scall	'recvfrom$sysv'		0x001d001d201d002d	globl hidden
scall	'sendmsg$sysv'		0x001c001c201c002e	globl hidden
scall	'recvmsg$sysv'		0x001b001b201b002f	globl hidden
scall	'shutdown$sysv'		0x0086008620860030	globl hidden
scall	'bind$sysv'		0x0068006820680031	globl hidden
scall	'listen$sysv'		0x006a006a206a0032	globl hidden
scall	'__getsockname$sysv'	0x0020002020200033	globl hidden
scall	'__getpeername$sysv'	0x001f008d201f0034	globl hidden
scall	'socketpair$sysv'	0x0087008720870035	globl hidden
scall	'setsockopt$sysv'	0x0069006920690036	globl hidden
scall	'getsockopt$sysv'	0x0076007620760037	globl hidden
scall	'fork$sysv'		0x0002000220020039	globl hidden
#scall	vfork			0x004200422042003a	globl # needs to be called via vfork.S
scall	posix_spawn		0xffffffff20f4ffff	globl hidden # TODO: put in spawnve()
scall	'execve$sysv'		0x003b003b203b003b	globl hidden
scall	'wait4$sysv'		0x000b00072007003d	globl hidden
scall	'kill$sysv'		0x007a00252025003e	globl hidden # kill(pid, sig, 1) b/c xnu
scall	'killpg$sysv'		0xffff0092ffffffff	globl hidden
scall	clone			0xffffffffffff0038	globl
scall	tkill			0xffffffffffff00c8	globl
scall	futex			0x0053ffffffff00ca	globl
scall	set_robust_list		0xffffffffffff0111	globl
scall	get_robust_list		0xffffffffffff0112	globl
scall	'uname$sysv'		0xffff00a4ffff003f	globl hidden
scall	semget			0x00dd00dd20ff0040	globl # won't polyfill for windows
scall	semop			0x012200de21000041	globl # won't polyfill for windows
scall	semctl			0x012701fe20fe0042	globl # won't polyfill for windows
scall	shmdt			0x00e600e621080043	globl # won't polyfill for windows
scall	msgget			0x00e100e121030044	globl # won't polyfill for windows
scall	msgsnd			0x00e200e221040045	globl # won't polyfill for windows
scall	msgrcv			0x00e300e321050046	globl # won't polyfill for windows
scall	msgctl			0x012901ff21020047	globl # won't polyfill for windows
scall	'fcntl$sysv'		0x005c005c205c0048	globl hidden
scall	'flock$sysv'		0x0083008320830049	globl hidden
scall	'fsync$sysv'		0x005f005f205f004a	globl hidden
scall	'fdatasync$sysv'	0x005f022620bb004b	globl hidden # fsync() on openbsd
scall	'__truncate$sysv'	0x00c801df20c8004c	globl hidden # openbsd:pad
scall	'__ftruncate$sysv'	0x00c901e020c9004d	globl hidden # openbsd:pad
scall	'getcwd$sysv'		0x01300146ffff004f	globl hidden
scall	'chdir$sysv'		0x000c000c200c0050	globl hidden
scall	fchdir			0x000d000d200d0051	globl
scall	'rename$sysv'		0x0080008020800052	globl hidden
scall	'mkdir$sysv'		0x0088008820880053	globl hidden
scall	'rmdir$sysv'		0x0089008920890054	globl hidden
scall	'creat$sysv'		0xffff0008ffff0055	globl hidden
scall	'link$sysv'		0x0009000920090056	globl hidden
scall	'unlink$sysv'		0x000a000a200a0057	globl hidden
scall	'symlink$sysv'		0x0039003920390058	globl hidden
scall	readlink		0x003a003a203a0059	globl        # usually an anti-pattern
scall	'chmod$sysv'		0x000f000f200f005a	globl hidden
scall	'fchmod$sysv'		0x007c007c207c005b	globl hidden
scall	'chown$sysv'		0x001000102010005c	globl hidden # impl. w/ fchownat() @asyncsignalsafe
scall	'fchown$sysv'		0x007b007b207b005d	globl hidden # @asyncsignalsafe
scall	'lchown$sysv'		0x00fe00fe216c005e	globl hidden # impl. w/ fchownat()
scall	umask			0x003c003c203c005f	globl
scall	'__gettimeofday$sysv'	0x0043007420740060	globl hidden # xnu esi/edx=0
scall	'getrlimit$sysv'	0x00c200c220c20061	globl hidden
scall	'getrusage$sysv'	0x0013007520750062	globl hidden
scall	'sysinfo$sysv'		0xffffffffffff0063	globl hidden
scall	'times$sysv'		0xffffffffffff0064	globl hidden
scall	ptrace			0x001a001a201a0065	globl
scall	syslog			0xffffffffffff0067	globl
scall	'getuid$sysv'		0x0018001820180066	globl hidden
scall	'getgid$sysv'		0x002f002f202f0068	globl hidden
scall	'getppid$sysv'		0x002700272027006e	globl hidden
scall	getpgrp			0x005100512051006f	globl
scall	'setsid$sysv'		0x0093009320930070	globl hidden
scall	'getsid$sysv'		0x00ff01362136007c	globl hidden
scall	getpgid			0x00cf00cf20970079	globl
scall	setpgid			0x005200522052006d	globl
scall	geteuid			0x001900192019006b	globl
scall	getegid			0x002b002b202b006c	globl
scall	getgroups		0x004f004f204f0073	globl
scall	setgroups		0x0050005020500074	globl
scall	setreuid		0x007e007e207e0071	globl
scall	setregid		0x007f007f207f0072	globl
scall	setuid			0x0017001720170069	globl
scall	setgid			0x00b500b520b5006a	globl
scall	'setresuid$sysv'	0x011a0137ffff0075	globl hidden # polyfilled for xnu
scall	'setresgid$sysv'	0x011c0138ffff0077	globl hidden # polyfilled for xnu
scall	getresuid		0x01190168ffff0076	globl # semantics aren't well-defined
scall	getresgid		0x011b0169ffff0078	globl # semantics aren't well-defined
scall	sigpending		0x003400342034007f	globl
scall	'sigsuspend$sysv'	0x006f0155206f0082	globl hidden
scall	sigaltstack		0x0120003520350083	globl
scall	'mknod$sysv'		0x000e000e200e0085	globl hidden
scall	mknodat			0x014022ffffff0103	globl # FreeBSD 12+
scall	'mkfifo$sysv'		0x008400842084ffff	globl hidden
scall	mkfifoat		0x013f01f1ffffffff	globl
scall	statfs			0x003f022b21590089	globl
scall	fstatfs			0x0040022c215a008a	globl
scall	'getpriority$sysv'	0x006400642064008c	globl hidden
scall	'setpriority$sysv'	0x006000602060008d	globl hidden # modern nice()
scall	mlock			0x00cb00cb20cb0095	globl
scall	munlock			0x00cc00cc20cc0096	globl
scall	mlockall		0x010f014421440097	globl
scall	munlockall		0x0110014521450098	globl
scall	'setrlimit$sysv'	0x00c300c320c300a0	globl hidden
scall	chroot			0x003d003d203d00a1	globl
scall	'sync$sysv'		0x00240024202400a2	globl hidden
scall	acct			0x00330033203300a3	globl
scall	settimeofday		0x0044007a207a00a4	globl
scall	mount			0x0015001520a700a5	globl
scall	reboot			0x00370037203700a9	globl
scall	quotactl		0x0094009420a500b3	globl
scall	setfsuid		0xffffffffffff007a	globl
scall	setfsgid		0xffffffffffff007b	globl
scall	capget			0xffffffffffff007d	globl
scall	capset			0xffffffffffff007e	globl
scall	sigtimedwait		0xffff0159ffff0080	globl
scall	rt_sigqueueinfo		0xffffffffffff0081	globl
scall	personality		0xffffffffffff0087	globl
scall	ustat			0xffffffffffff0088	globl
scall	sysfs			0xffffffffffff008b	globl
scall	sched_setparam		0xffff0147ffff008e	globl
scall	sched_getparam		0xffff0148ffff008f	globl
scall	sched_setscheduler	0xffff0149ffff0090	globl
scall	sched_getscheduler	0xffff014affff0091	globl
scall	sched_get_priority_max	0xffff014cffff0092	globl
scall	sched_get_priority_min	0xffff014dffff0093	globl
scall	sched_rr_get_interval	0xffff014effff0094	globl
scall	vhangup			0xffffffffffff0099	globl
scall	modify_ldt		0xffffffffffff009a	globl
scall	pivot_root		0xffffffffffff009b	globl
scall	_sysctl			0xffffffffffff009c	globl
scall	prctl			0xffffffffffff009d	globl
scall	'arch_prctl$sysv'	0x00a500a5ffff009e	globl hidden # sysarch() on bsd
scall	adjtimex		0xffffffffffff009f	globl
scall	umount2			0xffffffffffff00a6	globl
scall	swapon			0xffff0055205500a7	globl
scall	swapoff			0xffff01a8ffff00a8	globl
scall	sethostname		0xffff0058ffff00aa	globl
scall	setdomainname		0xffff00a3ffff00ab	globl
scall	iopl			0xffffffffffff00ac	globl
scall	ioperm			0xffffffffffff00ad	globl
scall	init_module		0xffffffffffff00af	globl
scall	delete_module		0xffffffffffff00b0	globl
scall	'gettid$sysv'		0xffffffff211e00ba	globl hidden
scall	readahead		0xffffffffffff00bb	globl # consider fadvise() / madvise()
scall	setxattr		0xffffffff20ec00bc	globl
scall	fsetxattr		0xffffffff20ed00be	globl
scall	getxattr		0xffffffff20ea00bf	globl
scall	fgetxattr		0xffffffff20eb00c1	globl
scall	listxattr		0xffffffff20f000c2	globl
scall	flistxattr		0xffffffff20f100c4	globl
scall	removexattr		0xffffffff20ee00c5	globl
scall	fremovexattr		0xffffffff20ef00c7	globl
scall	lsetxattr		0xffffffffffff00bd	globl
scall	lgetxattr		0xffffffffffff00c0	globl
scall	llistxattr		0xffffffffffff00c3	globl
scall	lremovexattr		0xffffffffffff00c6	globl
scall  'sched_setaffinity$sysv'	0xffffffffffff00cb	globl hidden
scall	sched_getaffinity	0xffffffffffff00cc	globl
scall	cpuset_getaffinity	0xffff01e7ffffffff	globl
scall	cpuset_setaffinity	0xffff01e8ffffffff	globl
scall	io_setup		0xffffffffffff00ce	globl
scall	io_destroy		0xffffffffffff00cf	globl
scall	io_getevents		0xffffffffffff00d0	globl
scall	io_submit		0xffffffffffff00d1	globl
scall	io_cancel		0xffffffffffff00d2	globl
scall	lookup_dcookie		0xffffffffffff00d4	globl
scall	'epoll_create$sysv'	0xffffffffffff00d5	globl
scall	'epoll_wait$sysv'	0xffffffffffff00e8	globl
scall	'epoll_ctl$sysv'	0xffffffffffff00e9	globl
scall	getdents		0x00630110ffff00d9	globl hidden # getdents64 on linux
scall	set_tid_address		0xffffffffffff00da	globl
scall	restart_syscall		0xffffffffffff00db	globl
scall	semtimedop		0xffffffffffff00dc	globl
scall	'fadvise$sysv'		0xffff0213ffff00dd	globl hidden
scall	timer_create		0xffffffffffff00de	globl
scall	timer_settime		0xffffffffffff00df	globl
scall	timer_gettime		0xffffffffffff00e0	globl
scall	timer_getoverrun	0xffffffffffff00e1	globl
scall	timer_delete		0xffffffffffff00e2	globl
scall	clock_settime		0x005800e9ffff00e3	globl
scall	'clock_gettime$sysv'	0x005700e8ffff00e4	globl hidden # Linux 2.6+ (c. 2003); XNU uses magic address
scall	clock_getres		0x005900eaffff00e5	globl
scall	clock_nanosleep		0xffff00f4ffff00e6	globl
scall	tgkill			0xffffffffffff00ea	globl
scall	mbind			0xffffffffffff00ed	globl
scall	set_mempolicy		0xffffffffffff00ee	globl
scall	get_mempolicy		0xffffffffffff00ef	globl
scall	mq_open			0xffffffffffff00f0	globl # won't polyfill
scall	mq_unlink		0xffffffffffff00f1	globl # won't polyfill
scall	mq_timedsend		0xffffffffffff00f2	globl # won't polyfill
scall	mq_timedreceive		0xffffffffffff00f3	globl # won't polyfill
scall	mq_notify		0xffffffffffff00f4	globl # won't polyfill
scall	mq_getsetattr		0xffffffffffff00f5	globl # won't polyfill
scall	kexec_load		0xffffffffffff00f6	globl
scall	waitid			0xffffffff20ad00f7	globl # Linux 2.6.9+
scall	add_key			0xffffffffffff00f8	globl
scall	request_key		0xffffffffffff00f9	globl
scall	keyctl			0xffffffffffff00fa	globl
scall	ioprio_set		0xffffffffffff00fb	globl
scall	ioprio_get		0xffffffffffff00fc	globl
scall	inotify_init		0xffffffffffff00fd	globl # wicked
scall	inotify_add_watch	0xffffffffffff00fe	globl
scall	inotify_rm_watch	0xffffffffffff00ff	globl
scall	'openat$sysv'		0x014101f321cf0101	globl hidden # Linux 2.6.16+ (c. 2007)
scall	'mkdirat$sysv'		0x013e01f021db0102	globl hidden
scall	'fchownat$sysv'		0x013b01eb21d40104	globl hidden # @asyncsignalsafe
scall	'utime$sysv'		0xffffffffffff0084	globl hidden
scall	'utimes$sysv'		0x004c008a208a00eb	globl hidden
scall	'futimesat$sysv'	0xffff01eeffff0105	globl hidden # @asyncsignalsafe
scall	'futimes$sysv'		0x004d00ce208bffff	globl hidden
scall	'futimens$sysv'		0x00550222ffffffff	globl hidden
scall	'__fstatat$sysv'	0x002a022821d60106	globl hidden # a.k.a. newfstatat(); FreeBSD 12+; needs stat2linux()
scall	'unlinkat$sysv'		0x014501f721d80107	globl hidden
scall	'renameat$sysv'		0x014301f521d10108	globl hidden
scall	'linkat$sysv'		0x013d01ef21d70109	globl hidden
scall	'symlinkat$sysv'	0x014401f621da010a	globl hidden
scall	'readlinkat$sysv'	0x014201f421d9010b	globl hidden
scall	'fchmodat$sysv'		0x013a01ea21d3010c	globl hidden
scall	'faccessat$sysv'	0x013901e921d2010d	globl hidden
scall	unshare			0xffffffffffff0110	globl
scall	'splice$sysv'		0xffffffffffff0113	globl hidden # Linux 2.6.17+ (c. 2007)
scall	tee			0xffffffffffff0114	globl        # Linux 2.6.17+
scall	'sync_file_range$sysv'	0xffffffffffff0115	globl hidden # Linux 2.6.17+
scall	'vmsplice$sysv'		0xffffffffffff0116	globl hidden
scall	migrate_pages		0xffffffffffff0100	globl        # numa numa yay
scall	move_pages		0xffffffffffff0117	globl        # NOTE: We view Red Hat versions as "epochs" for all distros.
#──────────────────────RHEL 5.0 LIMIT────────────────────────        # ←┬─ last gplv2 distro w/ sysv init was rhel5 c. 2007
scall	'__preadv$sysv'		0x010b0121ffff0127	globl hidden #  ├─ cosmopolitan at minimum requires rhel5
scall	'__pwritev$sysv'	0x010c0122ffff0128	globl hidden #  ├─ python modules need to work on this (pep513)
scall	'__utimensat$sysv'	0x00540223ffff0118	globl hidden #  └─ end of life 2020-11-30 (extended)
scall	'fallocate$sysv'	0xffffffffffff011d	globl hidden
scall	'posix_fallocate$sysv'	0xffff0212ffffffff	globl hidden
scall	'__accept4$sysv'	0x005d021dffff0120	globl hidden # Linux 2.6.28+
scall	'__dup3$sysv'		0x0066ffffffff0124	globl hidden # Linux 2.6.27+
scall	'__pipe2$sysv'		0x0065021effff0125	globl hidden # Linux 2.6.27+
scall	epoll_pwait		0xffffffffffff0119	globl
scall	'epoll_create1$sysv'	0xffffffffffff0123	globl
scall	perf_event_open		0xffffffffffff012a	globl
scall	inotify_init1		0xffffffffffff0126	globl
scall	rt_tgsigqueueinfo	0xffffffffffff0129	globl
scall	signalfd		0xffffffffffff011a	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	signalfd4		0xffffffffffff0121	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	eventfd			0xffffffffffff011c	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	eventfd2		0xffffffffffff0122	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	timerfd_create		0xffffffffffff011b	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	timerfd_settime		0xffffffffffff011e	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	timerfd_gettime		0xffffffffffff011f	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
#──────────────────────RHEL 6.0 LIMIT──────────────────────── # ←┬─ modern glibc at minimum requires rhel6+
scall	recvmmsg		0xffffffffffff012b	globl #  └─ end of life 2024-06-30 (extended)
scall	fanotify_init		0xffffffffffff012c	globl
scall	fanotify_mark		0xffffffffffff012d	globl
scall	prlimit			0xffffffffffff012e	globl
scall	name_to_handle_at	0xffffffffffff012f	globl
scall	open_by_handle_at	0xffffffffffff0130	globl
scall	clock_adjtime		0xffffffffffff0131	globl
scall	syncfs			0xffffffffffff0132	globl
scall	sendmmsg		0xffffffffffff0133	globl
scall	setns			0xffffffffffff0134	globl
scall	getcpu			0xffffffffffff0135	globl
scall	process_vm_readv	0xffffffffffff0136	globl
scall	process_vm_writev	0xffffffffffff0137	globl
scall	kcmp			0xffffffffffff0138	globl
scall	finit_module		0xffffffffffff0139	globl
#──────────────────────RHEL 7.0 LIMIT──────────────────────── # ←┬─ distros switched to systemd
scall	sched_setattr		0xffffffffffff013a	globl #  └─ C++11 needs this
scall	sched_getattr		0xffffffffffff013b	globl
scall	renameat2		0xffffffffffff013c	globl
scall	seccomp			0xffffffffffff013d	globl
scall	'getrandom$sysv'	0x0007023321f4013e	globl hidden  # Linux 3.17+ and getentropy() on XNU/OpenBSD
scall	memfd_create		0xffffffffffff013f	globl # wut
scall	kexec_file_load		0xffffffffffff0140	globl
scall	bpf			0xffffffffffff0141	globl
scall	execveat		0xffffffffffff0142	globl
scall	userfaultfd		0xffffffffffff0143	globl # Linux 4.3+ (c. 2015)
scall	membarrier		0xffffffffffff0144	globl # Linux 4.3+ (c. 2015)
scall	mlock2			0xffffffffffff0145	globl # Linux 4.5+ (c. 2016)
scall	'copy_file_range$sysv'	0xffffffffffff0146	globl hidden # Linux 4.5+ (c. 2016)
scall	preadv2			0xffffffffffff0147	globl
scall	pwritev2		0xffffffffffff0148	globl
scall	pkey_mprotect		0xffffffffffff0149	globl
scall	pkey_alloc		0xffffffffffff014a	globl
scall	pkey_free		0xffffffffffff014b	globl
scall	statx			0xffffffffffff014c	globl # lool https://lkml.org/lkml/2010/7/22/249
scall	io_pgetevents		0xffffffffffff014d	globl
scall	rseq			0xffffffffffff014e	globl # Linux 4.18+ (c. 2018)
#──────────────────────LINUX 4.18 LIMIT────────────────────── # ← last kernel buildable w/ gplv2 libraries
scall	pidfd_send_signal	0xffffffffffff01a8	globl # won't polyfill; see INTON/INTOFF tutorial in examples/unbourne.c
scall	io_uring_setup		0xffffffffffff01a9	globl # Linux 5.1+ (c. 2019)
scall	io_uring_enter		0xffffffffffff01aa	globl # Linux 5.1+ (c. 2019)
scall	io_uring_register	0xffffffffffff01ab	globl # Linux 5.1+ (c. 2019)
scall	pledge			0x006cffffffffffff	globl # it's cross-platorm if you ignore the return code

#	The Fifth Bell System Interface, Community Edition
#	» besiyata dishmaya
#	                             GNU/Systemd┐
#	                            Mac OS X┐   │
#	                         FreeBSD┐   │   │
#	                     OpenBSD┐   │ ┌─│───│── XnuClass{1:Mach,2:Unix}
#	                          ┌─┴┐┌─┴┐│┌┴┐┌─┴┐
scall	ktrace			0x002d002dffffffff	globl
scall	kqueue			0x010d016a216affff	globl
scall	kevent			0x004802302171ffff	globl
scall	revoke			0x003800382038ffff	globl
scall	setlogin		0x003200322032ffff	globl
scall	getfh			0x00a100a120a1ffff	globl
scall	chflags			0x002200222022ffff	globl
scall	getfsstat		0x003e022d215bffff	globl
scall	nfssvc			0x009b009b209bffff	globl
scall	adjtime			0x008c008c208cffff	globl
scall	fchflags		0x002300232023ffff	globl
scall	'__seteuid$bsd'		0x00b700b720b7ffff	globl hidden # wrapped via setreuid()
scall	'__setegid$bsd'		0x00b600b620b6ffff	globl hidden # wrapped via setregid()
scall	fpathconf		0x00c000c020c0ffff	globl
scall	fhopen			0x0108012a20f8ffff	globl
scall	unmount			0x00160016209fffff	globl
scall	issetugid		0x00fd00fd2147ffff	globl
scall	minherit		0x00fa00fa20faffff	globl
scall	pathconf		0x00bf00bf20bfffff	globl
scall	sysctl			0x00caffff20caffff	globl
#───────────────────────XNU & FREEBSD────────────────────────
scall	ntp_adjtime		0xffff00b0220fffff	globl
scall	ntp_gettime		0xffff00f82210ffff	globl
scall	shm_unlink		0xffff01e3210bffff	globl
scall	shm_open		0xffff01e2210affff	globl
scall	aio_read		0xffff013e213effff	globl
scall	aio_suspend		0xffff013b213bffff	globl
scall	aio_cancel		0xffff013c213cffff	globl
scall	aio_fsync		0xffff01d12139ffff	globl
scall	aio_error		0xffff013d213dffff	globl
scall	aio_return		0xffff013a213affff	globl
scall	aio_write		0xffff013f213fffff	globl
scall	aio_waitcomplete	0xffff0167ffffffff	globl
scall	aio_suspend_nocancel	0xffffffff21a5ffff	globl
scall	aio_mlock		0xffff021fffffffff	globl
scall	sigwait			0xffff01ad214affff	globl
scall	undelete		0xffff00cd20cdffff	globl
scall	getlogin		0xffff00312031ffff	globl
scall	getdtablesize		0xffff00592059ffff	globl
scall	setauid			0xffff01c02162ffff	globl
scall	audit			0xffff01bd215effff	globl
scall	auditctl		0xffff01c52167ffff	globl
scall	getaudit_addr		0xffff01c32165ffff	globl
scall	getdirentries		0xffff022a2158ffff	globl
scall	lio_listio		0xffff01402140ffff	globl
scall	setaudit_addr		0xffff01c42166ffff	globl
scall	getauid			0xffff01bf2161ffff	globl
scall	semsys			0xffff00a920fbffff	globl
scall	auditon			0xffff01be215fffff	globl
scall	msgsys			0xffff00aa20fcffff	globl
scall	shmsys			0xffff00ab20fdffff	globl
#─────────────────────FREEBSD & OPENBSD──────────────────────
scall	fhstat			0x01260229ffffffff	globl
scall	chflagsat		0x006b021cffffffff	globl
scall	profil			0x002c002cffffffff	globl
scall	fhstatfs		0x0041022effffffff	globl
scall	utrace			0x00d1014fffffffff	globl
scall	closefrom		0x011f01fdffffffff	globl
#───────────────────────────XNU──────────────────────────────
scall	__pthread_markcancel	0xffffffff214cffff	globl
scall	__pthread_kill		0xffffffff2148ffff	globl
scall	__pthread_fchdir	0xffffffff215dffff	globl
scall	__pthread_sigmask	0xffffffff2149ffff	globl
scall	__pthread_chdir		0xffffffff215cffff	globl
scall	__pthread_canceled	0xffffffff214dffff	globl
scall	__disable_threadsignal	0xffffffff214bffff	globl
scall	abort_with_payload	0xffffffff2209ffff	globl
scall	accept_nocancel		0xffffffff2194ffff	globl
scall	access_extended		0xffffffff211cffff	globl
scall	audit_session_join	0xffffffff21adffff	globl
scall	audit_session_port	0xffffffff21b0ffff	globl
scall	audit_session_self	0xffffffff21acffff	globl
scall	bsdthread_create	0xffffffff2168ffff	globl
scall	bsdthread_ctl		0xffffffff21deffff	globl
scall	bsdthread_register	0xffffffff216effff	globl
scall	bsdthread_terminate	0xffffffff2169ffff	globl
scall	change_fdguard_np	0xffffffff21bcffff	globl
scall	chmod_extended		0xffffffff211affff	globl
scall	clonefileat		0xffffffff21ceffff	globl
scall	close_nocancel		0xffffffff218fffff	globl
scall	coalition		0xffffffff21caffff	globl
scall	coalition_info		0xffffffff21cbffff	globl
scall	connect_nocancel	0xffffffff2199ffff	globl
scall	connectx		0xffffffff21bfffff	globl
scall	copyfile		0xffffffff20e3ffff	globl
scall	csops			0xffffffff20a9ffff	globl
scall	csops_audittoken	0xffffffff20aaffff	globl
scall	csrctl			0xffffffff21e3ffff	globl
scall	delete			0xffffffff20e2ffff	globl
scall	disconnectx		0xffffffff21c0ffff	globl
scall	exchangedata		0xffffffff20dfffff	globl
scall	fchmod_extended		0xffffffff211bffff	globl
scall	fclonefileat		0xffffffff2205ffff	globl
scall	fcntl_nocancel		0xffffffff2196ffff	globl
scall	ffsctl			0xffffffff20f5ffff	globl
scall	fgetattrlist		0xffffffff20e4ffff	globl
scall	fileport_makefd		0xffffffff21afffff	globl
scall	fileport_makeport	0xffffffff21aeffff	globl
scall	fmount			0xffffffff220effff	globl
scall	fs_snapshot		0xffffffff2206ffff	globl
scall	fsctl			0xffffffff20f2ffff	globl
scall	fsetattrlist		0xffffffff20e5ffff	globl
scall	fstat_extended		0xffffffff2119ffff	globl
scall	fsync_nocancel		0xffffffff2198ffff	globl
scall	getattrlist		0xffffffff20dcffff	globl
scall	getattrlistat		0xffffffff21dcffff	globl
scall	getattrlistbulk		0xffffffff21cdffff	globl
scall	getdirentriesattr	0xffffffff20deffff	globl
scall	gethostuuid		0xffffffff208effff	globl
scall	getsgroups		0xffffffff2120ffff	globl
scall	getwgroups		0xffffffff2122ffff	globl
scall	grab_pgo_data		0xffffffff21edffff	globl
scall	guarded_close_np	0xffffffff21baffff	globl
scall	guarded_kqueue_np	0xffffffff21bbffff	globl
scall	guarded_open_np		0xffffffff21b9ffff	globl
scall	guarded_pwrite_np	0xffffffff21e6ffff	globl
scall	guarded_write_np	0xffffffff21e5ffff	globl
scall	guarded_writev_np	0xffffffff21e7ffff	globl
scall	identitysvc		0xffffffff2125ffff	globl
scall	initgroups		0xffffffff20f3ffff	globl
scall	iopolicysys		0xffffffff2142ffff	globl
scall	kas_info		0xffffffff21b7ffff	globl
scall	kdebug_trace		0xffffffff20b3ffff	globl
scall	kdebug_trace_string	0xffffffff20b2ffff	globl
scall	kdebug_typefilter	0xffffffff20b1ffff	globl
scall	kevent_id		0xffffffff2177ffff	globl
scall	kevent_qos		0xffffffff2176ffff	globl
scall	ledger			0xffffffff2175ffff	globl
scall	lstat_extended		0xffffffff2156ffff	globl
scall	memorystatus_control	0xffffffff21b8ffff	globl
scall	memorystatus_get_level	0xffffffff21c5ffff	globl
scall	microstackshot		0xffffffff21ecffff	globl
scall	mkdir_extended		0xffffffff2124ffff	globl
scall	mkfifo_extended		0xffffffff2123ffff	globl
scall	modwatch		0xffffffff20e9ffff	globl
scall	mremap_encrypted	0xffffffff21e9ffff	globl
scall	msgrcv_nocancel		0xffffffff21a3ffff	globl
scall	msgsnd_nocancel		0xffffffff21a2ffff	globl
scall	msync_nocancel		0xffffffff2195ffff	globl
scall	necp_client_action	0xffffffff21f6ffff	globl
scall	necp_match_policy	0xffffffff21ccffff	globl
scall	necp_open		0xffffffff21f5ffff	globl
scall	necp_session_action	0xffffffff220bffff	globl
scall	necp_session_open	0xffffffff220affff	globl
scall	net_qos_guideline	0xffffffff220dffff	globl
scall	netagent_trigger	0xffffffff21eaffff	globl
scall	nfsclnt			0xffffffff20f7ffff	globl
scall	open_dprotected_np	0xffffffff20d8ffff	globl
scall	open_extended		0xffffffff2115ffff	globl
scall	open_nocancel		0xffffffff218effff	globl
scall	openat_nocancel		0xffffffff21d0ffff	globl
scall	openbyid_np		0xffffffff21dfffff	globl
scall	os_fault_with_payload	0xffffffff2211ffff	globl
scall	peeloff			0xffffffff21c1ffff	globl
scall	persona			0xffffffff21eeffff	globl
scall	pid_hibernate		0xffffffff21b3ffff	globl
scall	pid_resume		0xffffffff21b2ffff	globl
scall	pid_shutdown_sockets	0xffffffff21b4ffff	globl
scall	pid_suspend		0xffffffff21b1ffff	globl
scall	poll_nocancel		0xffffffff21a1ffff	globl
scall	pread_nocancel		0xffffffff219effff	globl
scall	proc_info		0xffffffff2150ffff	globl
scall	proc_rlimit_control	0xffffffff21beffff	globl
scall	proc_trace_log		0xffffffff21ddffff	globl
scall	proc_uuid_policy	0xffffffff21c4ffff	globl
scall	process_policy		0xffffffff2143ffff	globl
scall	pselect_nocancel	0xffffffff218bffff	globl
scall	psynch_cvbroad		0xffffffff212fffff	globl
scall	psynch_cvclrprepost	0xffffffff2138ffff	globl
scall	psynch_cvsignal		0xffffffff2130ffff	globl
scall	psynch_mutexdrop	0xffffffff212effff	globl
scall	psynch_mutexwait	0xffffffff212dffff	globl
scall	psynch_rw_downgrade	0xffffffff212bffff	globl
scall	psynch_rw_longrdlock	0xffffffff2129ffff	globl
scall	psynch_rw_rdlock	0xffffffff2132ffff	globl
scall	psynch_rw_unlock	0xffffffff2134ffff	globl
scall	psynch_rw_unlock2	0xffffffff2135ffff	globl
scall	psynch_rw_upgrade	0xffffffff212cffff	globl
scall	psynch_rw_wrlock	0xffffffff2133ffff	globl
scall	psynch_rw_yieldwrlock	0xffffffff212affff	globl
scall	pwrite_nocancel		0xffffffff219fffff	globl
scall	read_nocancel		0xffffffff218cffff	globl
scall	readv_nocancel		0xffffffff219bffff	globl
scall	recvfrom_nocancel	0xffffffff2193ffff	globl
scall	recvmsg_nocancel	0xffffffff2191ffff	globl
scall	recvmsg_x		0xffffffff21e0ffff	globl
scall	renameatx_np		0xffffffff21e8ffff	globl
scall	searchfs		0xffffffff20e1ffff	globl
scall	select_nocancel		0xffffffff2197ffff	globl
scall	sem_close		0xffffffff210dffff	globl
scall	sem_open		0xffffffff210cffff	globl
scall	sem_post		0xffffffff2111ffff	globl
scall	sem_trywait		0xffffffff2110ffff	globl
scall	sem_unlink		0xffffffff210effff	globl
scall	sem_wait		0xffffffff210fffff	globl
scall	sem_wait_nocancel	0xffffffff21a4ffff	globl
scall	sendmsg_nocancel	0xffffffff2192ffff	globl
scall	sendmsg_x		0xffffffff21e1ffff	globl
scall	sendto_nocancel		0xffffffff219dffff	globl
scall	setattrlist		0xffffffff20ddffff	globl
scall	setattrlistat		0xffffffff220cffff	globl
scall	setprivexec		0xffffffff2098ffff	globl
scall	setsgroups		0xffffffff211fffff	globl
scall	settid			0xffffffff211dffff	globl
scall	settid_with_pid		0xffffffff2137ffff	globl
scall	setwgroups		0xffffffff2121ffff	globl
scall	sfi_ctl			0xffffffff21c8ffff	globl
scall	sfi_pidctl		0xffffffff21c9ffff	globl
scall	shared_region_check_np	0xffffffff2126ffff	globl
scall	sigsuspend_nocancel	0xffffffff219affff	globl
scall	socket_delegate		0xffffffff21c2ffff	globl
scall	stat_extended		0xffffffff2155ffff	globl
scall	sysctlbyname		0xffffffff2112ffff	globl
scall	system_override		0xffffffff21c6ffff	globl
scall	telemetry		0xffffffff21c3ffff	globl
scall	terminate_with_payload	0xffffffff2208ffff	globl
scall	thread_selfcounts	0xffffffff20baffff	globl
scall	thread_selfid		0xffffffff2174ffff	globl
scall	thread_selfusage	0xffffffff21e2ffff	globl
scall	ulock_wait		0xffffffff2203ffff	globl
scall	ulock_wake		0xffffffff2204ffff	globl
scall	umask_extended		0xffffffff2116ffff	globl
scall	usrctl			0xffffffff21bdffff	globl
scall	vfs_purge		0xffffffff21c7ffff	globl
scall	vm_pressure_monitor	0xffffffff2128ffff	globl
scall	wait4_nocancel		0xffffffff2190ffff	globl
scall	waitevent		0xffffffff20e8ffff	globl
scall	waitid_nocancel		0xffffffff21a0ffff	globl
scall	watchevent		0xffffffff20e7ffff	globl
scall	work_interval_ctl	0xffffffff21f3ffff	globl
scall	workq_kernreturn	0xffffffff2170ffff	globl
scall	workq_open		0xffffffff216fffff	globl
scall	write_nocancel		0xffffffff218dffff	globl
scall	writev_nocancel		0xffffffff219cffff	globl
#──────────────────────────FREEBSD───────────────────────────
scall	abort2			0xffff01cfffffffff	globl
scall	afs3_syscall		0xffff0179ffffffff	globl
scall	bindat			0xffff021affffffff	globl
scall	break			0xffff0011ffffffff	globl
scall	cap_enter		0xffff0204ffffffff	globl
scall	cap_fcntls_get		0xffff0219ffffffff	globl
scall	cap_fcntls_limit	0xffff0218ffffffff	globl
scall	cap_getmode		0xffff0205ffffffff	globl
scall	cap_ioctls_get		0xffff0217ffffffff	globl
scall	cap_ioctls_limit	0xffff0216ffffffff	globl
scall	cap_rights_limit	0xffff0215ffffffff	globl
scall	clock_getcpuclockid2	0xffff00f7ffffffff	globl
scall	connectat		0xffff021bffffffff	globl
scall	cpuset			0xffff01e4ffffffff	globl
scall	cpuset_getdomain	0xffff0231ffffffff	globl
scall	cpuset_getid		0xffff01e6ffffffff	globl
scall	cpuset_setdomain	0xffff0232ffffffff	globl
scall	cpuset_setid		0xffff01e5ffffffff	globl
scall	eaccess			0xffff0178ffffffff	globl
scall	extattr_delete_fd	0xffff0175ffffffff	globl
scall	extattr_delete_file	0xffff0166ffffffff	globl
scall	extattr_delete_link	0xffff019effffffff	globl
scall	extattr_get_fd		0xffff0174ffffffff	globl
scall	extattr_get_file	0xffff0165ffffffff	globl
scall	extattr_get_link	0xffff019dffffffff	globl
scall	extattr_list_fd		0xffff01b5ffffffff	globl
scall	extattr_list_file	0xffff01b6ffffffff	globl
scall	extattr_list_link	0xffff01b7ffffffff	globl
scall	extattr_set_fd		0xffff0173ffffffff	globl
scall	extattr_set_file	0xffff0164ffffffff	globl
scall	extattr_set_link	0xffff019cffffffff	globl
scall	extattrctl		0xffff0163ffffffff	globl
scall	fexecve			0xffff01ecffffffff	globl
scall	ffclock_getcounter	0xffff00f1ffffffff	globl
scall	ffclock_getestimate	0xffff00f3ffffffff	globl
scall	ffclock_setestimate	0xffff00f2ffffffff	globl
scall	fhlink			0xffff0235ffffffff	globl
scall	fhlinkat		0xffff0236ffffffff	globl
scall	fhreadlink		0xffff0237ffffffff	globl
scall	getaudit		0xffff01c1ffffffff	globl
scall	getcontext		0xffff01a5ffffffff	globl
#scall	getdomainname		0xffff00a2ffffffff	globl
scall	getfhat			0xffff0234ffffffff	globl
scall	gethostid		0xffff008effffffff	globl
#scall	gethostname		0xffff0057ffffffff	globl
scall	getkerninfo		0xffff003fffffffff	globl
scall	getloginclass		0xffff020bffffffff	globl
scall	'getpagesize$freebsd'	0xffff0040ffffffff	globl hidden
scall	gssd_syscall		0xffff01f9ffffffff	globl
scall	jail			0xffff0152ffffffff	globl
scall	jail_attach		0xffff01b4ffffffff	globl
scall	jail_get		0xffff01faffffffff	globl
scall	jail_remove		0xffff01fcffffffff	globl
scall	jail_set		0xffff01fbffffffff	globl
scall	kenv			0xffff0186ffffffff	globl
scall	kldfind			0xffff0132ffffffff	globl
scall	kldfirstmod		0xffff0135ffffffff	globl
scall	kldload			0xffff0130ffffffff	globl
scall	kldnext			0xffff0133ffffffff	globl
scall	kldstat			0xffff0134ffffffff	globl
scall	kldsym			0xffff0151ffffffff	globl
scall	kldunload		0xffff0131ffffffff	globl
scall	kldunloadf		0xffff01bcffffffff	globl
scall	kmq_notify		0xffff01cdffffffff	globl
scall	kmq_setattr		0xffff01caffffffff	globl
scall	kmq_timedreceive	0xffff01cbffffffff	globl
scall	kmq_timedsend		0xffff01ccffffffff	globl
scall	kmq_unlink		0xffff01ceffffffff	globl
scall	ksem_close		0xffff0190ffffffff	globl
scall	ksem_destroy		0xffff0198ffffffff	globl
scall	ksem_getvalue		0xffff0197ffffffff	globl
scall	ksem_init		0xffff0194ffffffff	globl
scall	ksem_open		0xffff0195ffffffff	globl
scall	ksem_post		0xffff0191ffffffff	globl
scall	ksem_timedwait		0xffff01b9ffffffff	globl
scall	ksem_trywait		0xffff0193ffffffff	globl
scall	ksem_unlink		0xffff0196ffffffff	globl
scall	ksem_wait		0xffff0192ffffffff	globl
scall	ktimer_create		0xffff00ebffffffff	globl
scall	ktimer_delete		0xffff00ecffffffff	globl
scall	ktimer_getoverrun	0xffff00efffffffff	globl
scall	ktimer_gettime		0xffff00eeffffffff	globl
scall	ktimer_settime		0xffff00edffffffff	globl
scall	lchflags		0xffff0187ffffffff	globl
scall	lchmod			0xffff0112ffffffff	globl
scall	lgetfh			0xffff00a0ffffffff	globl
scall	lpathconf		0xffff0201ffffffff	globl
scall	lutimes			0xffff0114ffffffff	globl
scall	mac_syscall		0xffff018affffffff	globl
scall	modfind			0xffff012fffffffff	globl
scall	modfnext		0xffff012effffffff	globl
scall	modnext			0xffff012cffffffff	globl
scall	modstat			0xffff012dffffffff	globl
scall	nfstat			0xffff0117ffffffff	globl
scall	nlm_syscall		0xffff009affffffff	globl
scall	nlstat			0xffff0118ffffffff	globl
scall	nmount			0xffff017affffffff	globl
scall	nnpfs_syscall		0xffff0153ffffffff	globl
scall	nstat			0xffff0116ffffffff	globl
scall	pdfork			0xffff0206ffffffff	globl
scall	pdgetpid		0xffff0208ffffffff	globl
scall	pdkill			0xffff0207ffffffff	globl
scall	'posix_openpt$sysv'	0xffff01f8ffffffff	globl hidden
scall	procctl			0xffff0220ffffffff	globl
scall	psynch_cvwait		0xffffffff2131ffff	globl
scall	quota			0xffff0095ffffffff	globl
scall	rctl_add_rule		0xffff0210ffffffff	globl
scall	rctl_get_limits		0xffff020fffffffff	globl
scall	rctl_get_racct		0xffff020dffffffff	globl
scall	rctl_get_rules		0xffff020effffffff	globl
scall	rctl_remove_rule	0xffff0211ffffffff	globl
scall	recv			0xffff0066ffffffff	globl
scall	rfork			0xffff00fbffffffff	globl
scall	rtprio			0xffff00a6ffffffff	globl
scall	rtprio_thread		0xffff01d2ffffffff	globl
scall	send			0xffff0065ffffffff	globl
scall	setaudit		0xffff01c2ffffffff	globl
scall	setcontext		0xffff01a6ffffffff	globl
scall	setfib			0xffff00afffffffff	globl
scall	sethostid		0xffff008fffffffff	globl
scall	setloginclass		0xffff020cffffffff	globl
scall	sigblock		0xffff006dffffffff	globl
scall	sigqueue		0xffff01c8ffffffff	globl
scall	sigsetmask		0xffff006effffffff	globl
scall	sigstack		0xffff0070ffffffff	globl
scall	sigvec			0xffff006cffffffff	globl
scall	sigwaitinfo		0xffff015affffffff	globl
scall	sstk			0xffff0046ffffffff	globl
scall	swapcontext		0xffff01a7ffffffff	globl
scall	thr_create		0xffff01aeffffffff	globl
scall	thr_exit		0xffff01afffffffff	globl
scall	thr_kill		0xffff01b1ffffffff	globl
scall	thr_kill2		0xffff01e1ffffffff	globl
scall	thr_new			0xffff01c7ffffffff	globl
scall	thr_self		0xffff01b0ffffffff	globl
scall	thr_set_name		0xffff01d0ffffffff	globl
scall	thr_suspend		0xffff01baffffffff	globl
scall	thr_wake		0xffff01bbffffffff	globl
scall	uuidgen			0xffff0188ffffffff	globl
scall	vadvise			0xffff0048ffffffff	globl
scall	wait			0xffff0054ffffffff	globl
scall	wait6			0xffff0214ffffffff	globl
scall	yield			0xffff0141ffffffff	globl
#──────────────────────────OPENBSD───────────────────────────
scall	__tfork			0x0008ffffffffffff	globl
scall	__thrsleep		0x005effffffffffff	globl
scall	__thrwakeup		0x012dffffffffffff	globl
scall	__threxit		0x012effffffffffff	globl
scall	__thrsigdivert		0x012fffffffffffff	globl
scall	__set_tcb		0x0149ffffffffffff	globl
scall	__get_tcb		0x014affffffffffff	globl
scall	adjfreq			0x0131ffffffffffff	globl
scall	getdtablecount		0x0012ffffffffffff	globl
scall	getlogin_r		0x008dffffffffffff	globl
scall	getrtable		0x0137ffffffffffff	globl
scall	getthrid		0x012bffffffffffff	globl
scall	kbind			0x0056ffffffffffff	globl
scall	mquery			0x011effffffffffff	globl # openbsd:pad
scall	obreak			0x0011ffffffffffff	globl
scall	sendsyslog		0x0070ffffffffffff	globl
scall	setrtable		0x0136ffffffffffff	globl
scall	swapctl			0x00c1ffffffffffff	globl
scall	thrkill			0x0077ffffffffffff	globl
scall	unveil			0x0072ffffffffffff	globl

#	The Fifth Bell System Interface, Community Edition
#	» beyond the pale
#	                             GNU/Systemd┐
#	                            Mac OS X┐   │
#	                         FreeBSD┐   │   │
#	                     OpenBSD┐   │ ┌─│───│── XnuClass{1:Mach,2:Unix}
#	                          ┌─┴┐┌─┴┐│┌┴┐┌─┴┐
scall	__mac_get_link		0xffff019a2180ffff	globl
scall	__mac_set_link		0xffff019b2181ffff	globl
scall	__mac_get_fd		0xffff01822184ffff	globl
scall	__mac_get_file		0xffff0183217effff	globl
scall	__mac_get_proc		0xffff01802182ffff	globl
scall	__mac_set_fd		0xffff01842185ffff	globl
scall	__mac_get_pid		0xffff01992186ffff	globl
scall	__mac_set_proc		0xffff01812183ffff	globl
scall	__mac_set_file		0xffff0185217fffff	globl
scall	__mac_execve		0xffff019f217cffff	globl
scall	__acl_get_link		0xffff01a9ffffffff	globl
scall	__sigwait_nocancel	0xffffffff21a6ffff	globl
scall	__cap_rights_get	0xffff0203ffffffff	globl
scall	__semwait_signal	0xffffffff214effff	globl
scall	__acl_set_link		0xffff01aaffffffff	globl
scall	__acl_set_fd		0xffff015effffffff	globl
scall	__old_semwait_signal	0xffffffff2172ffff	globl
scall	__setugid		0xffff0176ffffffff	globl
scall	__acl_aclcheck_fd	0xffff0162ffffffff	globl
scall	__acl_get_fd		0xffff015dffffffff	globl
scall	__sysctl		0xffff00caffffffff	globl
scall	__mac_getfsstat		0xffffffff21aaffff	globl
scall	__mac_get_mount		0xffffffff21a9ffff	globl
scall	__acl_delete_link	0xffff01abffffffff	globl
scall	__mac_mount		0xffffffff21a8ffff	globl
scall	__acl_get_file		0xffff015bffffffff	globl
scall	__acl_aclcheck_file	0xffff0161ffffffff	globl
scall	__acl_delete_fd		0xffff0160ffffffff	globl
scall	__acl_aclcheck_link	0xffff01acffffffff	globl
scall	__mac_syscall		0xffffffff217dffff	globl
scall	__acl_set_file		0xffff015cffffffff	globl
scall	__acl_delete_file	0xffff015fffffffff	globl
scall	__syscall		0x00c6ffffffffffff	globl
scall	_umtx_op		0xffff01c6ffffffff	globl
scall	__semwait_signal_nocancel	0xffffffff21a7ffff	globl
scall	__old_semwait_signal_nocancel	0xffffffff2173ffff	globl
scall	sctp_peeloff			0xffff01d7ffffffff	globl
scall	sctp_generic_recvmsg		0xffff01daffffffff	globl
scall	sctp_generic_sendmsg		0xffff01d8ffffffff	globl
scall	sctp_generic_sendmsg_iov	0xffff01d9ffffffff	globl
scall	shared_region_map_and_slide_np	0xffffffff21b6ffff	globl
scall	guarded_open_dprotected_np	0xffffffff21e4ffff	globl
scall	stack_snapshot_with_config	0xffffffff21ebffff	globl
