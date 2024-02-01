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
dir=libc/sysv/consts
. libc/sysv/gen.sh

#	The Fifth Bell System, Community Edition
#	» catalogue of carnage
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	errno	ENOSYS					38			38			78			78			78			78			78			1			# system call unavailable; bsd consensus; kNtErrorInvalidFunction
syscon	errno	EPERM					1			1			1			1			1			1			1			12			# operation not permitted; unix consensus; kNtErrorInvalidAccess (should be kNtErrorNotOwner but is that mutex only??); raised by accept(2), acct(2), add_key(2), adjtimex(2), arch_prctl(2), bdflush(2), bpf(2), capget(2), chmod(2), chown(2), chroot(2), clock_getres(2), clone(2), copy_file_range(2), create_module(2), delete_module(2), epoll_ctl(2), execve(2), fallocate(2), fanotify_init(2), fcntl(2), futex(2), get_robust_list(2), getdomainname(2), getgroups(2), gethostname(2), getpriority(2), getrlimit(2), getsid(2), gettimeofday(2), idle(2), init_module(2), io_submit(2), ioctl_console(2), ioctl_ficlonerange(2), ioctl_fideduperange(2), ioctl_ns(2), ioctl_tty(2), ioperm(2), iopl(2), ioprio_set(2), kcmp(2), kexec_load(2), keyctl(2), kill(2), link(2), lookup_dcookie(2), madvise(2), mbind(2), membarrier(2), migrate_pages(2), mkdir(2), mknod(2), mlock(2), mmap(2), mount(2), move_pages(2), msgctl(2), nice(2), open(2), open_by_handle_at(2), pciconfig_read(2), perf_event_open(2), pidfd_getfd(2), pidfd_send_signal(2), pivot_root(2), prctl(2), process_vm_readv(2), ptrace(2), quotactl(2), reboot(2), rename(2), request_key(2), rmdir(2), rt_sigqueueinfo(2), sched_setaffinity(2), sched_setattr(2), sched_setparam(2), sched_setscheduler(2), semctl(2), seteuid(2), setfsgid(2), setfsuid(2), setgid(2), setns(2), setpgid(2), setresuid(2), setreuid(2), setsid(2), setuid(2), setup(2), setxattr(2), shmctl(2), shmget(2), sigaltstack(2), spu_create(2), stime(2), swapon(2), symlink(2), syslog(2), timer_create(2), timerfd_create(2), tkill(2), truncate(2), umount(2), unlink(2), unshare(2), utime(2), utimensat(2), vhangup(2), vm86(2), write(2), unix(7), ip(7)
syscon	errno	ENOENT					2			2			2			2			2			2			2			2			# no such file or directory; unix consensus; kNtErrorFileNotFound; raised by access(2), acct(2), alloc_hugepages(2), bind(2), bpf(2), chdir(2), chmod(2), chown(2), chroot(2), clock_getres(2), delete_module(2), epoll_ctl(2), execve(2), execveat(2), fanotify_mark(2), getdents(2), inotify_add_watch(2), ioctl_fat(2), kcmp(2), keyctl(2), link(2), mkdir(2), mknod(2), mount(2), msgget(2), open(2), open_by_handle_at(2), perf_event_open(2), query_module(2), quotactl(2), readdir(2), readlink(2), rename(2), rmdir(2), semget(2), shmget(2), spu_create(2), stat(2), statfs(2), statx(2), swapon(2), symlink(2), truncate(2), umount(2), unlink(2), utime(2), utimensat(2), unix(7), ip(7)
syscon	errno	ESRCH					3			3			3			3			3			3			3			566			# no such process; kNtErrorThreadNotInProcess (cf. kNtErrorInvalidHandle); raised by capget(2), get_robust_list(2), getpriority(2), getrlimit(2), getsid(2), ioprio_set(2), kcmp(2), kill(2), migrate_pages(2), move_pages(2), perf_event_open(2), pidfd_getfd(2), pidfd_open(2), pidfd_send_signal(2), process_vm_readv(2), ptrace(2), quotactl(2), rt_sigqueueinfo(2), sched_rr_get_interval(2), sched_setaffinity(2), sched_setattr(2), sched_setparam(2), sched_setscheduler(2), set_thread_area(2), setpgid(2), tkill(2), utimensat(2), unix(7)
syscon	errno	EINTR					4			4			4			4			4			4			4			10004			# the greatest of all errnos; crucial for building real time reliable software; unix consensus; WSAEINTR; raised by accept(2), clock_nanosleep(2), close(2), connect(2), dup(2), epoll_wait(2), fallocate(2), fcntl(2), flock(2), futex(2), getrandom(2), io_getevents(2), msgop(2), nanosleep(2), open(2), pause(2), perf_event_open(2), poll(2), ptrace(2), read(2), recv(2), request_key(2), select(2), semop(2), send(2), sigsuspend(2), sigwaitinfo(2), spu_run(2), statfs(2), truncate(2), wait(2), write(2)
syscon	errno	EIO					5			5			5			5			5			5			5			1117			# unix consensus; kNtErrorIoDevice; raised by access(2) acct(2) chdir(2) chmod(2) chown(2) chroot(2) close(2) copy_file_range(2) execve(2) fallocate(2) fsync(2) ioperm(2) link(2) madvise(2) mbind(2) pciconfig_read(2) ptrace(2) read(2) readlink(2) sendfile(2) statfs(2) symlink(2) sync_file_range(2) truncate(2) unlink(2) write(2)
syscon	errno	ENXIO					6			6			6			6			6			6			6			1112			# no such device or address; unix consensus; kNtErrorNoMediaInDrive; raised by lseek(2), mount(2), open(2), prctl(2)
syscon	errno	E2BIG					7			7			7			7			7			7			7			1639			# argument list too long; unix consensus; kNtErrorInvalidCommandLine; raised by bpf(2), execve(2), getxattr(2), listxattr(2), move_pages(2), msgop(2), openat2(2), perf_event_open(2), sched_setattr(2), semop(2)
syscon	errno	ENOEXEC					8			8			8			8			8			8			8			193			# exec format error; unix consensus; kNtErrorBadExeFormat; raised by execve(2), init_module(2), kexec_load(2), uselib(2)
syscon	errno	EBADF					9			9			9			9			9			9			9			6			# bad file descriptor; cf. EBADFD; unix consensus; kNtErrorInvalidHandle; raised by accept(2), access(2), bind(2), bpf(2), chdir(2), chmod(2), chown(2), close(2), connect(2), copy_file_range(2), dup(2), epoll_ctl(2), epoll_wait(2), execveat(2), fallocate(2), fanotify_mark(2), fcntl(2), flock(2), fsync(2), futimesat(2), getdents(2), getpeername(2), getsockname(2), getsockopt(2), init_module(2), inotify_add_watch(2), inotify_rm_watch(2), io_submit(2), ioctl(2), ioctl_console(2), ioctl_ficlonerange(2), ioctl_fideduperange(2), ioctl_getfsmap(2), kcmp(2), kexec_load(2), link(2), listen(2), llseek(2), lseek(2), madvise(2), mkdir(2), mknod(2), mmap(2), open(2), open_by_handle_at(2), perf_event_open(2), pidfd_getfd(2), pidfd_send_signal(2), posix_fadvise(2), prctl(2), read(2), readahead(2), readdir(2), readlink(2), recv(2), rename(2), select(2), send(2), sendfile(2), setns(2), shutdown(2), signalfd(2), splice(2), spu_run(2), stat(2), statfs(2), statx(2), symlink(2), sync(2), sync_file_range(2), timerfd_create(2), truncate(2), unlink(2), utimensat(2), vmsplice(2), write(2), unix(7)
syscon	errno	ECHILD					10			10			10			10			10			10			10			128			# no child process; unix consensus; kNtErrorWaitNoChildren; raised by wait(2), waitpid(2), waitid(2), wait3(2), wait4(2)
syscon	errno	EAGAIN					11			11			35			35			35			35			35			10035			# resource temporarily unavailable (e.g. SO_RCVTIMEO expired, too many processes, too much memory locked, read or write with O_NONBLOCK needs polling, etc.); bsd consensus; WSAEWOULDBLOCK; raised by accept(2), clone(2), connect(2), eventfd(2), fcntl(2), fork(2), futex(2), getrandom(2), io_cancel(2), io_setup(2), io_submit(2), ioctl_userfaultfd(2), keyctl(2), madvise(2), mincore(2), mlock(2), mmap(2), mremap(2), msgop(2), openat2(2), poll(2), read(2), rt_sigqueueinfo(2), select(2), semop(2), send(2), sendfile(2), setresuid(2), setreuid(2), setuid(2), signalfd(2), sigwaitinfo(2), splice(2), tee(2), timer_create(2), timerfd_create(2), tkill(2), umount(2), vmsplice(2), write(2), ip(7)
syscon	errno	ENOMEM					12			12			12			12			12			12			12			14			# we require more vespene gas; unix consensus; kNtErrorOutofmemory; raised by access(2), acct(2), add_key(2), bind(2), bpf(2), chdir(2), chmod(2), chown(2), chroot(2), clone(2), copy_file_range(2), create_module(2), epoll_create(2), epoll_ctl(2), eventfd(2), execve(2), fanotify_init(2), fanotify_mark(2), fork(2), getgroups(2), getrlimit(2), init_module(2), inotify_add_watch(2), inotify_init(2), io_setup(2), ioctl_fideduperange(2), ioctl_getfsmap(2), ioperm(2), kexec_load(2), keyctl(2), link(2), lookup_dcookie(2), madvise(2), mbind(2), memfd_create(2), mincore(2), mkdir(2), mknod(2), mlock(2), mmap(2), mount(2), mprotect(2), mremap(2), msgget(2), msgop(2), msync(2), open(2), pidfd_open(2), poll(2), process_vm_readv(2), readlink(2), recv(2), rename(2), request_key(2), rmdir(2), s390_guarded_storage(2), s390_pci_mmio_write(2), s390_runtime_instr(2), s390_sthyi(2), select(2), semget(2), semop(2), send(2), sendfile(2), set_mempolicy(2), setns(2), shmctl(2), shmget(2), shmop(2), sigaltstack(2), signalfd(2), splice(2), spu_create(2), spu_run(2), stat(2), statfs(2), statx(2), subpage_prot(2), swapon(2), symlink(2), sync_file_range(2), tee(2), timer_create(2), timerfd_create(2), umount(2), unlink(2), unshare(2), userfaultfd(2), vmsplice(2), unix(7), ip(7)
syscon	errno	EACCES					13			13			13			13			13			13			13			5			# permission denied; unix consensus; kNtErrorAccessDenied; raised by access(2), acct(2), add_key(2), bind(2), bpf(2), chdir(2), chmod(2), chown(2), chroot(2), clock_getres(2), connect(2), execve(2), fcntl(2), futex(2), getpriority(2), inotify_add_watch(2), keyctl(2), link(2), madvise(2), mkdir(2), mknod(2), mmap(2), mount(2), move_pages(2), mprotect(2), msgctl(2), msgget(2), msgop(2), open(2), perf_event_open(2), prctl(2), ptrace(2), quotactl(2), readlink(2), rename(2), request_key(2), rmdir(2), semctl(2), semget(2), semop(2), send(2), setpgid(2), shmctl(2), shmget(2), shmop(2), socket(2), spu_create(2), stat(2), statfs(2), statx(2), symlink(2), truncate(2), unlink(2), uselib(2), utime(2), utimensat(2), ip(7)
syscon	errno	EFAULT					14			14			14			14			14			14			14			487			# pointer passed to system call that would otherwise segfault; unix consensus; kNtErrorInvalidAddress; raised by accept(2), access(2), acct(2), add_key(2), adjtimex(2), arch_prctl(2), bdflush(2), bind(2), bpf(2), cacheflush(2), capget(2), chdir(2), chmod(2), chown(2), chroot(2), clock_getres(2), clock_nanosleep(2), connect(2), create_module(2), delete_module(2), epoll_wait(2), execve(2), fcntl(2), futex(2), get_mempolicy(2), get_robust_list(2), getcpu(2), getdents(2), getdomainname(2), getgroups(2), gethostname(2), getitimer(2), getpeername(2), getrandom(2), getresuid(2), getrlimit(2), getrusage(2), getsockname(2), getsockopt(2), gettimeofday(2), getunwind(2), init_module(2), inotify_add_watch(2), io_cancel(2), io_destroy(2), io_getevents(2), io_setup(2), io_submit(2), ioctl(2), ioctl_getfsmap(2), ioctl_userfaultfd(2), kcmp(2), keyctl(2), link(2), llseek(2), lookup_dcookie(2), mbind(2), memfd_create(2), migrate_pages(2), mincore(2), mkdir(2), mknod(2), mmap2(2), modify_ldt(2), mount(2), move_pages(2), mremap(2), msgctl(2), msgop(2), msync(2), nanosleep(2), open(2), open_by_handle_at(2), perf_event_open(2), pipe(2), poll(2), prctl(2), process_vm_readv(2), ptrace(2), query_module(2), quotactl(2), read(2), readdir(2), readlink(2), reboot(2), recv(2), rename(2), request_key(2), rmdir(2), s390_guarded_storage(2), s390_pci_mmio_write(2), s390_sthyi(2), sched_rr_get_interval(2), sched_setaffinity(2), semctl(2), semop(2), send(2), sendfile(2), set_mempolicy(2), set_thread_area(2), shmctl(2), sigaction(2), sigaltstack(2), sigpending(2), sigprocmask(2), sigsuspend(2), socketpair(2), spu_create(2), spu_run(2), stat(2), statfs(2), statx(2), stime(2), subpage_prot(2), symlink(2), sysctl(2), sysfs(2), sysinfo(2), time(2), timer_settime(2), timerfd_create(2), times(2), truncate(2), umount(2), uname(2), unlink(2), ustat(2), utimensat(2), vm86(2), write(2), unix(7)
syscon	errno	ENOTBLK					15			15			15			15			15			15			15			26			# block device required; unix consensus; kNtErrorNotDosDisk; raised by mount(2), quotactl(2), umount(2)
syscon	errno	EBUSY					16			16			16			16			16			16			16			170			# device or resource busy; unix consensus; kNtErrorBusy; raised by bdflush(2), delete_module(2), dup(2), fcntl(2), init_module(2), ioctl_tty(2), ioctl_userfaultfd(2), kexec_load(2), mount(2), msync(2), pivot_root(2), prctl(2), ptrace(2), quotactl(2), rename(2), rmdir(2), sched_setattr(2), swapon(2), umount(2)
syscon	errno	EEXIST					17			17			17			17			17			17			17			183			# file exists; unix consensus; kNtErrorAlreadyExists (should be kNtErrorFileExists too); raised by bpf(2), create_module(2), epoll_ctl(2), init_module(2), inotify_add_watch(2), keyctl(2), link(2), mkdir(2), mknod(2), mmap(2), msgget(2), open(2), rename(2), rmdir(2), semget(2), setxattr(2), shmget(2), spu_create(2), symlink(2)
syscon	errno	EXDEV					18			18			18			18			18			18			18			17			# improper link; unix consensus; kNtErrorNotSameDevice; raised by copy_file_range(2), fanotify_mark(2), ioctl_ficlonerange(2), ioctl_fideduperange(2), link(2), openat2(2), rename(2)
syscon	errno	ENODEV					19			19			19			19			19			19			19			1200			# no such device; unix consensus; kNtErrorBadDevice; raised by arch_prctl(2), eventfd(2), fallocate(2), fanotify_mark(2), mmap(2), mount(2), move_pages(2), open(2), pciconfig_read(2), perf_event_open(2), pidfd_open(2), prctl(2), s390_pci_mmio_write(2), signalfd(2), spu_create(2), timerfd_create(2)
syscon	errno	ENOTDIR					20			20			20			20			20			20			20			3			# not a directory; unix consensus; kNtErrorPathNotFound; raised by access(2), acct(2), bind(2), chdir(2), chmod(2), chown(2), chroot(2), execve(2), execveat(2), fanotify_mark(2), fcntl(2), futimesat(2), getdents(2), inotify_add_watch(2), ioctl_fat(2), keyctl(2), link(2), mkdir(2), mknod(2), mount(2), open(2), open_by_handle_at(2), pivot_root(2), readdir(2), readlink(2), rename(2), rmdir(2), spu_create(2), stat(2), statfs(2), statx(2), symlink(2), sysctl(2), truncate(2), unlink(2), utimensat(2)
syscon	errno	EISDIR					21			21			21			21			21			21			21			267			# is a a directory; unix consensus; kNtErrorDirectory; raised by acct(2), copy_file_range(2), execve(2), ioctl_ficlonerange(2), ioctl_fideduperange(2), open(2), read(2), rename(2), truncate(2), unlink(2)
syscon	errno	EINVAL					22			22			22			22			22			22			22			87			# invalid argument; unix consensus; kNtErrorInvalidParameter; raised by accept(2), access(2), add_key(2), adjtimex(2), arch_prctl(2), bdflush(2), bind(2), bpf(2), cacheflush(2), capget(2), chmod(2), chown(2), clock_getres(2), clock_nanosleep(2), clone(2), copy_file_range(2), create_module(2), dup(2), epoll_create(2), epoll_ctl(2), epoll_wait(2), eventfd(2), execve(2), execveat(2), fallocate(2), fanotify_init(2), fanotify_mark(2), fcntl(2), flock(2), futex(2), get_mempolicy(2), get_robust_list(2), getdents(2), getdomainname(2), getgroups(2), gethostname(2), getitimer(2), getpeername(2), getpriority(2), getrandom(2), getrlimit(2), getrusage(2), getsockname(2), getsockopt(2), gettimeofday(2), init_module(2), inotify_add_watch(2), inotify_init(2), inotify_rm_watch(2), io_cancel(2), io_destroy(2), io_getevents(2), io_setup(2), io_submit(2), ioctl(2), ioctl_console(2), ioctl_ficlonerange(2), ioctl_fideduperange(2), ioctl_getfsmap(2), ioctl_ns(2), ioctl_tty(2), ioctl_userfaultfd(2), ioperm(2), iopl(2), ioprio_set(2), kcmp(2), kexec_load(2), keyctl(2), kill(2), link(2), llseek(2), lookup_dcookie(2), lseek(2), madvise(2), mbind(2), membarrier(2), memfd_create(2), migrate_pages(2), mincore(2), mkdir(2), mknod(2), mlock(2), mmap(2), mmap2(2), modify_ldt(2), mount(2), move_pages(2), mprotect(2), mremap(2), msgctl(2), msgop(2), msync(2), nanosleep(2), open(2), open_by_handle_at(2), openat2(2), pciconfig_read(2), perf_event_open(2), personality(2), pidfd_getfd(2), pidfd_open(2), pidfd_send_signal(2), pipe(2), pivot_root(2), pkey_alloc(2), poll(2), posix_fadvise(2), prctl(2), process_vm_readv(2), ptrace(2), query_module(2), quotactl(2), read(2), readahead(2), readdir(2), readlink(2), readv(2), reboot(2), recv(2), recvmmsg(2), remap_file_pages(2), rename(2), request_key(2), rmdir(2), rt_sigqueueinfo(2), s390_guarded_storage(2), s390_pci_mmio_write(2), s390_runtime_instr(2), s390_sthyi(2), sched_get_priority_max(2), sched_rr_get_interval(2), sched_setaffinity(2), sched_setattr(2), sched_setparam(2), sched_setscheduler(2), seccomp(2), select(2), semctl(2), semget(2), semop(2), send(2), sendfile(2), set_mempolicy(2), set_thread_area(2), seteuid(2), setfsgid(2), setfsuid(2), setgid(2), setns(2), setpgid(2), setresuid(2), setreuid(2), setuid(2), shmctl(2), shmget(2), shmop(2), shutdown(2), sigaction(2), sigaltstack(2), signal(2), signalfd(2), sigprocmask(2), sigsuspend(2), sigwaitinfo(2), socket(2), splice(2), spu_create(2), spu_run(2), stat(2), statx(2), subpage_prot(2), swapon(2), sync_file_range(2), sysfs(2), syslog(2), tee(2), timer_create(2), timer_delete(2), timer_getoverrun(2), timer_settime(2), timerfd_create(2), tkill(2), truncate(2), umount(2), unlink(2), unshare(2), userfaultfd(2), ustat(2), utimensat(2), vmsplice(2), wait(2), write(2), unix(7), ip(7)
syscon	errno	ENFILE					23			23			23			23			23			23			23			331			# too many open files in system; unix consensus; kNtErrorTooManyDescriptors; raised by accept(2), acct(2), epoll_create(2), eventfd(2), execve(2), futex(2), inotify_init(2), memfd_create(2), mmap(2), open(2), pidfd_getfd(2), pidfd_open(2), pipe(2), shmget(2), signalfd(2), socket(2), socketpair(2), spu_create(2), swapon(2), timerfd_create(2), uselib(2), userfaultfd(2)
syscon	errno	EMFILE					24			24			24			24			24			24			24			4			# too many open files; unix consensus; kNtErrorTooManyOpenFiles; raised by accept(2), dup(2), epoll_create(2), eventfd(2), execve(2), fanotify_init(2), fcntl(2), inotify_init(2), memfd_create(2), mount(2), open(2), perf_event_open(2), pidfd_getfd(2), pidfd_open(2), pipe(2), signalfd(2), socket(2), socketpair(2), spu_create(2), timerfd_create(2)
syscon	errno	ENOTTY					25			25			25			25			25			25			25			1118			# inappropriate i/o control operation; unix consensus; kNtErrorSerialNoDevice; raised by ioctl(2), ioctl_console(2), ioctl_fat(2), ioctl_ns(2), ioctl_tty(2)
syscon	errno	ETXTBSY					26			26			26			26			26			26			26			148			# won't open executable that's executing in write mode; try UnlockExecutable(); unix consensus; kNtErrorPathBusy; raised by access(2), copy_file_range(2), execve(2), fallocate(2), ioctl_ficlonerange(2), ioctl_fideduperange(2), mmap(2), open(2), truncate(2)
syscon	errno	EFBIG					27			27			27			27			27			27			27			223			# file too large; unix consensus; kNtErrorFileTooLarge; raised by copy_file_range(2), fallocate(2), init_module(2), open(2), semop(2), truncate(2), write(2)
syscon	errno	ENOSPC					28			28			28			28			28			28			28			39			# no space left on device; unix consensus; kNtErrorDiskFull; raised by copy_file_range(2), epoll_ctl(2), fallocate(2), fanotify_mark(2), fsync(2), inotify_add_watch(2), link(2), mkdir(2), mknod(2), msgget(2), open(2), perf_event_open(2), pkey_alloc(2), query_module(2), rename(2), semget(2), setxattr(2), shmget(2), spu_create(2), symlink(2), sync_file_range(2), write(2)
syscon	errno	EDQUOT					122			122			69			69			69			69			69			1816			# disk quota exceeded; bsd consensus; kNtErrorNotEnoughQuota; raised by add_key(2), keyctl(2), link(2), mkdir(2), mknod(2), open(2), rename(2), request_key(2), setxattr(2), symlink(2), write(2)
syscon	errno	ESPIPE					29			29			29			29			29			29			29			25			# invalid seek; unix consensus; kNtErrorSeek; raised by fallocate(2), lseek(2), posix_fadvise(2), sendfile(2), splice(2), sync_file_range(2)
syscon	errno	EROFS					30			30			30			30			30			30			30			6009			# read-only filesystem; unix consensus; kNtErrorFileReadOnly; raised by access(2), acct(2), bind(2), chmod(2), chown(2), link(2), mkdir(2), mknod(2), mount(2), open(2), rename(2), rmdir(2), symlink(2), truncate(2), unlink(2), utime(2), utimensat(2)
syscon	errno	EMLINK					31			31			31			31			31			31			31			1142			# too many links; unix consensus; kNtErrorTooManyLinks; raised by link(2), mkdir(2), rename(2)
syscon	errno	EPIPE					32			32			32			32			32			32			32			109			# broken pipe; unix consensus; kNtErrorBrokenPipe; raised by send(2), write(2), tcp(7), unix(7), ip(7)
syscon	errno	EDOM					33			33			33			33			33			33			33			20000			# mathematics argument out of domain of function; bsd consensus; made up on NT; returned by cos(3), fmod(3), log1p(3), sin(3), tan(3), tgamma(3)
syscon	errno	ERANGE					34			34			34			34			34			34			34			6604			# result too large; bsd consensus; kNtErrorLogInvalidRange; raised by getxattr(2), listxattr(2), lookup_dcookie(2), prctl(2), quotactl(2), semctl(2), semop(2), setxattr(2)
syscon	errno	EDEADLK					35			35			11			11			11			11			11			1131			# resource deadlock avoided; bsd consensus; kNtErrorPossibleDeadlock; raised by fcntl(2), keyctl(2)
syscon	errno	ENAMETOOLONG				36			36			63			63			63			63			63			10063			# filename too long; bsd consensus; WSAENAMETOOLONG; raised by access(2), acct(2), bind(2), chdir(2), chmod(2), chown(2), chroot(2), execve(2), gethostname(2), inotify_add_watch(2), link(2), lookup_dcookie(2), mkdir(2), mknod(2), mount(2), open(2), readlink(2), rename(2), rmdir(2), spu_create(2), stat(2), statfs(2), statx(2), symlink(2), truncate(2), umount(2), unlink(2), utimensat(2)
syscon	errno	ENOLCK					37			37			77			77			77			77			77			158			# no locks available; kNtErrorNotLocked; bsd consensus; raised by fcntl(2), flock(2)
syscon	errno	ENOTEMPTY				39			39			66			66			66			66			66			145			# directory not empty; bsd consensus; kNtErrorDirNotEmpty (TODO: What is WSAENOTEMPTY? 10066); raised by rmdir(2)
syscon	errno	ELOOP					40			40			62			62			62			62			62			1921			# too many levels of symbolic links; bsd consensus; kNtErrorCantResolveFilename; raised by access(2), acct(2), bind(2), chdir(2), chmod(2), chown(2), chroot(2), epoll_ctl(2), execve(2), execveat(2), keyctl(2), link(2), mkdir(2), mknod(2), mount(2), open(2), open_by_handle_at(2), openat2(2), readlink(2), rename(2), rmdir(2), spu_create(2), stat(2), statfs(2), statx(2), symlink(2), truncate(2), unlink(2), utimensat(2)
syscon	errno	ENOMSG					42			42			91			91			83			90			83			4306			# kNtErrorEmpty; raised by msgop(2)
syscon	errno	EIDRM					43			43			90			90			82			89			82			1287			# identifier removed; kNtErrorUnidentifiedError; raised by msgctl(2), msgget(2), msgop(2), semctl(2), semop(2), shmctl(2), shmget(2), shmop(2)
syscon	errno	EPROTO					71			71			100			100			92			95			96			7065			# Protocol error; kNtErrorRdpProtocolError; raised by accept(2), connect(2), socket(2), socketpair(2)
syscon	errno	EOVERFLOW				75			75			84			84			84			87			84			534			# kNtErrorArithmeticOverflow; raised by aio_read(2), copy_file_range(2), ctime(2), fanotify_init(2), lseek(2), mmap(2), open(2), open_by_handle_at(2), sem_post(2), sendfile(2), shmctl(2), stat(2), statfs(2), statvfs(2), time(2), timegm(2)
syscon	errno	EILSEQ					84			84			92			92			86			84			85			582			# kNtErrorIllegalCharacter; returned by fgetwc(3), fputwc(3), getwchar(3), putwchar(3), scanf(3), ungetwc(3)
syscon	errno	EUSERS					87			87			68			68			68			68			68			10068			# too many users; bsd consensus; WSAEUSERS; raised by acct(2)
syscon	errno	ENOTSOCK				88			88			38			38			38			38			38			10038			# not a socket; bsd consensus; WSAENOTSOCK; raised by accept(2), bind(2), connect(2), getpeername(2), getsockname(2), getsockopt(2), listen(2), recv(2), send(2), shutdown(2)
syscon	errno	EDESTADDRREQ				89			89			39			39			39			39			39			10039			# destination address required; bsd consensus; WSAEDESTADDRREQ; raised by send(2), write(2)
syscon	errno	EMSGSIZE				90			90			40			40			40			40			40			10040			# message too long; bsd consensus; WSAEMSGSIZE; raised by keyctl(2), send(2), ip(7)
syscon	errno	EPROTOTYPE				91			91			41			41			41			41			41			10041			# protocol wrong type for socket; bsd consensus; WSAEPROTOTYPE; raised by connect(2), unix(7)
syscon	errno	ENOPROTOOPT				92			92			42			42			42			42			42			10042			# protocol not available; bsd consensus; WSAENOPROTOOPT; raised by getsockopt(2), accept(2), ip(7)
syscon	errno	EPROTONOSUPPORT				93			93			43			43			43			43			43			10043			# protocol not supported; bsd consensus; WSAEPROTONOSUPPORT; raised by socket(2), socketpair(2), unix(7)
syscon	errno	ESOCKTNOSUPPORT				94			94			44			44			44			44			44			10044			# socket type not supported; bsd consensus; WSAESOCKTNOSUPPORT; raised by unix(7), ip(7)
syscon	errno	ENOTSUP					95			95			45			45			45			91			86			10045			# operation not supported; raised by chmod(2), clock_getres(2), clock_nanosleep(2), getxattr(2), listxattr(2), removexattr(2), setxattr(2), timer_create(2), mmap(2)
syscon	errno	EOPNOTSUPP				95			95			102			102			45			45			45			10045			# socket operation not supported; raised by accept(2), fallocate(2), fanotify_mark(2), ioctl_ficlonerange(2), ioctl_fideduperange(2), ioctl_getfsmap(2), keyctl(2), listen(2), mmap(2), open_by_handle_at(2), pciconfig_read(2), perf_event_open(2), prctl(2), readv(2), s390_guarded_storage(2), s390_runtime_instr(2), s390_sthyi(2), send(2), socketpair(2), unix(7), ip(7)
syscon	errno	EPFNOSUPPORT				96			96			46			46			46			46			46			10046			# protocol family not supported; bsd consensus; WSAEPFNOSUPPORT
syscon	errno	EAFNOSUPPORT				97			97			47			47			47			47			47			10047			# address family not supported; bsd consensus; WSAEAFNOSUPPORT; raised by connect(2), socket(2), socketpair(2), tcp(7)
syscon	errno	EADDRINUSE				98			98			48			48			48			48			48			10048			# address already in use; bsd consensus; WSAEADDRINUSE; raised by bind(2), connect(2), listen(2), unix(7), ip(7)
syscon	errno	EADDRNOTAVAIL				99			99			49			49			49			49			49			10049			# address not available; bsd consensus; WSAEADDRNOTAVAIL; raised by bind(2), connect(2), kexec_load(2), ip(7)
syscon	errno	ENETDOWN				100			100			50			50			50			50			50			10050			# network is down; bsd consensus; WSAENETDOWN; raised by accept(2)
syscon	errno	ENETUNREACH				101			101			51			51			51			51			51			10051			# host is unreachable; bsd consensus; WSAENETUNREACH; raised by accept(2), connect(2)
syscon	errno	ENETRESET				102			102			52			52			52			52			52			10052			# connection reset by network; bsd consensus; WSAENETRESET
syscon	errno	ECONNABORTED				103			103			53			53			53			53			53			10053			# connection reset before accept; bsd consensus; WSAECONNABORTED; raised by accept(2)
syscon	errno	ECONNRESET				104			104			54			54			54			54			54			10054			# connection reset by client; bsd consensus; WSAECONNRESET; raised by send(2), unix(7)
syscon	errno	ENOBUFS					105			105			55			55			55			55			55			10055			# no buffer space available; bsd consensus; WSAENOBUFS; raised by getpeername(2), getsockname(2), send(2), ip(7)
syscon	errno	EISCONN					106			106			56			56			56			56			56			10056			# socket is connected; bsd consensus; WSAEISCONN; raised by connect(2), send(2), unix(7), ip(7)
syscon	errno	ENOTCONN				107			107			57			57			57			57			57			10057			# socket is not connected; bsd consensus; WSAENOTCONN; raised by getpeername(2), recv(2), send(2), shutdown(2), ip(7)
syscon	errno	ESHUTDOWN				108			108			58			58			58			58			58			10058			# cannot send after transport endpoint shutdown; note that shutdown write is an EPIPE; bsd consensus; WSAESHUTDOWN
syscon	errno	ETOOMANYREFS				109			109			59			59			59			59			59			10059			# too many references: cannot splice; bsd consensus; WSAETOOMANYREFS; raised by sendmsg(2), unix(7)
syscon	errno	ETIMEDOUT				110			110			60			60			60			60			60			1460			# connection timed out; kNtErrorTimeout; bsd consensus; WSAETIMEDOUT; raised by connect(2), futex(2), keyctl(2), tcp(7)
syscon	errno	ETIME					62			62			101			101			60			60			92			1460			# timer expired (POSIX.1 XSI STREAMS)
syscon	errno	ECONNREFUSED				111			111			61			61			61			61			61			10061			# bsd consensus; WSAECONNREFUSED; raised by connect(2), listen(2), recv(2), unix(7), udp(7)system-imposed limit on the number of threads was encountered.
syscon	errno	EHOSTDOWN				112			112			64			64			64			64			64			10064			# bsd consensus; WSAEHOSTDOWN; raised by accept(2)
syscon	errno	EHOSTUNREACH				113			113			65			65			65			65			65			10065			# bsd consensus; WSAEHOSTUNREACH; raised by accept(2), ip(7)
syscon	errno	EALREADY				114			114			37			37			37			37			37			10037			# connection already in progress; bsd consensus; WSAEALREADY; raised by connect(2), send(2), ip(7)
syscon	errno	EINPROGRESS				115			115			36			36			36			36			36			10036			# bsd consensus; WSAEINPROGRESS; raised by connect(2) w/ O_NONBLOCK
syscon	errno	ESTALE					116			116			70			70			70			70			70			10070			# bsd consensus; WSAESTALE; raised by open_by_handle_at(2)
syscon	errno	EREMOTE					66			66			71			71			71			71			71			10071			# bsd consensus
syscon	errno	EBADRPC					300			300			72			72			72			72			72			1626			# kNtErrorFunctionNotCalled; bsd consensus; made up on linux
syscon	errno	ERPCMISMATCH				301			301			73			73			73			73			73			1627			# kNtErrorFunctionFailed; bsd consensus; made up on linux
syscon	errno	EPROGUNAVAIL				302			302			74			74			74			74			74			329			# kNtErrorOperationInProgress; bsd consensus; made up on linux
syscon	errno	EPROGMISMATCH				303			303			75			75			75			75			75			595			# kNtErrorReplyMessageMismatch; bsd consensus; made up on linux
syscon	errno	EPROCUNAVAIL				304			304			76			76			76			76			76			15841			# kNtErrorApiUnavailable; bsd consensus; made up on linux
syscon	errno	EFTYPE					305			305			79			79			79			79			79			222			# Inappropriate file type or format; kNtErrorBadFileType; bsd consensus; made up on linux
syscon	errno	EAUTH					306			306			80			80			80			80			80			1244			# Authentication error; kNtErrorNotAuthenticated; bsd consensus; made up on linux
syscon	errno	ENEEDAUTH				307			307			81			81			81			81			81			224			# Need authenticator; kNtErrorFormsAuthRequired; bsd consensus; made up on linux
syscon	errno	EPROCLIM				308			308			67			67			67			67			67			10067			# bsd consensus; made up on linux
syscon	errno	ENOATTR					309			309			93			93			87			83			93			117			# Attribute not found; kNtErrorInvalidCategory; made up on linux
syscon	errno	EPWROFF					310			310			82			82			310			310			310			639			# Intelligent device errors. Device power is off; kNtErrorInsufficientPower; made up on non-xnu
syscon	errno	EDEVERR					311			311			83			83			311			311			311			483			# kNtErrorDeviceHardwareError; made up on non-xnu
syscon	errno	EBADEXEC				312			312			85			85			312			312			312			192			# kNtErrorExeMarkedInvalid; made up on non-xnu
syscon	errno	EBADARCH				313			313			86			86			313			313			313			216			# kNtErrorExeMachineTypeMismatch; made up on non-xnu
syscon	errno	ESHLIBVERS				314			314			87			87			314			314			314			0			# shiver me timbers; made up on non-xnu
syscon	errno	EBADMACHO				315			315			88			88			315			315			315			0			# made up on non-xnu
syscon	errno	ENOPOLICY				316			316			103			103			316			316			316			0			# made up on non-xnu
syscon	errno	EBADMSG					74			74			94			94			89			92			88			0			# raised by ioctl_getfsmap(2)
syscon	errno	ECANCELED				125			125			89			89			85			88			87			1223			# kNtErrorCancelled; raised by timerfd_create(2)
syscon	errno	EOWNERDEAD				130			130			105			105			96			94			97			105			# kNtErrorSemOwnerDied; raised by pthread_cond_timedwait(3), pthread_mutex_consistent(3), pthread_mutex_getprioceiling(3), pthread_mutex_lock(3), pthread_mutex_timedlock(3), pthread_mutexattr_getrobust(3), pthread_mutexattr_setrobust(3)
syscon	errno	ENOTRECOVERABLE				131			131			104			104			95			93			98			0			# raised by pthread_cond_timedwait(3), pthread_mutex_consistent(3), pthread_mutex_getprioceiling(3), pthread_mutex_lock(3), pthread_mutex_timedlock(3), pthread_mutexattr_getrobust(3), pthread_mutexattr_setrobust(3)
syscon	errno	ENONET					64			64			317			317			317			317			317			0			# made up on BSDs; raised by accept(2)
syscon	errno	ERESTART				85			85			318			318			318			318			-3			20000			# should only be seen in ptrace()
syscon	errno	ENODATA					61			61			96			96			0			0			89			232			# no message is available in xsi stream or named pipe is being closed; no data available; barely in posix; returned by ioctl; very close in spirit to EPIPE?
syscon	errno	ENOSR					63			63			98			98			0			90			90			0			# out of streams resources; something like EAGAIN; it's in POSIX; maybe some commercial UNIX returns it with openat, putmsg, putpmsg, posix_openpt, ioctl, open
syscon	errno	ENOSTR					60			60			99			99			0			0			91			0			# not a stream; returned by getmsg, putmsg, putpmsg, getpmsg
syscon	errno	EMULTIHOP				72			72			95			95			90			0			94			0			# barely in posix
syscon	errno	ENOLINK					67			67			97			97			91			0			95			0			# barely in posix
syscon	errno	ENOMEDIUM				123			123			0			0			0			85			0			0			# not posix; not documented
syscon	errno	EMEDIUMTYPE				124			124			0			0			0			86			0			0			# not posix; not documented
syscon	errno	EBADFD					77			77			9			9			0			0			0			0			# file descriptor in bad state
syscon	compat	EWOULDBLOCK				11			11			35			35			35			35			35			10035			# same as EAGAIN on every platform we've seen

#	signals
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	sig	SIGHUP					1			1			1			1			1			1			1			1			# terminal hangup or daemon reload; resumable; auto-broadcasted to process group; unix consensus & faked on nt
syscon	sig	SIGINT					2			2			2			2			2			2			2			2			# terminal ctrl-c keystroke; resumable; auto-broadcasted to process group; unix consensus & faked on nt; X3.159-1988
syscon	sig	SIGQUIT					3			3			3			3			3			3			3			3			# terminal ctrl-\ keystroke; resumable; unix consensus & faked on nt
syscon	sig	SIGILL					4			4			4			4			4			4			4			4			# illegal instruction; unresumable (unless you longjmp() or edit ucontex->rip+=ild(ucontex->rip)); unix consensus & faked on nt; X3.159-1988
syscon	sig	SIGTRAP					5			5			5			5			5			5			5			5			# int3 instruction; resumable; unix consensus & faked on nt
syscon	sig	SIGABRT					6			6			6			6			6			6			6			6			# process aborted; resumable; unix consensus & faked on nt; X3.159-1988
syscon	sig	SIGBUS					7			7			10			10			10			10			10			7			# valid memory access that went beyond underlying end of file; bsd consensus
syscon	sig	SIGFPE					8			8			8			8			8			8			8			8			# illegal math; unresumable (unless you longjmp() or edit ucontex->rip+=ild(ucontex->rip)); unix consensus & faked on nt; X3.159-1988
syscon	sig	SIGKILL					9			9			9			9			9			9			9			9			# terminate with extreme prejudice; unreceivable; unix consensus & faked on nt
syscon	sig	SIGUSR1					10			10			30			30			30			30			30			10			# do whatever you want; bsd consensus
syscon	sig	SIGSEGV					11			11			11			11			11			11			11			11			# invalid memory access; unresumable (unless you longjmp() or edit ucontex->rip+=ild(ucontex->rip)); unix consensus & faked on nt; X3.159-1988
syscon	sig	SIGUSR2					12			12			31			31			31			31			31			12			# do whatever you want; bsd consensus
syscon	sig	SIGPIPE					13			13			13			13			13			13			13			13			# write to closed file descriptor; unix consensus & faked on nt
syscon	sig	SIGALRM					14			14			14			14			14			14			14			14			# sent by setitimer(2) or timer_settime(2); unix consensus & faked on nt
syscon	sig	SIGTERM					15			15			15			15			15			15			15			15			# terminate; resumable; unix consensus & faked on nt; X3.159-1988
syscon	sig	SIGSTKFLT				16			16			0			0			0			0			0			0			# wut
syscon	sig	SIGCHLD					17			17			20			20			20			20			20			17			# child process exited or terminated and is now a zombie (unless this is SIG_IGN or SA_NOCLDWAIT) or child process stopped due to terminal i/o or profiling/debugging (unless you used SA_NOCLDSTOP); bsd consensus
syscon	sig	SIGCONT					18			18			19			19			19			19			19			18			# child process resumed from profiling/debugging; bsd consensus
syscon	sig	SIGSTOP					19			19			17			17			17			17			17			19			# child process stopped due to profiling/debugging; bsd consensus
syscon	sig	SIGTSTP					20			20			18			18			18			18			18			20			# terminal ctrl-z keystroke; bsd consensus
syscon	sig	SIGTTIN					21			21			21			21			21			21			21			21			# terminal input for background process; resumable; unix consensus & faked on nt
syscon	sig	SIGTTOU					22			22			22			22			22			22			22			22			# terminal output for background process; resumable; unix consensus & faked on nt
syscon	sig	SIGURG					23			23			16			16			16			16			16			23			# bsd consensus
syscon	sig	SIGXCPU					24			24			24			24			24			24			24			24			# cpu time limit exceeded; unix consensus & faked on nt
syscon	sig	SIGXFSZ					25			25			25			25			25			25			25			25			# file size limit exceeded; unix consensus & faked on nt
syscon	sig	SIGVTALRM				26			26			26			26			26			26			26			26			# virtual alarm clock; wut; unix consensus & faked on nt
syscon	sig	SIGPROF					27			27			27			27			27			27			27			27			# profiling timer expired; unix consensus & faked on nt
syscon	sig	SIGWINCH				28			28			28			28			28			28			28			28			# terminal resized; unix consensus & faked on nt
syscon	sig	SIGIO					29			29			23			23			23			23			23			29			# bsd consensus
syscon	sig	SIGSYS					31			31			12			12			12			12			12			31			# wut; bsd consensus
syscon	sig	SIGINFO					63			63			29			29			29			29			29			63			# bsd consensus
syscon	sig	SIGEMT					64			64			7			7			7			7			7			64			# not implemented in most community editions of system five; consider doing this using SIGUSR1 or SIGUSR2 instead
syscon	sig	SIGPWR					30			30			30			30			30			30			32			30			# not implemented in most community editions of system five; consider doing this using SIGUSR1 or SIGUSR2 instead
syscon	sig	SIGTHR					32			32			7			7			32			32			33			32			# used by pthread_cancel(); SIGRTMIN+0 on Linux/NetBSD; faked as SIGEMT on XNU (what is SIG32 on XNU anyway?)
syscon	sig	SIGRTMIN				32			32			0			0			65			0			33			32
syscon	sig	SIGRTMAX				64			64			0			0			126			0			63			64
syscon	compat	SIGPOLL					29			29			23			23			23			23			23			29			# same as SIGIO
syscon	compat	SIGIOT					6			6			6			6			6			6			6			6			# PDP-11 feature; same as SIGABRT

#	open() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			Windoze			Commentary
syscon	open	O_APPEND				0x00000400		0x00000400		8			8			8			8			8			0x00000400		# bsd consensus & kNtFileAppendData; won't pose issues w/ mknod(S_IFIFO)
syscon	open	O_CREAT					0x00000040		0x00000040		0x00000200		0x00000200		0x00000200		0x00000200		0x00000200		0x00000040		# bsd consensus & NT faked as Linux
syscon	open	O_EXCL					0x00000080		0x00000080		0x00000800		0x00000800		0x00000800		0x00000800		0x00000800		0x00000080		# bsd consensus & NT faked as Linux
syscon	open	O_TRUNC					0x00000200		0x00000200		0x00000400		0x00000400		0x00000400		0x00000400		0x00000400		0x00000200		# bsd consensus & NT faked as Linux
syscon	open	O_DIRECTORY				0x00010000		0x00004000		0x00100000		0x00100000		0x00020000		0x00020000		0x00200000		0x00010000		# useful hint on UNIX, but required on NT (see kNtFileFlagBackupSemantics)
syscon	open	O_NOFOLLOW				0x00020000		0x00008000		0x00000100		0x00000100		0x00000100		0x00000100		0x00000100		0x00020000	       	# don't follow symlinks in the final path component; bsd consensus; kNtFileFlagOpenReparsePoint
syscon	open	O_DIRECT				0x00004000		0x00010000		0xffffffff		0xffffffff		0x00010000		0xffffffff		0x00080000		0x00004000		# kNtFileFlagNoBuffering
syscon	open	O_NONBLOCK				0x00000800		0x00000800		0x00000004		0x00000004		0x00000004		0x00000004		0x00000004		0x00000800		# same as O_NDELAY; overlaps with kNtFileFlagWriteThrough which we don't actually pass to win32 (we implement non-blocking ourselves using overlapped i/o)
syscon	open	O_RANDOM				0			0			0			0			0			0			0			0x80000000		# kNtFileFlagRandomAccess
syscon	open	O_SEQUENTIAL				0			0			0			0			0			0			0			0x40000000		# kNtFileFlagSequentialScan
syscon	open	O_COMPRESSED				0			0			0			0			0			0			0			0x20000000		# kNtFileAttributeCompressed
syscon	open	O_INDEXED				0			0			0			0			0			0			0			0x10000000		# !kNtFileAttributeNotContentIndexed
syscon	open	O_CLOEXEC				0x00080000		0x00080000		0x01000000		0x01000000		0x00100000		0x00010000		0x00400000		0x00080000		# NT faked as Linux
syscon	open	O_TMPFILE				0x00410000		0x00404000		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		# please use tmpfd(); Linux 3.11+ (c. 2013) __O_TMPFILE | O_DIRECTORY; kNtFileAttributeTemporary|kNtFileFlagDeleteOnClose
syscon	open	O_UNLINK				0x40000000		0x40000000		0x40000000		0x40000000		0x40000000		0x40000000		0x40000000		0x04000100		# kNtFileAttributeTemporary|kNtFileFlagDeleteOnClose on WIN32; polyfilled w/ unlink() on UNIX
syscon	open	O_ASYNC					0x00002000		0x00002000		0x00000040		0x00000040		0x00000040		0x00000040		0x00000040		0xffffffff		# bsd consensus
syscon	open	O_NOFOLLOW_ANY				0xffffffff		0xffffffff		0x20000000		0x20000000		0xffffffff		0xffffffff		0xffffffff		0xffffffff		# don't follow symlinks in any path component
syscon	open	O_SYNC					0x00101000		0x00101000		0x00000080		0x00000080		0x00000080		0x00000080		0x00000080		0xffffffff		# bsd consensus
syscon	open	O_NOCTTY				0x00000100		0x00000100		0x00020000		0x00020000		0x00008000		0x00008000		0x00008000		0			# used for remote viewing (default behavior on freebsd)
syscon	open	O_NOATIME				0x00040000		0x00040000		0			0			0			0			0			0			# optimize away access time update
syscon	open	O_EXEC					0x00200000		0x00200000		0			0x40000000		0x00040000		0			0x04000000		0			# open only for executing (POSIX.1 hack for when file mode is 0111); see fexecve(); O_PATH on Linux
syscon	open	O_SEARCH				0xffffffff		0xffffffff		0xffffffff		0x40100000		0x00040000		0xffffffff		0x00800000		0xffffffff		# it's specified by posix what does it mean
syscon	open	O_DSYNC					0x00001000		0x00001000		0x00400000		0x00400000		0xffffffff		0x00000080		0x00010000		0xffffffff		#
syscon	open	O_RSYNC					0x00101000		0x00101000		0xffffffff		0xffffffff		0xffffffff		0x00000080		0x00020000		0xffffffff		#
syscon	open	O_PATH					0x00200000		0x00200000		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		# Linux 2.6.39+
syscon	open	O_VERIFY				0xffffffff		0xffffffff		0xffffffff		0xffffffff		0x00200000		0xffffffff		0xffffffff		0xffffffff		#
syscon	open	O_SHLOCK				0xffffffff		0xffffffff		0x00000010		0x00000010		0x00000010		0x00000010		0x00000010		0xffffffff		#
syscon	open	O_EXLOCK				0xffffffff		0xffffffff		0x00000020		0x00000020		0x00000020		0x00000020		0x00000020		0xffffffff		#
syscon	open	O_TTY_INIT				0			0			0			0			0x00080000		0			0			0			#
syscon	compat	O_LARGEFILE				0x00008000		0x00020000		0			0			0			0			0			0			#

#	mmap() flags
#	the revolutionary praxis of malloc()
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	mmap	MAP_FILE				0			0			0			0			0			0			0			0			# consensus
syscon	mmap	MAP_SHARED				1			1			1			1			1			1			1			1			# forced consensus & faked nt
syscon	mmap	MAP_SHARED_VALIDATE			3			3			1			1			1			1			1			1			# weird linux thing
syscon	mmap	MAP_PRIVATE				2			2			2			2			2			2			2			2			# forced consensus & faked nt
syscon	mmap	MAP_STACK				6			6			6			6			6			6			6			6			# our definition
syscon	mmap	MAP_TYPE				15			15			15			15			15			15			15			15			# mask for type of mapping
syscon	mmap	MAP_FIXED				0x00000010		0x00000010		0x00000010		0x00000010		0x00000010		0x00000010		0x00000010		0x00000010		# unix consensus; openbsd appears to forbid; faked nt
syscon	mmap	MAP_FIXED_NOREPLACE			0x08000000		0x08000000		0x00004010		0x00004010		0x08000000		0x08000000		0x08000000		0x08000000     		# handled and defined by cosmo runtime; 0x100000 on linux 4.7+; MAP_FIXED|MAP_EXCL on FreeBSD
syscon	mmap	MAP_ANONYMOUS				0x00000020		0x00000020		0x00001000		0x00001000		0x00001000		0x00001000		0x00001000		0x00000020		# bsd consensus; faked nt
syscon	mmap	MAP_GROWSDOWN				0x00000100		0x00000100		0			0			0			0			0			0			# use MAP_STACK; abstracted by MAP_STACK; may be passed to __sys_mmap() for low-level Linux fiddling
syscon	mmap	MAP_LOCKED				0x00002000		0x00002000		0			0			0			0			0			0
syscon	mmap	MAP_NORESERVE				0x00004000		0x00004000		0x00000040		0x00000040		0			0			0x00000040		0			# Linux calls it "reserve"; NT calls it "commit"? which is default?
syscon	mmap	MAP_POPULATE				0x00008000		0x00008000		0			0			0x00040000		0			0			0			# MAP_PREFAULT_READ on FreeBSD; can avoid madvise(MADV_WILLNEED) on private file mapping
syscon	mmap	MAP_NONBLOCK				0x00010000		0x00010000		0			0			0			0			0			0
syscon	mmap	MAP_SYNC				0x00080000		0x00080000		0			0			0			0			0			0			# perform synchronous page faults for mapping (Linux 4.15+)
syscon	mmap	MAP_INHERIT				-1			-1			-1			-1			-1			-1			0x00000080		-1			# make it inherit across execve()
syscon	mmap	MAP_HASSEMAPHORE			0			0			0x00000200		0x00000200		0x00000200		0			0x00000200		0			# does it matter on x86?
syscon	mmap	MAP_NOSYNC				0			0			0			0			0x00000800		0			0			0			# flush to physical media only when necessary rather than gratuitously; be sure to use write() rather than ftruncate() with this!
syscon	mmap	MAP_CONCEAL				0			0			0			0			0x00020000		0x00008000		0x00008000		0			# omit from core dumps; MAP_NOCORE on FreeBSD
syscon	mmap	MAP_JIT					0			0			0			0x00000800		0			0			0			0			# allocate region used for just-in-time compilation
syscon	mmap	MAP_NOCACHE				0			0			0x00000400		0x00000400		0			0			0			0			# don't cache pages for this mapping
syscon	mmap	MAP_NOEXTEND				0			0			0x00000100		0x00000100		0			0			0			0			# for MAP_FILE, don't change file size
syscon	compat	MAP_NOCORE				0			0			0			0			0x00020000		0x00008000		0x00008000		0			# use MAP_CONCEAL
syscon	compat	MAP_ANON				0x00000020		0x00000020		0x00001000		0x00001000		0x00001000		0x00001000		0x00001000		0x00000020		# bsd consensus; faked nt
syscon	compat	MAP_EXECUTABLE				0x00001000		0x00001000		0			0			0			0			0			0			# ignored
syscon	compat	MAP_DENYWRITE				0x00000800		0x00000800		0			0			0			0			0			0
syscon	compat	MAP_32BIT				0x00000040		0x00000040		0			0x00008000		0x00080000		0			0			0			# iffy

#	madvise() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	madv	MADV_NORMAL				0			0			0			0			0			0			0			0			# consensus
syscon	madv	POSIX_FADV_NORMAL			0			0			0			0			0			0			0			0			# consensus
syscon	madv	POSIX_MADV_NORMAL			0			0			0			0			0			0			0			0			# consensus
syscon	madv	MADV_DONTNEED				4			4			4			4			4			4			4			127			# TODO(jart): weird nt decommit thing?
syscon	madv	POSIX_MADV_DONTNEED			4			4			4			4			4			4			4			127			# unix consensus
syscon	madv	POSIX_FADV_DONTNEED			4			4			127			127			4			4			4			127			# unix consensus
syscon	madv	MADV_RANDOM				1			1			1			1			1			1			1			1			# unix consensus
syscon	madv	POSIX_MADV_RANDOM			1			1			1			1			1			1			1			1			# unix consensus
syscon	madv	POSIX_FADV_RANDOM			1			1			127			127			1			1			1			1			# unix consensus
syscon	madv	MADV_SEQUENTIAL				2			2			2			2			2			2			2			2			# unix consensus
syscon	madv	POSIX_MADV_SEQUENTIAL			2			2			2			2			2			2			2			2			# unix consensus
syscon	madv	POSIX_FADV_SEQUENTIAL			2			2			127			127			2			2			2			2			# TODO(jart): double check xnu
syscon	madv	MADV_WILLNEED				3			3			3			3			3			3			3			3			# unix consensus (faked on NT)
syscon	madv	POSIX_MADV_WILLNEED			3			3			3			3			3			3			3			3			# unix consensus
syscon	madv	POSIX_FADV_WILLNEED			3			3			127			127			3			3			3			3			# TODO(jart): double check xnu
syscon	madv	MADV_MERGEABLE				12			12			127			127			127			127			127			127			# turns on (private anon range) page scanning and merging service (linux only)
syscon	madv	MADV_UNMERGEABLE			13			13			127			127			127			127			127			127			# turns off mergeable (linux only)
syscon	madv	MADV_FREE				8			8			5			5			5			6			6			8			# Linux 4.5+ (c. 2016) / NT Faked → VMOfferPriorityNormal (Win8+)
syscon	madv	MADV_HUGEPAGE				14			14			127			127			127			127			127			127			# TODO(jart): why would we need it?
syscon	madv	MADV_NOHUGEPAGE				15			15			127			127			127			127			127			127			# TODO(jart): why would we need it?
syscon	madv	MADV_DODUMP				17			17			127			127			127			127			127			127			# TODO(jart): what is it?
syscon	madv	MADV_WIPEONFORK				18			18			127			127			127			127			127			127			# TODO: add support ?
syscon	madv	MADV_KEEPONFORK				19			19			127			127			127			127			127			127			# TODO: add support ?
syscon	madv	MADV_COLD				20			20			127			127			127			127			127			127			# TODO: add support ?
syscon	madv	MADV_PAGEOUT				21			21			127			127			127			127			127			127			# TODO: add support ?
syscon	madv	MADV_POPULATE_READ			22			22			127			127			127			127			127			127			# TODO: add support ?
syscon	madv	MADV_POPULATE_WRITE			23			23			127			127			127			127			127			127			# TODO: add support ?
syscon	madv	MADV_DONTNEED_LOCKED			24			24			127			127			127			127			127			127			# TODO: add support ?
syscon	madv	MADV_COLLAPSE				25			25			127			127			127			127			127			127			# TODO: add support ?
syscon	madv	MADV_DOFORK				11			11			127			127			127			127			127			127			# TODO(jart): what is it?
syscon	madv	MADV_DONTDUMP				16			16			127			127			127			127			127			127			# see MAP_CONCEAL in OpenBSD; TODO(jart): what is it?
syscon	madv	MADV_DONTFORK				10			10			127			127			127			127			127			127			# TODO(jart): what is it?
syscon	madv	MADV_HWPOISON				100			100			127			127			127			127			127			127			# TODO(jart): what is it?
syscon	madv	MADV_SOFT_OFFLINE			101			101			127			127			127			127			127			127			# TODO: add support ?
syscon	madv	MADV_REMOVE				9			9			127			127			127			127			127			127			# TODO(jart): what is it?
syscon	fadv	POSIX_FADV_NOREUSE			5			5			127			127			5			127			5			127			# wut
syscon	madv	MADV_REMOVE				9			9			127			127			127			127			127			127			# TODO(jart): what is it?

#	mmap(), mprotect(), etc.
#	digital restrictions management for the people
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	mprot	PROT_NONE				0			0			0			0			0			0			0			0			# mmap, mprotect, unix consensus (nt needs special business logic here)
syscon	mprot	PROT_READ				1			1			1			1			1			1			1			1			# mmap, mprotect, unix consensus
syscon	mprot	PROT_WRITE				2			2			2			2			2			2			2			2			# mmap, mprotect, unix consensus
syscon	mprot	PROT_EXEC				4			4			4			4			4			4			4			4			# mmap, mprotect, unix consensus
syscon	mprot	PROT_GROWSDOWN				0x01000000		0x01000000		0			0			0			0			0			0			# intended for mprotect; see MAP_GROWSDOWN for mmap() (todo: what was 0x01000000 on nt)
syscon	mprot	PROT_GROWSUP				0x02000000		0x02000000		0			0			0			0			0			0			# intended for mprotect; see MAP_GROWSDOWN for mmap()

#	mremap() flags
#	the revolutionary praxis of realloc()
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	mremap	MREMAP_MAYMOVE				1			1			1			1			1			1			1			1			# faked non-linux (b/c linux only)
syscon	mremap	MREMAP_FIXED				2			2			2			2			2			2			2			2			# faked non-linux (b/c linux only)

#	sigprocmask() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	misc	SIG_BLOCK				0			0			1			1			1			1			1			0			# bsd consensus; faked nt
syscon	misc	SIG_UNBLOCK				1			1			2			2			2			2			2			1			# bsd consensus; faked nt
syscon	misc	SIG_SETMASK				2			2			3			3			3			3			3			2			# bsd consensus; faked nt

#	lseek() whence
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	splice	SEEK_HOLE				4			4			3			3			4			-1			-1			-1			#
syscon	splice	SEEK_DATA				3			3			4			4			3			-1			-1			-1			#

#	splice() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	splice	SPLICE_F_MOVE				1			1			0			0			0			0			0			0			# can be safely ignored by polyfill; it's a hint
syscon	splice	SPLICE_F_NONBLOCK			2			2			0			0			0			0			0			0			# can be safely ignored by polyfill, since linux says it doesn't apply to underlying FDs
syscon	splice	SPLICE_F_MORE				4			4			0			0			0			0			0			0			# can be safely ignored by polyfill; it's a hint
syscon	splice	SPLICE_F_GIFT				8			8			0			0			0			0			0			0			# can probably be ignored by polyfill

#	access() flags
#	libc/sysv/consts/ok.h
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	access	X_OK					1			1			1			1			1			1			1			0x20000000		# unix consensus and kNtGenericExecute
syscon	access	W_OK					2			2			2			2			2			2			2			0x40000000		# unix consensus and kNtGenericWrite
syscon	access	R_OK					4			4			4			4			4			4			4			0x80000000		# unix consensus and kNtGenericRead

#	flock() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	lock	LOCK_SH					1			1			1			1			1			1			1			0			# shared [unix consensus]; hard-coded into flock-nt.c too
syscon	lock	LOCK_EX					2			2			2			2			2			2			2			2			# exclusive [consensus!] a.k.a. kNtLockfileExclusiveLock
syscon	lock	LOCK_NB					4			4			4			4			4			4			4			1			# non-blocking [unix consensus] a.k.a. kNtLockfileFailImmediately
syscon	lock	LOCK_UN					8			8			8			8			8			8			8			8			# unlock [unix consensus & faked NT]; hard-coded into flock-nt.c too

#	waitpid() / wait4() options
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	waitpid	WNOHANG					1			1			1			1			1			1			1			1			# helps you reap zombies; unix consensus; fake nt
syscon	waitpid	WUNTRACED				2			2			2			2			2			2			2			0			# unix consensus
syscon	waitpid	WCONTINUED				8			8			0x10			0x10			4			8			16			0			#

#	waitid() options
#	no dice on openbsd >:\
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	waitid	WEXITED					4			4			4			4			0x10			0			32			0
syscon	waitid	WSTOPPED				2			2			8			8			2			0			2			0
syscon	waitid	WNOWAIT					0x01000000		0x01000000		0x20			0x20			8			0			0x10000			0

#	fcntl()
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	fcntl2	F_DUPFD					0			0			0			0			0			0			0			0			# consensus
syscon	fcntl2	F_GETFD					1			1			1			1			1			1			1			1			# unix consensus & faked nt
syscon	fcntl2	F_SETFD					2			2			2			2			2			2			2			2			# unix consensus & faked nt
syscon	fcntl2	F_GETFL					3			3			3			3			3			3			3			3			# unix consensus & faked nt
syscon	fcntl2	F_SETFL					4			4			4			4			4			4			4			4			# unix consensus & faked nt
syscon	fcntl	F_DUPFD_CLOEXEC				0x0406			0x0406			67			67			17			10			12			0x0406			# Linux 2.6.24+; faked nt
syscon	fcntl2	F_SETOWN				8			8			6			6			6			6			6			-1			# bsd consensus
syscon	fcntl2	F_GETOWN				9			9			5			5			5			5			5			-1			# bsd consensus
syscon	fcntl2	F_SETOWN_EX				15			15			-1			-1			-1			-1			-1			-1			# TODO(jart): polyfill
syscon	fcntl2	F_GETOWN_EX				16			16			-1			-1			-1			-1			-1			-1			# TODO(jart): polyfill
syscon	fcntl2	F_FULLFSYNC				-1			-1			51			51			-1			-1			-1			-1			#
syscon	fcntl2	F_BARRIERFSYNC				-1			-1			85			85			-1			-1			-1			-1			#
syscon	fcntl2	F_NOCACHE				-1			-1			48			48			-1			-1			-1			-1			#
syscon	fcntl3	F_SETNOSIGPIPE				-1			-1			73			73			-1			-1			14			-1			# 
syscon	fcntl3	F_GETNOSIGPIPE				-1			-1			74			74			-1			-1			13			-1			# 
syscon	fcntl3	F_GETPATH				-1			-1			50			50			-1			-1			15			-1			# geth path associated with fd into buffer with PATH_MAX (1024) bytes
syscon	fcntl3	FD_CLOEXEC				1			1			1			1			1			1			1			1			# unix consensus & faked nt
syscon	fcntl	F_MAXFD					-1			-1			-1			-1			-1			-1			11			-1			#
syscon	fcntl	F_NOTIFY				0x0402			0x0402			-1			-1			-1			-1			-1			-1
syscon	fcntl	F_SETPIPE_SZ				0x0407			0x0407			-1			-1			-1			-1			-1			-1
syscon	fcntl	F_GETPIPE_SZ				0x0408			0x0408			-1			-1			-1			-1			-1			-1
syscon	fcntl	FREAD					0			0			1			1			1			1			1			0			# wut is it
syscon	fcntl	FWRITE					0			0			2			2			2			2			2			0			# wut is it

#       fcntl3	O_NONBLOCK
#       fcntl3	O_APPEND
#       fcntl3	O_ASYNC
#       fcntl3	O_DIRECT
#       fcntl3	O_NOATIME

#	fcntl() POSIX Advisory Locks
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	fcntl	F_SETLK					6			6			8			8			12			8			8			6			# polyfilled nt
syscon	fcntl	F_SETLKW				7			7			9			9			13			9			9			7			# polyfilled nt
syscon	fcntl	F_GETLK					5			5			7			7			11			7			7			5			# polyfilled nt
syscon	fcntl	F_OFD_SETLK				37			37			-1			-1			-1			-1			-1			-1			# listed in xnu source code but marked private
syscon	fcntl	F_OFD_SETLKW				38			38			-1			-1			-1			-1			-1			-1			# listed in xnu source code but marked private
syscon	fcntl	F_OFD_GETLK				36			36			-1			-1			-1			-1			-1			-1			# listed in xnu source code but marked private
syscon	fcntl	F_RDLCK					0			0			1			1			1			1			1			0			# polyfilled nt; bsd consensus
syscon	fcntl	F_WRLCK					1			1			3			3			3			3			3			1			# polyfilled nt; bsd consensus
syscon	fcntl	F_UNLCK					2			2			2			2			2			2			2			2			# polyfilled nt; unix consensus

syscon	fcntl	F_SETSIG				10			10			-1			-1			-1			-1			-1			-1
syscon	fcntl	F_GETSIG				11			11			-1			-1			-1			-1			-1			-1
syscon	fcntl	F_SETOWN_EX				15			15			-1			-1			-1			-1			-1			-1
syscon	fcntl	F_GETOWN_EX				0x10			0x10			-1			-1			-1			-1			-1			-1
syscon	fcntl	F_SETLEASE				0x0400			0x0400			-1			106			-1			-1			-1			-1
syscon	fcntl	F_GETLEASE				0x0401			0x0401			-1			107			-1			-1			-1			-1

#	openat(), fstatat(), linkat(), etc. magnums
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	at	AT_FDCWD				-100			-100			-2			-2			-100			-100			-100			-100			# faked nt
syscon	at	AT_SYMLINK_NOFOLLOW			0x0100			0x0100			0x20			0x20			0x0200			2			0x200			0x0100			# faked nt
syscon	at	AT_SYMLINK_FOLLOW			0x0400			0x0400			0x40			0x40			0x0400			4			0x400			0x0400			# see linkat(2)
syscon	at	AT_REMOVEDIR				0x0200			0x0200			0x80			0x80			0x0800			8			0x800			0x0200			# faked nt
syscon	at	AT_EACCESS				0x0200			0x0200			0x10			0x10			0x0100			1			0x100			0			# performs check using effective uid/gid; unnecessary nt
syscon	at	AT_EMPTY_PATH				0x1000			0x1000			0			0			0			0			0			0x1000			# linux 2.6.39+; see unlink, O_TMPFILE, etc.

#	utimensat() special values
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	utime	UTIME_NOW				0x3fffffff		0x3fffffff		-1			-1			-1			-2			0x3fffffff		-2			# timespec::tv_sec may be this; polyfilled xnu/nt
syscon	utime	UTIME_OMIT				0x3ffffffe		0x3ffffffe		-2			-2			-2			-1			0x3ffffffe		-1			# timespec::tv_nsec may be this; polyfilled xnu/nt

#	getauxval() keys
#	libc/sysv/consts/auxv.h
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	auxv	AT_EXECFN				31			31			31			31			15			31			2014			31			# address of string containing first argument passed to execve() used when running program; AT_EXECPATH on FreeBSD
syscon	auxv	AT_SECURE				23			23			23			23			0			23			0			23
syscon	auxv	AT_RANDOM				25			25			25			25			16			25			0			25			# address of sixteen bytes of random data; AT_CANARY on FreeBSD whose AT_CANARYLEN should be 64
syscon	auxv	AT_HWCAP				16			16			16			16			25			16			0			16
syscon	auxv	AT_HWCAP2				26			26			26			26			26			26			0			26
syscon	auxv	AT_UID					11			11			11			11			0			11			2001			11
syscon	auxv	AT_EUID					12			12			12			12			0			12			2000			12
syscon	auxv	AT_GID					13			13			13			13			0			13			2003			13
syscon	auxv	AT_EGID					14			14			14			14			0			14			2002			14
syscon	auxv	AT_EXECFD				2			2			0			0			2			2			2			2			# file descriptor of program
syscon	auxv	AT_NOTELF				10			10			0			0			10			10			0			10
syscon	auxv	AT_OSRELDATE				0			0			0			0			18			0			0			0
syscon	auxv	AT_PLATFORM				15			15			0			0			0			15			0			15			# address of string with hardware platform for rpath interpretation
syscon	auxv	AT_CLKTCK				17			17			0			0			0			17			0			17
syscon	auxv	AT_DCACHEBSIZE				19			19			0			0			0			19			0			19
syscon	auxv	AT_ICACHEBSIZE				20			20			0			0			0			20			0			20
syscon	auxv	AT_UCACHEBSIZE				21			21			0			0			0			21			0			21
syscon	auxv	AT_BASE_PLATFORM			24			24			0			0			0			24			0			24
syscon	auxv	AT_SYSINFO_EHDR				33			33			0			0			0			33			0			33
syscon	auxv	AT_STACKBASE				0			0			0			0			0			0			13			0
syscon	auxv	AT_EXECPATH				31			31			31			31			15			31			2014			31			# FreeBSD name for AT_EXECFN
syscon	auxv	AT_MINSIGSTKSZ				51			51			0			0			0			51			0			51			# FreeBSD name for AT_EXECFN
syscon	auxv	AT_CANARY				0			0			0			0			16			0			0			0
syscon	auxv	AT_CANARYLEN				0			0			0			0			17			0			0			0
syscon	auxv	AT_NCPUS				0			0			0			0			19			0			0			0
syscon	auxv	AT_PAGESIZES				0			0			0			0			20			0			0			0
syscon	auxv	AT_PAGESIZESLEN				0			0			0			0			21			0			0			0
syscon	auxv	AT_TIMEKEEP				0			0			0			0			22			0			0			0
syscon	auxv	AT_STACKPROT				0			0			0			0			23			0			0			0
syscon	auxv	AT_EHDRFLAGS				0			0			0			0			24			0			0			0
syscon	auxv	AT_NO_AUTOMOUNT				0x0800			0x0800			0			0			0			0x0800			0			0x0800

#	getrlimit() / setrlimit() resource parameter
#
#	Unsupported values are encoded as 127.
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	rlimit	RLIMIT_CPU				0			0			0			0			0			0			0			127			# max cpu time in seconds; see SIGXCPU; unix consensus
syscon	rlimit	RLIMIT_FSIZE				1			1			1			1			1			1			1			127			# max file size in bytes; unix consensus
syscon	rlimit	RLIMIT_DATA				2			2			2			2			2			2			2			127			# max mmap() / brk() / sbrk() size in bytes; unix consensus
syscon	rlimit	RLIMIT_STACK				3			3			3			3			3			3			3			1			# max stack size in bytes; see SIGXFSZ; unix consensus
syscon	rlimit	RLIMIT_CORE				4			4			4			4			4			4			4			127			# max core file size in bytes; unix consensus
syscon	rlimit	RLIMIT_RSS				5			5			5			5			5			5			5			127			# max physical memory size in bytes; see mmap()→ENOMEM; unix consensus
syscon	rlimit	RLIMIT_NPROC				6			6			7			7			7			7			7			127			# max number of processes; see fork()→EAGAIN; bsd consensus
syscon	rlimit	RLIMIT_NOFILE				7			7			8			8			8			8			8			127			# max number of open files; see accept()→EMFILE/ENFILE; bsd consensus
syscon	rlimit	RLIMIT_MEMLOCK				8			8			6			6			6			6			6			127			# max locked-in-memory address space; bsd consensus
syscon	rlimit	RLIMIT_AS				9\			9			5			5			10			2			10			0			# max virtual memory size in bytes; this one actually works; fudged as RLIMIT_DATA on OpenBSD
syscon	rlimit	RLIMIT_LOCKS				10			10			127			127			127			127			127			127			# max flock() / fcntl() locks; bsd consensus
syscon	rlimit	RLIMIT_SIGPENDING			11			11			127			127			127			127			127			127			# max sigqueue() can enqueue; bsd consensus
syscon	rlimit	RLIMIT_MSGQUEUE				12			12			127			127			127			127			127			127			# meh posix message queues; bsd consensus
syscon	rlimit	RLIMIT_NICE				13			13			127			127			127			127			127			127			# max scheduling priority; 𝑥 ∈ [1,40]; niceness is traditionally displayed as as 𝟸𝟶-𝑥, therefore 𝑥=1 (lowest priority) prints as 19 and 𝑥=40 (highest priority) prints as -20; bsd consensus
syscon	rlimit	RLIMIT_RTPRIO				14			14			127			127			127			127			127			127			# woop
syscon	rlimit	RLIMIT_RTTIME				15			15			127			127			127			127			127			127			# woop
syscon	rlimit	RLIMIT_SWAP				127			127			127			127			12			127			127			127			# swap used
syscon	rlimit	RLIMIT_SBSIZE				127			127			127			127			9			127			127			127			# max size of all socket buffers
syscon	rlimit	RLIMIT_NPTS				127			127			127			127			11			127			127			127			# pseudoteletypewriters
syscon	compat	RLIMIT_VMEM				9			9			5			5			10			127			10			127			# same as RLIMIT_AS

#	resource limit special values
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	rlim	RLIM_NLIMITS				16			16			9			9			15			9			12			2
syscon	rlim	RLIM_INFINITY				0xffffffffffffffff	0xffffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0xffffffffffffffff
syscon	rlim	RLIM_SAVED_CUR				0xffffffffffffffff	0xffffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0xffffffffffffffff
syscon	rlim	RLIM_SAVED_MAX				0xffffffffffffffff	0xffffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0x7fffffffffffffff	0xffffffffffffffff

#	sigaction() codes
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	sigact	SA_NOCLDSTOP				1			1			8			8			8			8			8			1			# lets you set SIGCHLD handler that's only notified on exit/termination and not notified on SIGSTOP/SIGTSTP/SIGTTIN/SIGTTOU/SIGCONT lool; bsd consensus
syscon	sigact	SA_NOCLDWAIT				2			2			32			32			32			32			32			2			# changes SIGCHLD so the zombie is gone and you can't call wait(2) anymore; similar to SIGCHLD+SIG_IGN but may still deliver the SIGCHLD; bsd consensus
syscon	sigact	SA_SIGINFO				4			4			64			64			64			64			64			4			# asks kernel to provide ucontext_t argument, which has mutable cpu/fpu state of signalled process; and it is polyfilled by cosmopolitan; bsd consensus
syscon	sigact	SA_ONSTACK				0x08000000		0x08000000		1			1			1			1			1			0x08000000		# causes signal handler to be called on stack provided by sigaltstack(2); bsd consensus
syscon	sigact	SA_RESTART				0x10000000		0x10000000		2			2			2			2			2			0x10000000		# prevents signal delivery from triggering EINTR on i/o calls (e.g. read/write/open/wait/accept) but doesn't impact non-i/o blocking calls (e.g. poll, sigsuspend, nanosleep) which will still EINTR; bsd consensus
syscon	sigact	SA_NODEFER				0x40000000		0x40000000		16			16			16			16			16			0x40000000		# lets signal handler be reentrant (e.g. so you can longjmp() out of signal handler); bsd consensus
syscon	sigact	SA_RESETHAND				0x80000000		0x80000000		4			4			4			4			4			0x80000000		# causes signal handler to be called at most once and then set to SIG_DFL automatically; bsd consensus
syscon	compat	SA_NOMASK				0x40000000		0x40000000		16			16			16			16			16			0x40000000		# same as SA_NODEFER
syscon	compat	SA_ONESHOT				0x80000000		0x80000000		4			4			4			4			4			0x80000000		# same as SA_RESETHAND

#	siginfo::si_code values
#
#	The New Technology NT is polyfilled as Linux.
#	Unsupported values are encoded as 0x80000000.
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	sicode	SI_USER					0			0			0x010001		0x010001		0x010001		0			0			0			# sent by kill(2); openbsd defines si_code<=0 as originating from user
syscon	sicode	SI_QUEUE				-1			-1			0x010002		0x010002		0x010002		-2			-1			-1			# sent by sigqueue(2)
syscon	sicode	SI_TIMER				-2			-2			0x010003		0x010003		0x010003		-3			-2			-2			# sent by setitimer(2) or clock_settime(2)
syscon	sicode	SI_TKILL				-6			-6			0x80000000		0x80000000		0x010007		-1			-5			-6			# sent by tkill(2) or tgkill(2) or thr_kill(2) or lwp_kill(2) or _lwp_kill(2); cries
syscon	sicode	SI_MESGQ				-3			-3			0x010005		0x010005		0x010005		0x80000000		-4			-3			# sent by mq_notify(2); lool
syscon	sicode	SI_ASYNCIO				-4			-4			0x010004		0x010004		0x010004		0x80000000		-3			-4			# aio completion; no thank you
syscon	sicode	SI_ASYNCNL				-60			-60			0x80000000		0x80000000		0x80000000		0x80000000		0x80000000		0x80000000		# aio completion for dns; the horror
syscon	sicode	SI_KERNEL				128			128			0x80000000		0x80000000		0x010006		0x80000000		0x80000000		0x80			# wut; openbsd defines as si_code>0
syscon	sicode	SI_NOINFO				32767			32767			0x80000000		0x80000000		0			32767			32767			32767			# no signal specific info available
syscon	sicode	CLD_EXITED				1			1			1			1			1			1			1			1			# SIGCHLD; child exited; unix consensus
syscon	sicode	CLD_KILLED				2			2			2			2			2			2			2			2			# SIGCHLD; child terminated w/o core; unix consensus
syscon	sicode	CLD_DUMPED				3			3			3			3			3			3			3			3			# SIGCHLD; child terminated w/ core; unix consensus
syscon	sicode	CLD_TRAPPED				4			4			4			4			4			4			4			4			# SIGCHLD; traced child trapped; unix consensus
syscon	sicode	CLD_STOPPED				5			5			5			5			5			5			5			5			# SIGCHLD; child stopped; unix consensus
syscon	sicode	CLD_CONTINUED				6			6			6			6			6			6			6			6			# SIGCHLD; stopped child continued; unix consensus
syscon	sicode	TRAP_BRKPT				1			1			1			1			1			1			1			1			# SIGTRAP; process breakpoint; unix consensus
syscon	sicode	TRAP_TRACE				2			2			2			2			2			2			2			2			# SIGTRAP; process trace trap; unix consensus
syscon	sicode	SEGV_MAPERR				1			1			1			1			1			1			1			1			# SIGSEGV; address not mapped to object; unix consensus
syscon	sicode	SEGV_ACCERR				2			2			2			2			2			2			2			2			# SIGSEGV; invalid permissions for mapped object; unix consensus
syscon	sicode	SEGV_PKUERR				-1			-1			-1			-1			100			-1			-1			-1			# SIGSEGV: x86: PKU violation
syscon	sicode	FPE_INTDIV				1			1			7			7			2			1			1			1			# SIGFPE; integer divide by zero
syscon	sicode	FPE_INTOVF				2			2			8			8			1			2			2			2			# SIGFPE; integer overflow
syscon	sicode	FPE_FLTDIV				3			3			1			1			3			3			3			3			# SIGFPE; floating point divide by zero
syscon	sicode	FPE_FLTOVF				4			4			2			2			4			4			4			4			# SIGFPE; floating point overflow
syscon	sicode	FPE_FLTUND				5			5			3			3			5			5			5			5			# SIGFPE; floating point underflow
syscon	sicode	FPE_FLTRES				6			6			4			4			6			6			6			6			# SIGFPE; floating point inexact
syscon	sicode	FPE_FLTINV				7			7			5			5			7			7			7			7			# SIGFPE; invalid floating point operation
syscon	sicode	FPE_FLTSUB				8			8			6			6			8			8			8			8			# SIGFPE; subscript out of range
syscon	sicode	ILL_ILLOPC				1			1			1			1			1			1			1			1			# SIGILL; illegal opcode; unix consensus
syscon	sicode	ILL_ILLOPN				2			2			4			4			2			2			2			2			# SIGILL; illegal operand
syscon	sicode	ILL_ILLADR				3			3			5			5			3			3			3			3			# SIGILL; illegal addressing mode
syscon	sicode	ILL_ILLTRP				4			4			2			2			4			4			4			4			# SIGILL; illegal trap
syscon	sicode	ILL_PRVOPC				5			5			3			3			5			5			5			5			# SIGILL; privileged opcode
syscon	sicode	ILL_PRVREG				6			6			6			6			6			6			6			6			# SIGILL; privileged register; unix consensus
syscon	sicode	ILL_COPROC				7			7			7			7			7			7			7			7			# SIGILL; coprocessor error; unix consensus
syscon	sicode	ILL_BADSTK				8			8			8			8			8			8			8			8			# SIGILL; internal stack error; unix consensus
syscon	sicode	BUS_ADRALN				1			1			1			1			1			1			1			1			# SIGBUS; invalid address alignment; unix consensus
syscon	sicode	BUS_ADRERR				2			2			2			2			2			2			2			2			# SIGBUS; non-existent physical address; unix consensus
syscon	sicode	BUS_OBJERR				3			3			3			3			3			3			3			3			# SIGBUS; object specific hardware error; unix consensus
syscon	sicode	BUS_OOMERR				-1			-1			-1			-1			100			-1			-1			-1			# SIGBUS; Non-standard: No memory.
syscon	sicode	BUS_MCEERR_AR				4			4			0x80000000		0x80000000		0x80000000		0x80000000		0x80000000		0x80000000		# SIGBUS; Linux 2.6.32+
syscon	sicode	BUS_MCEERR_AO				5			5			0x80000000		0x80000000		0x80000000		0x80000000		0x80000000		0x80000000		# SIGBUS; Linux 2.6.32+
syscon	sicode	POLL_IN					1			1			1			1			1			1			1			1			# SIGIO; data input available; unix consensus
syscon	sicode	POLL_OUT				2			2			2			2			2			2			2			2			# SIGIO; output buffer available; unix consensus
syscon	sicode	POLL_MSG				3			3			3			3			3			3			3			3			# SIGIO; input message available; unix consensus
syscon	sicode	POLL_ERR				4			4			4			4			4			4			4			4			# SIGIO; i/o error; unix consensus
syscon	sicode	POLL_PRI				5			5			5			5			5			5			5			5			# SIGIO; high priority input available; unix consensus
syscon	sicode	POLL_HUP				6			6			6			6			6			6			6			6			# SIGIO; device disconnected; unix consensus
syscon	sicode	SYS_SECCOMP				1			1			-1			-1			-1			-1			-1			-1			# SIGSYS; seccomp triggered
syscon	sicode	SYS_USER_DISPATCH			2			2			-1			-1			-1			-1			-1			-1			# SIGSYS; syscall user dispatch triggered

#	sigaltstack() values
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	ss	SS_ONSTACK				1			1			1			1			1			1			1			1			# unix consensus
syscon	ss	SS_DISABLE				2			2			4			4			4			4			4			2			# bsd consensus

#	close_range() values
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	close	CLOSE_RANGE_UNSHARE			2			2			-1			-1			-1			-1			-1			-1			#
syscon	close	CLOSE_RANGE_CLOEXEC			4			4			-1			-1			-1			-1			-1			-1			#

#	clock_{gettime,settime} timers
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	clock	CLOCK_REALTIME				0			0			0			0			0			0			0			0			# consensus
syscon	clock	CLOCK_REALTIME_PRECISE			0			0			0			0			9			0			0			0			#
syscon	clock	CLOCK_REALTIME_FAST			0			0			0			0			10			0			0			0			#
syscon	clock	CLOCK_REALTIME_COARSE			5			5			0			0			10			0			0			2			# Linux 2.6.32+; bsd consensus; not available on RHEL5
syscon	clock	CLOCK_MONOTONIC				1			1			6			6			4			3			3			1			# XNU/NT faked; could move backwards if NTP introduces negative leap second
syscon	clock	CLOCK_MONOTONIC_PRECISE			1			1			6			6			11			3			3			1			#
syscon	clock	CLOCK_MONOTONIC_FAST			1			1			6			6			12			3			3			1			#
syscon	clock	CLOCK_MONOTONIC_COARSE			6			6			5			5			12			3			3			1			# Linux 2.6.32+; bsd consensus; not available on RHEL5
syscon	clock	CLOCK_MONOTONIC_RAW			4			4			4			4			127			127			127			127			# actually monotonic; not subject to NTP adjustments; Linux 2.6.28+; XNU/NT/FreeBSD/OpenBSD faked; not available on RHEL5
syscon	clock	CLOCK_PROCESS_CPUTIME_ID		2			2			12			12			15			2			0x40000000		4			# NetBSD lets you bitwise a PID into clockid_t
syscon	clock	CLOCK_THREAD_CPUTIME_ID			3			3			16			16			14			4			0x20000000		5			#
syscon	clock	CLOCK_PROF				127			127			127			127			2			127			2			127			#
syscon	clock	CLOCK_BOOTTIME				7			7			7			127			127			6			127			3			#
syscon	clock	CLOCK_REALTIME_ALARM			8			8			127			127			127			127			127			127			#
syscon	clock	CLOCK_BOOTTIME_ALARM			9			9			127			127			127			127			127			127			#
syscon	clock	CLOCK_TAI				11			11			127			127			127			127			127			127			#
syscon	clock	CLOCK_UPTIME				127			127			8			8			5			5			127			127			#
syscon	clock	CLOCK_UPTIME_PRECISE			127			127			127			127			7			127			127			127			#
syscon	clock	CLOCK_UPTIME_FAST			127			127			127			127			8			127			127			127			#
syscon	clock	CLOCK_SECOND				127			127			127			127			13			127			127			127			#

#	poll()
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	poll	POLLIN					1			1			1			1			1			1			1			0x0300			# unix consensus; POLLRDNORM|POLLRDBAND on Windows
syscon	poll	POLLPRI					2			2			2			2			2			2			2			0x0400			# unix consensus
syscon	poll	POLLOUT					4			4			4			4			4			4			4			0x0010			# unix consensus; POLLWRNORM on Windows
syscon	poll	POLLERR					8			8			8			8			8			8			8			0x0001			# unix consensus
syscon	poll	POLLHUP					0x10			0x10			0x10			0x10			0x10			0x10			0x10			0x0002			# unix consensus
syscon	poll	POLLNVAL				0x20			0x20			0x20			0x20			0x20			0x20			0x20			0x0004			# unix consensus
syscon	poll	POLLRDBAND				0x80			0x80			0x80			0x80			0x80			0x80			0x80			0x0200			# unix consensus
syscon	poll	POLLRDNORM				0x40			0x40			0x40			0x40			0x40			0x40			0x40			0x0100			# unix consensus
syscon	poll	POLLWRBAND				0x0200			0x0200			0x0100			0x0100			0x0100			0x0100			0x0100			0x0020			# bsd consensus
syscon	poll	POLLWRNORM				0x0100			0x0100			4			4			4			4			4			0x0010			# bsd consensus
syscon	poll	POLLRDHUP				0x2000			0x2000			0x10			0x10			0x10			0x10			0x10			2			# bsd consensus (POLLHUP on non-Linux)

#	{set,get}sockopt(fd, level=SOL_SOCKET, X, ...)
#
#	Unsupported magic numbers are set to zero.
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	so	SOL_SOCKET				1			1			0xffff			0xffff			0xffff			0xffff			0xffff			0xffff			# yes it's actually 0xffff; bsd+nt consensus (todo: what's up with ipproto_icmp overlap)
syscon	so	SO_DEBUG				1			1			1			1			1			1			1			1			# debugging is enabled; consensus
syscon	so	SO_TYPE					3			3			0x1008			0x1008			0x1008			0x1008			0x1008			0x1008			# bsd consensus
syscon	so	SO_ERROR				4			4			0x1007			0x1007			0x1007			0x1007			0x1007			0x1007			# takes int pointer and stores/clears the pending error code; bsd consensus
syscon	so	SO_ACCEPTCONN				30			30			2			2			2			2			2			2			# takes int pointer and stores boolean indicating if listen() was called on fd; bsd consensus
syscon	so	SO_REUSEPORT				15			15			0x0200			0x0200			0x0200			0x0200			0x0200			0			# bsd consensus; no windows support
syscon	so	SO_REUSEADDR				2			2			4			4			4			4			4			-5			# SO_EXCLUSIVEADDRUSE on Windows (see third_party/python/Lib/test/support/__init__.py)
syscon	so	SO_KEEPALIVE				9			9			8			8			8			8			8			8			# bsd consensus
syscon	so	SO_DONTROUTE				5			5			0x10			0x10			0x10			0x10			0x10			0x10			# bsd consensus
syscon	so	SO_BROADCAST				6			6			0x20			0x20			0x20			0x20			0x20			0x20			# socket is configured for broadcast messages; bsd consensus
syscon	so	SO_USELOOPBACK				0			0			0x40			0x40			0x40			0x40			0x40			0x40			# bsd consensus
syscon	so	SO_LINGER				13			13			0x1080			0x1080			0x80			0x80			0x80			0x80			# takes struct linger; causes close() return value to actually mean something; SO_LINGER_SEC on XNU; bsd consensus
syscon	so	SO_OOBINLINE				10			10			0x0100			0x0100			0x0100			0x0100			0x0100			0x0100			# bsd consensus
syscon	so	SO_SNDBUF				7			7			0x1001			0x1001			0x1001			0x1001			0x1001			0x1001			# bsd consensus
syscon	so	SO_RCVBUF				8			8			0x1002			0x1002			0x1002			0x1002			0x1002			0x1002			# bsd consensus
syscon	so	SO_RCVTIMEO				20			20			0x1006			0x1006			0x1006			0x1006			0x100c			0x1006			# recv timeout; takes struct timeval (overrides SA_RESTART restoring EINTR behavior on recv/send/connect/accept/etc.; bsd consensus)
syscon	so	SO_SNDTIMEO				21			21			0x1005			0x1005			0x1005			0x1005			0x100b			0x1005			# send timeout; takes struct timeval; bsd consensus
syscon	so	SO_RCVLOWAT				18			18			0x1004			0x1004			0x1004			0x1004			0x1004			0x1004			# bsd consensus
syscon	so	SO_SNDLOWAT				19			19			0x1003			0x1003			0x1003			0x1003			0x1003			0x1003			# bsd consensus

#	{set,get}sockopt(fd, level=SOL_TCP, X, ...)
#	» most elite of all tuning groups
#
#	@see https://www.kernel.org/doc/Documentation/networking/ip-sysctl.txt
#	@see https://www.iana.org/assignments/tcp-parameters/tcp-parameters.txt
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	tcp	TCP_NODELAY				1			1			1			1			1			1			1			1			# strong consensus for disabling nagle's algorithm; so be sure to disable it by turning this on
syscon	tcp	TCP_CORK				3			3			4			4			4			16			4			0			# nagle's algorithm strikes again; TCP_NOPUSH on BSD; be sure to turn it off; protip: mmap+writev vs. write+sendfile; see also /proc/sys/net/ipv4/tcp_autocorking; netbsd is 4 but not implemented
syscon	tcp	TCP_MAXSEG				2			2			2			2			2			2			2			0			# reduces tcp segment size; see also tcp offloading
syscon	tcp	TCP_FASTOPEN				23			23			0x105			0x105			0x0401			0			0			15			# reduces roundtrips; for listener; Linux 3.7+ (c. 2012) / or is windows it 0x22? /proc/sys/net/ipv4/tcp_fastopen TODO(jart): MSG_FASTOPEN; XNU sources say 261 but not sure if that's true
syscon	tcp	TCP_FASTOPEN_CONNECT			30			30			0			0			0			0			0			0			# reduces roundtrips; for listener; Linux 3.7+ (c. 2012) / or is windows it 0x22? /proc/sys/net/ipv4/tcp_fastopen TODO(jart): MSG_FASTOPEN; XNU sources say 261 but not sure if that's true
syscon	tcp	TCP_KEEPIDLE				4			4			0			0			0x100			0			3			0			# start keepalives after this period
syscon	tcp	TCP_KEEPINTVL				5			5			0x101			0x101			0x200			0			5			0			# interval between keepalives
syscon	tcp	TCP_KEEPCNT				6			6			0x102			0x102			0x400			0			6			0			# number of keepalives before death
syscon	tcp	TCP_SYNCNT				7			7			0			0			0			0			0			0			# how hard to syn packet the enemy
syscon	tcp	TCP_ULP					31			31			0			0			0			0			0			0			# setsockopt(sock, IPPROTO_TCP, TCP_ULP, "tls", 4)
syscon	tcp	TCP_COOKIE_TRANSACTIONS			15			15			0			0			0			0			0			0			# defense against the syn packets
syscon	tcp	TCP_LINGER2				8			8			0			0			0			0			0			0			# orphaned fin-wait-2 lifetime cf. net.ipv4.tcp_fin_timeout see cloudflare blog
syscon	tcp	TCP_NOTSENT_LOWAT			25			25			513			513			0			0			0			0			# limit unset byte queue
syscon	tcp	TCP_INFO				11			11			0			0			0x20			0			9			0			# get connection info
syscon	tcp	TCP_CC_INFO				26			26			0			0			0			0			0			0			# get congestion control info
syscon	tcp	TCP_CONGESTION				13			13			0			0			0x40			0			0			0			# set traffic control
syscon	tcp	TCP_MD5SIG				14			14			0			0			0x10			4			16			0			# what is it (rfc2385)
syscon	tcp	TCP_MD5SIG_MAXKEYLEN			80			80			0			0			0			0			0			0			# what is it
syscon	tcp	TCP_TIMESTAMP				24			24			0			0			0			0			0			0			# what is it
syscon	tcp	TCP_USER_TIMEOUT			18			18			0			0			0			0			0			0			# what is it
syscon	tcp	TCP_QUICKACK				12			12			0			0			0			0			0			0			# disables optimization that lets kernel merge response data into ack packets
syscon	tcp	TCP_SAVE_SYN				27			27			0			0			0			0			0			0			# record syn packets
syscon	tcp	TCP_SAVED_SYN				28			28			0			0			0			0			0			0			# get recorded syn packets
syscon	tcp	TCP_THIN_DUPACK				17			17			0			0			0			0			0			0			# what is it
syscon	tcp	TCP_QUEUE_SEQ				21			21			0			0			0			0			0			0			# what is it
syscon	tcp	TCP_WINDOW_CLAMP			10			10			0			0			0			0			0			0			# what is it
syscon	tcp	TCP_DEFER_ACCEPT			9			9			0			0			0			0			0			0			# defer accept() until readable data has arrived
syscon	tcp	TCP_REPAIR				19			19			0			0			0			0			0			0			# what is it
syscon	tcp	TCP_REPAIR_OPTIONS			22			22			0			0			0			0			0			0			# what is it
syscon	tcp	TCP_REPAIR_QUEUE			20			20			0			0			0			0			0			0			# what is it
syscon	tcp	TCP_THIN_LINEAR_TIMEOUTS		16			16			0			0			0			0			0			0			# what is it

#	https://blog.cloudflare.com/know-your-scm_rights/
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	scm	SCM_RIGHTS				1			1			1			1			1			1			1			1			# unix consensus; faked nt
syscon	scm	SCM_TIMESTAMP				29			29			2			2			2			4			8			0
syscon	scm	SCM_CREDENTIALS				2			2			0			0			0			0			0			0
syscon	scm	SCM_TIMESTAMPING			37			37			0			0			0			0			0			0
syscon	scm	SCM_TIMESTAMPNS				35			35			0			0			0			0			0			0
syscon	scm	SCM_WIFI_STATUS				41			41			0			0			0			0			0			0

#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	ip	IP_TOS					1			1			3			3			3			3			3			3			# bsd consensus
syscon	ip	IP_TTL					2			2			4			4			4			4			4			4			# bsd consensus
syscon	ip	IP_HDRINCL				3			3			2			2			2			2			2			2			# bsd consensus
syscon	ip	IP_DEFAULT_MULTICAST_LOOP		1			1			1			1			1			1			1			1			# consensus
syscon	ip	IP_DEFAULT_MULTICAST_TTL		1			1			1			1			1			1			1			1			# consensus
syscon	ip	IP_PMTUDISC_DONT			0			0			0			0			0			0			0			0			# consensus
syscon	ip	IP_MAX_MEMBERSHIPS			20			20			0x0fff			0x0fff			0x0fff			0x0fff			0x0fff			20			# bsd consensus
syscon	ip	IP_OPTIONS				4			4			1			1			1			1			1			1			# bsd consensus
syscon	ip	IP_RECVTTL				12			12			24			24			65			31			23			21
syscon	ip	IP_ADD_MEMBERSHIP			35			35			12			12			12			12			12			12			# bsd consensus
syscon	ip	IP_DROP_MEMBERSHIP			36			36			13			13			13			13			13			13			# bsd consensus
syscon	ip	IP_MULTICAST_IF				0x20			0x20			9			9			9			9			9			9			# bsd consensus
syscon	ip	IP_MULTICAST_LOOP			34			34			11			11			11			11			11			11			# bsd consensus
syscon	ip	IP_MULTICAST_TTL			33			33			10			10			10			10			10			10			# bsd consensus
syscon	ip	IP_RECVOPTS				6			6			5			5			5			5			5			0			# bsd consensus
syscon	ip	IP_RECVRETOPTS				7			7			6			6			6			6			6			0			# bsd consensus
syscon	ip	IP_RECVDSTADDR				0			0			7			7			7			7			7			0			# bsd consensus
syscon	ip	IP_RETOPTS				7			7			8			8			8			8			8			0			# bsd consensus
syscon	ip	IP_ADD_SOURCE_MEMBERSHIP		39			39			70			70			70			0			0			15
syscon	ip	IP_BLOCK_SOURCE				38			38			72			72			72			0			0			17
syscon	ip	IP_DROP_SOURCE_MEMBERSHIP		40			40			71			71			71			0			0			16
syscon	ip	IP_UNBLOCK_SOURCE			37			37			73			73			73			0			0			18
syscon	ip	IP_IPSEC_POLICY				0x10			0x10			21			21			21			0			0			0
syscon	ip	IP_MINTTL				21			21			0			0			66			32			24			0			# minimum ttl for packet or drop
syscon	ip	IP_MSFILTER				41			41			74			74			74			0			0			0
syscon	ip	IP_PKTINFO				8			8			26			26			0			0			25			19
syscon	ip	IP_RECVTOS				13			13			0			0			68			0			0			40
syscon	ip	IP_MTU					14			14			0			0			0			0			0			73			# bsd consensus
syscon	ip	IP_MTU_DISCOVER				10			10			0			0			0			0			0			71			# bsd consensus
syscon	ip	IP_RECVERR				11			11			0			0			0			0			0			75			# bsd consensus
syscon	ip	IP_UNICAST_IF				50			50			0			0			0			0			0			31			# bsd consensus
syscon	ip	IP_ORIGDSTADDR				20			20			0			0			27			0			0			0
syscon	ip	IP_RECVORIGDSTADDR			20			20			0			0			27			0			0			0
syscon	ip	IP_BIND_ADDRESS_NO_PORT			24			24			0			0			0			0			0			0
syscon	ip	IP_CHECKSUM				23			23			0			0			0			0			0			0
syscon	ip	IP_FREEBIND				15			15			0			0			0			0			0			0
syscon	ip	IP_MULTICAST_ALL			49			49			0			0			0			0			0			0
syscon	ip	IP_NODEFRAG				22			22			0			0			0			0			0			0
syscon	ip	IP_PASSSEC				18			18			0			0			0			0			0			0
syscon	ip	IP_PKTOPTIONS				9			9			0			0			0			0			0			0
syscon	ip	IP_PMTUDISC				10			10			0			0			0			0			0			0
syscon	ip	IP_PMTUDISC_DO				2			2			0			0			0			0			0			0
syscon	ip	IP_PMTUDISC_INTERFACE			4			4			0			0			0			0			0			0
syscon	ip	IP_PMTUDISC_OMIT			5			5			0			0			0			0			0			0
syscon	ip	IP_PMTUDISC_PROBE			3			3			0			0			0			0			0			0
syscon	ip	IP_PMTUDISC_WANT			1			1			0			0			0			0			0			0
syscon	ip	IP_ROUTER_ALERT				5			5			0			0			0			0			0			0
syscon	ip	IP_TRANSPARENT				19			19			0			0			0			0			0			0
syscon	ip	IP_XFRM_POLICY				17			17			0			0			0			0			0			0
syscon	ip	INET_ADDRSTRLEN				0x10			0x10			0x10			0x10			0x10			0x10			0x10			22			# unix consensus

syscon	ipport	IPPORT_USERRESERVED			5000			5000			5000			5000			5000			49151			5000			5000

#	ptrace() codes
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	ptrace	PTRACE_TRACEME				0			0			0			0			0			0			-1			-1			# unix consensus a.k.a. PT_TRACE_ME
syscon	ptrace	PTRACE_PEEKTEXT				1			1			1			1			1			1			-1			-1			# unix consensus a.k.a. PT_READ_I
syscon	ptrace	PTRACE_PEEKDATA				2			2			2			2			2			2			-1			-1			# unix consensus a.k.a. PT_READ_D
syscon	ptrace	PTRACE_PEEKUSER				3			3			3			3			-1			-1			-1			-1			# a.k.a. PT_READ_U
syscon	ptrace	PTRACE_POKETEXT				4			4			4			4			4			4			-1			-1			# unix consensus a.k.a. PT_WRITE_I
syscon	ptrace	PTRACE_POKEDATA				5			5			5			5			5			5			-1			-1			# unix consensus a.k.a. PT_WRITE_D
syscon	ptrace	PTRACE_POKEUSER				6			6			6			6			-1			-1			-1			-1			# a.k.a. PT_WRITE_U
syscon	ptrace	PTRACE_CONT				7			7			7			7			7			7			-1			-1			# unix consensus a.k.a. PT_CONTINUE
syscon	ptrace	PTRACE_KILL				8			8			8			8			8			8			-1			-1			# unix consensus a.k.a. PT_KILL
syscon	ptrace	PTRACE_SINGLESTEP 			9			9			9			9			9			32			-1			-1			# a.k.a. PT_STEP
syscon	ptrace	PTRACE_GETREGS				12			12			-1			-1			33			33			-1			-1			# a.k.a. PT_GETREGS
syscon	ptrace	PTRACE_SETREGS				13			13			-1			-1			34			34			-1			-1			# a.k.a. PT_SETREGS
syscon	ptrace	PTRACE_GETFPREGS			14			14			-1			-1			35			35			-1			-1			# a.k.a. PT_GETFPREGS
syscon	ptrace	PTRACE_SETFPREGS			15			15			-1			-1			36			36			-1			-1			# a.k.a. PT_SETFPREGS
syscon	ptrace	PTRACE_ATTACH				16			16			10			10			10			9			-1			-1			# a.k.a. PT_ATTACH
syscon	ptrace	PTRACE_DETACH				17			17			11			11			11			10			-1			-1			# a.k.a. PT_DETACH
syscon	ptrace	PTRACE_GETFPXREGS			18			18			-1			-1			-1			-1			-1			-1			# a.k.a. PT_GETFPXREGS
syscon	ptrace	PTRACE_SETFPXREGS			19			19			-1			-1			-1			-1			-1			-1			# a.k.a. PT_SETFPXREGS
syscon	ptrace	PTRACE_SYSCALL				24			24			-1			-1			22			-1			-1			-1			# a.k.a. PT_SYSCALL
syscon	ptrace	PTRACE_GETEVENTMSG			0x4201			0x4201			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_GETSIGINFO			0x4202			0x4202			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_SETOPTIONS			0x4200			0x4200			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_SETSIGINFO			0x4203			0x4203			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_GETREGSET			0x4204			0x4204			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_GETSIGMASK			0x420a			0x420a			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_INTERRUPT			0x4207			0x4207			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_LISTEN				0x4208			0x4208			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_PEEKSIGINFO			0x4209			0x4209			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_SECCOMP_GET_FILTER		0x420c			0x420c			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_SECCOMP_GET_METADATA		0x420d			0x420d			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_SEIZE				0x4206			0x4206			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_SETREGSET			0x4205			0x4205			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_SETSIGMASK			0x420b			0x420b			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_O_TRACESYSGOOD			0x0001			0x0001			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_O_TRACEFORK			0x0002			0x0002			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_O_TRACEVFORK			0x0004			0x0004			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_O_TRACECLONE			0x0008			0x0008			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_O_TRACEEXEC			0x0010			0x0010			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_O_TRACEVFORKDONE			0x0020			0x0020			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_O_TRACEEXIT			0x0040			0x0040			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_O_TRACESECCOMP			0x0080			0x0080			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_O_MASK				0x007f			0x007f			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_EVENT_FORK			1			1			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_EVENT_VFORK			2			2			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_EVENT_CLONE			3			3			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_EVENT_EXEC			4			4			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_EVENT_VFORK_DONE			5			5			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_EVENT_EXIT			6			6			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_EVENT_SECCOMP			7			7			-1			-1			-1			-1			-1			-1
syscon	ptrace	PTRACE_EVENT_STOP			128			128			-1			-1			-1			-1			-1			-1

#	clone() codes
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	clone	CLONE_VM				0x00000100		0x00000100		0x00000100		0x00000100		0x00000100		0x00000100		0x00000100		0x00000100		# intentionally symbolic so we can tell if clone() is being used to create threads

#	ioctl() requests
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	ioctl	FIONBIO					0x5421			0x5421			0x8004667e		0x8004667e		0x8004667e		0x8004667e		0x8004667e		0x8004667e		# BSD-The New Technology consensus; FIONBIO is traditional O_NONBLOCK; see F_SETFL for re-imagined api
syscon	ioctl	FIOASYNC				0x5452			0x5452			0x8004667d		0x8004667d		0x8004667d		0x8004667d		0x8004667d		0x8004667d		# BSD-The New Technology consensus
syscon	ioctl	FIONREAD				0x541b			0x541b			0x4004667f		0x4004667f		0x4004667f		0x4004667f		0x4004667f		0x4004667f		# BSD-The New Technology consensus; bytes waiting in FD's input buffer
syscon	ioctl	FIOCLEX					0x5451			0x5451			0x20006601		0x20006601		0x20006601		0x20006601		0x20006601		0x5451			# sets "close on exec" on file descriptor the fast way; faked nt
syscon	ioctl	FIONCLEX				0x5450			0x5450			0x20006602		0x20006602		0x20006602		0x20006602		0x20006602		0x5450			# clears "close on exec" on file descriptor the fast way; faked nt
#syscon	ioctl	FIONWRITE				0x0			0x0			0x0			0x0			0x40046677		0x0			0x0			-1			# [FreeBSD Generalization] bytes queued in FD's output buffer (same as TIOCOUTQ for TTY FDs; see also SO_SNDBUF)
#syscon	ioctl	FIONSPACE				0x0			0x0			0x0			0x0			0x40046676		0x0			0x0			-1			# [FreeBSD Generalization] capacity of FD's output buffer, e.g. equivalent to TIOCGSERIAL w/ UART
syscon	ioctl	SIOCGIFCONF				0x8912			0x8912			0xc00c6924		0xc00c6924		0xc0106924		0xc0106924		0xc0106924		0x8912
syscon	ioctl	SIOCADDMULTI				0x8931			0x8931			0x80206931		0x80206931		0x80206931		0x80206931		0x80206931		0x8931			# bsd consensus
syscon	ioctl	SIOCDELMULTI				0x8932			0x8932			0x80206932		0x80206932		0x80206932		0x80206932		0x80206932		0x8932			# bsd consensus
syscon	ioctl	SIOCDIFADDR				0x8936			0x8936			0x80206919		0x80206919		0x80206919		0x80206919		0x80206919		0x8936			# bsd consensus
syscon	ioctl	SIOCGIFADDR				0x8915			0x8915			0xc0206921		0xc0206921		0xc0206921		0xc0206921		0xc0206921		0x8915			# bsd consensus
syscon	ioctl	SIOCGIFBRDADDR				0x8919			0x8919			0xc0206923		0xc0206923		0xc0206923		0xc0206923		0xc0206923		0x8919			# bsd consensus
syscon	ioctl	SIOCGIFDSTADDR				0x8917			0x8917			0xc0206922		0xc0206922		0xc0206922		0xc0206922		0xc0206922		0x8917			# bsd consensus
syscon	ioctl	SIOCGIFFLAGS				0x8913			0x8913			0xc0206911		0xc0206911		0xc0206911		0xc0206911		0xc0206911		0x8913			# bsd consensus
syscon	ioctl	SIOCGIFMETRIC				0x891d			0x891d			0xc0206917		0xc0206917		0xc0206917		0xc0206917		0xc0206917		0x891d			# bsd consensus
syscon	ioctl	SIOCGIFNETMASK				0x891b			0x891b			0xc0206925		0xc0206925		0xc0206925		0xc0206925		0xc0206925		0x891b			# bsd consensus
syscon	ioctl	SIOCGPGRP				0x8904			0x8904			0x40047309		0x40047309		0x40047309		0x40047309		0x40047309		0x8904			# bsd consensus
syscon	ioctl	SIOCSIFADDR				0x8916			0x8916			0x8020690c		0x8020690c		0x8020690c		0x8020690c		0x8020690c		0x8916			# bsd consensus
syscon	ioctl	SIOCSIFBRDADDR				0x891a			0x891a			0x80206913		0x80206913		0x80206913		0x80206913		0x80206913		0x891a			# bsd consensus
syscon	ioctl	SIOCSIFDSTADDR				0x8918			0x8918			0x8020690e		0x8020690e		0x8020690e		0x8020690e		0x8020690e		0x8918			# bsd consensus
syscon	ioctl	SIOCSIFFLAGS				0x8914			0x8914			0x80206910		0x80206910		0x80206910		0x80206910		0x80206910		0x8914			# bsd consensus
syscon	ioctl	SIOCSIFMETRIC				0x891e			0x891e			0x80206918		0x80206918		0x80206918		0x80206918		0x80206918		0x891e			# bsd consensus
syscon	ioctl	SIOCSIFNETMASK				0x891c			0x891c			0x80206916		0x80206916		0x80206916		0x80206916		0x80206916		0x891c			# bsd consensus
syscon	ioctl	SIOCSPGRP				0x8902			0x8902			0x80047308		0x80047308		0x80047308		0x80047308		0x80047308		0x8902			# bsd consensus
syscon	ioctl	SIOCGIFMTU				0x8921			0x8921			0xc0206933		0xc0206933		0xc0206933		0xc020697e		0xc020697e		0x8921
syscon	ioctl	SIOCSIFMTU				0x8922			0x8922			0x80206934		0x80206934		0x80206934		0x8020697f		0x8020697f		0x8922

syscon	ioctl	SIOCGIFINDEX				0x8933			0x8933			0			0			0xc0206920		0			0			0
syscon	ioctl	SIOCSIFNAME				0x8923			0x8923			0			0			0x80206928		0			0			0
syscon	ioctl	SIOCADDDLCI				0x8980			0x8980			0			0			0			0			0			0
syscon	ioctl	SIOCADDRT				0x890b			0x890b			0			0			0			0			0			0
syscon	ioctl	SIOCDARP				0x8953			0x8953			0			0			0			0			0			0
syscon	ioctl	SIOCDELDLCI				0x8981			0x8981			0			0			0			0			0			0
syscon	ioctl	SIOCDELRT				0x890c			0x890c			0			0			0			0			0			0
syscon	ioctl	SIOCDEVPRIVATE				0x89f0			0x89f0			0			0			0			0			0			0
syscon	ioctl	SIOCDRARP				0x8960			0x8960			0			0			0			0			0			0
syscon	ioctl	SIOCGARP				0x8954			0x8954			0			0			0			0			0			0
syscon	ioctl	SIOCGIFBR				0x8940			0x8940			0			0			0			0			0			0
syscon	ioctl	SIOCGIFCOUNT				0x8938			0x8938			0			0			0			0			0			0
syscon	ioctl	SIOCGIFENCAP				0x8925			0x8925			0			0			0			0			0			0
syscon	ioctl	SIOCGIFHWADDR				0x8927			0x8927			0			0			0			0			0			0
syscon	ioctl	SIOCGIFMAP				0x8970			0x8970			0			0			0			0			0			0
syscon	ioctl	SIOCGIFMEM				0x891f			0x891f			0			0			0			0			0			0
syscon	ioctl	SIOCGIFNAME				0x8910			0x8910			0			0			0			0			0			0
syscon	ioctl	SIOCGIFPFLAGS				0x8935			0x8935			0			0			0			0			0			0
syscon	ioctl	SIOCGIFSLAVE				0x8929			0x8929			0			0			0			0			0			0
syscon	ioctl	SIOCGIFTXQLEN				0x8942			0x8942			0			0			0			0			0			0
syscon	ioctl	SIOCGRARP				0x8961			0x8961			0			0			0			0			0			0
syscon	ioctl	SIOCGSTAMP				0x8906			0x8906			0			0			0			0			0			0
syscon	ioctl	SIOCGSTAMPNS				0x8907			0x8907			0			0			0			0			0			0
syscon	ioctl	SIOCPROTOPRIVATE			0x89e0			0x89e0			0			0			0			0			0			0
syscon	ioctl	SIOCRTMSG				0x890d			0x890d			0			0			0			0			0			0
syscon	ioctl	SIOCSARP				0x8955			0x8955			0			0			0			0			0			0
syscon	ioctl	SIOCSIFBR				0x8941			0x8941			0			0			0			0			0			0
syscon	ioctl	SIOCSIFENCAP				0x8926			0x8926			0			0			0			0			0			0
syscon	ioctl	SIOCSIFHWADDR				0x8924			0x8924			0			0			0			0			0			0
syscon	ioctl	SIOCSIFHWBROADCAST			0x8937			0x8937			0			0			0			0			0			0
syscon	ioctl	SIOCSIFLINK				0x8911			0x8911			0			0			0			0			0			0
syscon	ioctl	SIOCSIFMAP				0x8971			0x8971			0			0			0			0			0			0
syscon	ioctl	SIOCSIFMEM				0x8920			0x8920			0			0			0			0			0			0
syscon	ioctl	SIOCSIFPFLAGS				0x8934			0x8934			0			0			0			0			0			0
syscon	ioctl	SIOCSIFSLAVE				0x8930			0x8930			0			0			0			0			0			0
syscon	ioctl	SIOCSIFTXQLEN				0x8943			0x8943			0			0			0			0			0			0
syscon	ioctl	SIOCSRARP				0x8962			0x8962			0			0			0			0			0			0
syscon	ioctl	SIOGIFINDEX				0x8933			0x8933			0			0			0			0			0			0

#	socket() address families
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	af	AF_UNSPEC				0			0			0			0			0			0			0			0			# consensus
syscon	af	AF_UNIX					1			1			1			1			1			1			1			1			# consensus
syscon	af	AF_LOCAL				1			1			1			1			1			1			1			1			# consensus
syscon	af	AF_INET					2			2			2			2			2			2			2			2			# consensus
syscon	af	AF_INET6				10			10			30			30			28			24			24			23
syscon	af	AF_IPX					4			4			23			23			23			23			23			6			# bsd consensus
syscon	af	AF_APPLETALK				5			5			0x10			0x10			0x10			0x10			0x10			0x10			# bsd consensus
syscon	af	AF_ROUTE				16			16			17			17			17			17			34			-1			# bsd consensus
syscon	af	AF_LINK					-1			-1			18			18			18			18			18			-1
syscon	af	AF_SNA					22			22			11			11			11			11			11			11			# bsd consensus
syscon	af	AF_FILE					1			1			-1			-1			-1			-1			-1			-1
syscon	af	AF_AX25					3			3			-1			-1			-1			-1			-1			-1
syscon	af	AF_NETROM				6			6			-1			-1			-1			-1			-1			-1
syscon	af	AF_BRIDGE				7			7			-1			-1			-1			-1			-1			-1
syscon	af	AF_ATMPVC				8			8			-1			-1			-1			-1			-1			-1
syscon	af	AF_X25					9			9			-1			-1			-1			-1			-1			-1
syscon	af	AF_ROSE					11			11			-1			-1			-1			-1			-1			-1
syscon	af	AF_NETBEUI				13			13			-1			-1			-1			-1			-1			-1
syscon	af	AF_SECURITY				14			14			-1			-1			-1			-1			-1			-1
syscon	af	AF_KEY					15			15			-1			-1			-1			30			-1			-1
syscon	af	AF_NETLINK				16			16			-1			-1			-1			-1			-1			-1
syscon	af	AF_PACKET				17			17			-1			-1			-1			-1			-1			-1
syscon	af	AF_ASH					18			18			-1			-1			-1			-1			-1			-1
syscon	af	AF_ECONET				19			19			-1			-1			-1			-1			-1			-1
syscon	af	AF_ATMSVC				20			20			-1			-1			-1			-1			-1			-1
syscon	af	AF_RDS					21			21			-1			-1			-1			-1			-1			-1
syscon	af	AF_IRDA					23			23			-1			-1			-1			-1			-1			-1
syscon	af	AF_PPPOX				24			24			-1			-1			-1			-1			-1			-1
syscon	af	AF_WANPIPE				25			25			-1			-1			-1			-1			-1			-1
syscon	af	AF_LLC					26			26			-1			-1			-1			-1			-1			-1
syscon	af	AF_IB					27			27			-1			-1			-1			-1			-1			-1
syscon	af	AF_MPLS					28			28			-1			-1			-1			33			33			-1
syscon	af	AF_CAN					29			29			-1			-1			-1			-1			35			-1
syscon	af	AF_TIPC					30			30			-1			-1			-1			-1			-1			-1
syscon	af	AF_BLUETOOTH				31			31			-1			-1			36			0x20			31			-1
syscon	af	AF_IUCV					0x20			0x20			-1			-1			-1			-1			-1			-1
syscon	af	AF_RXRPC				33			33			-1			-1			-1			-1			-1			-1
syscon	af	AF_ISDN					34			34			28			28			26			26			26			-1
syscon	af	AF_PHONET				35			35			-1			-1			-1			-1			-1			-1
syscon	af	AF_IEEE802154				36			36			-1			-1			-1			-1			-1			-1
syscon	af	AF_CAIF					37			37			-1			-1			-1			-1			-1			-1
syscon	af	AF_ALG					38			38			-1			-1			-1			-1			-1			-1
syscon	af	AF_NFC					39			39			-1			-1			-1			-1			-1			-1
syscon	af	AF_VSOCK				40			40			-1			-1			-1			-1			-1			-1
syscon	af	AF_KCM					41			41			-1			-1			-1			-1			-1			-1
syscon	af	AF_MAX					42			42			40			40			42			36			37			35

syscon	pf	PF_UNIX					1			1			1			1			1			1			1			1			# consensus
syscon	pf	PF_UNSPEC				0			0			0			0			0			0			0			0			# consensus
syscon	pf	PF_APPLETALK				5			5			0x10			0x10			0x10			0x10			0x10			0x10			# bsd consensus
syscon	pf	PF_SNA					22			22			11			11			11			11			11			11			# bsd consensus
syscon	pf	PF_INET6				10			10			30			30			28			24			24			23
syscon	pf	PF_MAX					42			42			40			40			42			36			36			35
syscon	pf	PF_INET					2			2			2			2			2			2			2			0			# unix consensus
syscon	pf	PF_LOCAL				1			1			1			1			1			1			1			0			# unix consensus
syscon	pf	PF_IPX					4			4			23			23			23			23			23			0			# bsd consensus
syscon	pf	PF_ROUTE				0x10			0x10			17			17			17			17			17			0			# bsd consensus
syscon	pf	PF_ISDN					34			34			28			28			26			26			26			0
syscon	pf	PF_KEY					15			15			29			29			27			30			30			0
syscon	pf	PF_BLUETOOTH				31			31			0			0			36			0x20			0x20			0
syscon	pf	PF_MPLS					28			28			0			0			0			33			33			0
syscon	pf	PF_ALG					38			38			0			0			0			0			0			0
syscon	pf	PF_ASH					18			18			0			0			0			0			0			0
syscon	pf	PF_ATMPVC				8			8			0			0			0			0			0			0
syscon	pf	PF_ATMSVC				20			20			0			0			0			0			0			0
syscon	pf	PF_AX25					3			3			0			0			0			0			0			0
syscon	pf	PF_BRIDGE				7			7			0			0			0			0			0			0
syscon	pf	PF_CAIF					37			37			0			0			0			0			0			0
syscon	af	PF_CAN					29			29			0			0			0			0			35			0
syscon	pf	PF_ECONET				19			19			0			0			0			0			0			0
syscon	pf	PF_FILE					1			1			0			0			0			0			0			0
syscon	pf	PF_IB					27			27			0			0			0			0			0			0
syscon	pf	PF_IEEE802154				36			36			0			0			0			0			0			0
syscon	pf	PF_IRDA					23			23			0			0			0			0			0			0
syscon	pf	PF_IUCV					0x20			0x20			0			0			0			0			0			0
syscon	pf	PF_KCM					41			41			0			0			0			0			0			0
syscon	pf	PF_LLC					26			26			0			0			0			0			0			0
syscon	pf	PF_NETBEUI				13			13			0			0			0			0			0			0
syscon	pf	PF_NETLINK				0x10			0x10			0			0			0			0			0			0
syscon	pf	PF_NETROM				6			6			0			0			0			0			0			0
syscon	pf	PF_NFC					39			39			0			0			0			0			0			0
syscon	pf	PF_PACKET				17			17			0			0			0			0			0			0
syscon	pf	PF_PHONET				35			35			0			0			0			0			0			0
syscon	pf	PF_PPPOX				24			24			0			0			0			0			0			0
syscon	pf	PF_RDS					21			21			0			0			0			0			0			0
syscon	pf	PF_ROSE					11			11			0			0			0			0			0			0
syscon	pf	PF_RXRPC				33			33			0			0			0			0			0			0
syscon	pf	PF_SECURITY				14			14			0			0			0			0			0			0
syscon	pf	PF_TIPC					30			30			0			0			0			0			0			0
syscon	pf	PF_VSOCK				40			40			0			0			0			0			0			0
syscon	pf	PF_WANPIPE				25			25			0			0			0			0			0			0
syscon	pf	PF_X25					9			9			0			0			0			0			0			0

#	msync() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	ms	MS_SYNC					4			4			16			16			1			2			4			4			# faked nt; actually 0 on freebsd
syscon	ms	MS_ASYNC				1			1			1			1			2			1			1			1			# faked nt; actually 1 on freebsd
syscon	ms	MS_INVALIDATE				2			2			2			2			4			4			2			0			# actually 2 on freebsd

#	statfs() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	statfs	ST_RDONLY				1			1			1			1			1			1			1			0x00080000		# MNT_RDONLY on BSD, kNtFileReadOnlyVolume on NT
syscon	statfs	ST_NOSUID				2			2			8			8			8			8			8			0			# MNT_NOSUID on BSD
syscon	statfs	ST_NODEV				4			4			16			16			0			16			16			0			# MNT_NODEV on BSD
syscon	statfs	ST_NOEXEC				8			8			4			4			4			4			4			0			# MNT_NOEXEC on BSD
syscon	statfs	ST_SYNCHRONOUS				16			16			2			2			2			2			2			0			# MNT_SYNCHRONOUS on BSD
syscon	statfs	ST_NOATIME				0x0040			0x0040			0x10000000		0x10000000		0x10000000		0x00008000		0x04000000		0			# MNT_NOATIME on BSD
syscon	statfs	ST_RELATIME				0x1000			0x1000			0			0			0			0			0x00020000		0			# MNT_RELATIME on NetBSD
syscon	statfs	ST_APPEND				0x0100			0x0100			0			0			0			0			0			0			#
syscon	statfs	ST_IMMUTABLE				0x0200			0x0200			0			0			0			0			0			0			#
syscon	statfs	ST_MANDLOCK				0x0040			0x0040			0			0			0			0			0			0			#
syscon	statfs	ST_NODIRATIME				0x0800			0x0800			0			0			0			0			0			0			#
syscon	statfs	ST_WRITE				0x0080			0x0080			0			0			0			0			0			0			#

#	sendfile() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	sf	SF_NODISKIO				0			0			0			0			1			0			0			0
syscon	sf	SF_MNOWAIT				0			0			0			0			2			0			0			0
syscon	sf	SF_SYNC					0			0			0			0			4			0			0			0

#	mount() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	mount	MS_RDONLY				0x00000001		0x00000001		0x00000001		0x00000001		0x00000001		0x00000001		0x00000001		0x00000001		# consensus; MNT_RDONLY on bsd; faked nt
syscon	mount	MNT_RDONLY				0x00000001		0x00000001		0x00000001		0x00000001		0x00000001		0x00000001		0x00000001		0x00000001		# consensus; MS_RDONLY on linux; faked nt
syscon	mount	MS_NOSUID				0x00000002		0x00000002		0x00000008		0x00000008		0x00000008		0x00000008		0x00000008		0x00000001		# don't honor S_ISUID bit; bsd consensus; MNT_NOSUID on bsd; faked nt
syscon	mount	MNT_NOSUID				0			0			0x00000008		0x00000008		0x00000008		0x00000008		0x00000008		0x00000001		# don't honor S_ISUID bit; bsd consensus; appears incorrectly defined in linux headers; MS_NOSUID on linux; faked nt
syscon	mount	MS_NODEV				0x00000004		0x00000004		0x00000010		0x00000010		0x00000010		0x00000010		0x00000010		0x00000004		# disallow special files; bsd consensus; MNT_NODEV on bsd; faked nt
syscon	mount	MNT_NODEV				0x00000004		0x00000004		0x00000010		0x00000010		0x00000010		0x00000010		0x00000010		0x00000004		# disallow special files; bsd consensus; MS_NODEV on linux; faked nt
syscon	mount	MS_NOEXEC				0x00000008		0x00000008		0x00000004		0x00000004		0x00000004		0x00000004		0x00000004		0x00000008		# disallow program execution; bsd consensus; MNT_NOEXEC on bsd; faked nt
syscon	mount	MNT_NOEXEC				0x00000008		0x00000008		0x00000004		0x00000004		0x00000004		0x00000004		0x00000004		0x00000008		# disallow program execution; bsd consensus; MS_NOEXEC on linux; faked nt
syscon	mount	MS_SYNCHRONOUS				0x00000010		0x00000010		0x00000002		0x00000002		0x00000002		0x00000002		0x00000002		0x00000010		# bsd consensus; MNT_SYNCHRONOUS on bsd; faked nt
syscon	mount	MNT_SYNCHRONOUS				0x00000010		0x00000010		0x00000002		0x00000002		0x00000002		0x00000002		0x00000002		0x00000010		# bsd consensus; MS_SYNCHRONOUS on linux; faked nt
syscon	mount	MS_REMOUNT				0x00000020		0x00000020		0x00010000		0x00010000		0x00010000		0x00010000		0x00010000		0x00000020		# tune existing mounting; bsd consensus; MNT_UPDATE on bsd; faked nt
syscon	mount	MNT_UPDATE				0x00000020		0x00000020		0x00010000		0x00010000		0x00010000		0x00010000		0x00010000		0x00000020		# tune existing mounting; bsd consensus; MS_REMOUNT on linux; faked nt
syscon	mount	MS_MANDLOCK				0x00000040		0x00000040		0			0			0			0			0			0			#
syscon	mount	MS_DIRSYNC				0x00000080		0x00000080		0			0			0			0			0			0			#
syscon	mount	MS_NOATIME				0x00000400		0x00000400		0x10000000		0x10000000		0x10000000		0x00008000		0x04000000		0x00000400		# do not update access times; MNT_NOATIME on bsd
syscon	mount	MNT_NOATIME				0x00000400		0x00000400		0x10000000		0x10000000		0x10000000		0x00008000		0x04000000		0x00000400		# do not update access times; MS_NOATIME on linux
syscon	mount	MS_NODIRATIME				0x00000800		0x00000800		0			0			0			0			0			0			#
syscon	mount	MS_BIND					0x00001000		0x00001000		0			0			0			0			0			0			#
syscon	mount	MS_MOVE					0x00002000		0x00002000		0			0			0			0			0			0			#
syscon	mount	MS_REC					0x00004000		0x00004000		0			0			0			0			0			0			#
syscon	mount	MS_SILENT				0x00008000		0x00008000		0			0			0			0			0			0			#
syscon	mount	MS_POSIXACL				0x00010000		0x00010000		0			0			0			0			0			0			#
syscon	mount	MS_UNBINDABLE				0x00020000		0x00020000		0			0			0			0			0			0			#
syscon	mount	MS_PRIVATE				0x00040000		0x00040000		0			0			0			0			0			0			#
syscon	mount	MS_SLAVE				0x00080000		0x00080000		0			0			0			0			0			0			#
syscon	mount	MS_SHARED				0x00100000		0x00100000		0			0			0			0			0			0			#
syscon	mount	MS_RELATIME				0x00200000		0x00200000		0			0			0			0			0x00020000		0			# MNT_RELATIME on bsd
syscon	mount	MNT_RELATIME				0x00200000		0x00200000		0			0			0			0			0x00020000		0			# MS_RELATIME on linux
syscon	mount	MS_KERNMOUNT				0x00400000		0x00400000		0			0			0			0			0			0			#
syscon	mount	MS_I_VERSION				0x00800000		0x00800000		0			0			0			0			0			0			#
syscon	mount	MS_STRICTATIME				0x01000000		0x01000000		0x80000000		0x80000000		0			0			0			0			# enable strict update of file access time; MNT_STRICTATIME on bsd
syscon	mount	MNT_STRICTATIME				0x01000000		0x01000000		0x80000000		0x80000000		0			0			0			0			# enable strict update of file access time; MS_STRICTATIME on linux
syscon	mount	MS_LAZYTIME				0x02000000		0x02000000		0			0			0			0			0			0			#
syscon	mount	MS_ACTIVE				0x40000000		0x40000000		0			0			0			0			0			0			#
syscon	mount	MS_NOUSER				0x80000000		0x80000000		0			0			0			0			0			0			#
syscon	mount	MS_RMT_MASK				0x02800051		0x02800051		0			0			0			0			0			0			#
syscon	mount	MS_MGC_VAL				0xc0ed0000		0xc0ed0000		0			0			0			0			0			0			# Linux 2.3
syscon	mount	MS_MGC_MSK				0xffff0000		0xffff0000		0			0			0			0			0			0			# Linux 2.3
syscon	mount	MNT_ASYNC				0			0			0x00000040		0x00000040		0x00000040		0x00000040		0x00000040		0			# file system written asynchronously; bsd consensus
syscon	mount	MNT_RELOAD				0			0			0x00040000		0x00040000		0x00040000		0x00040000		0x00040000		0			# reload filesystem data; bsd consensus
syscon	mount	MNT_SUIDDIR				0			0			0			0			0x00100000		0			0			0			# special suid dir handling
syscon	mount	MNT_NOCLUSTERR				0			0			0			0			0x40000000		0			0			0			# disable cluster read
syscon	mount	MNT_NOCLUSTERW				0			0			0			0			0x80000000		0			0			0			# disable cluster write
syscon	mount	MNT_SNAPSHOT				0			0			0x40000000		0x40000000		0x01000000		0			0			0			# confusing

#	limits
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	limits	PIPE_BUF				4096			4096			512			512			512			512			512			4096			# bsd consensus
syscon	limits	NGROUPS_MAX				65536			65536			16			16			1023			16			16			0			#
syscon	limits	LINK_MAX				127			127			32767			32767			32767			32767			32767			64			# freebsd/windows are educated guesses
syscon	limits	MAX_CANON				255			255			1024			1024			255			255			255			255			# windows is guessed
syscon	limits	MAX_INPUT				255			255			1024			1024			255			255			255			255			# windows is guessed
syscon	limits	SOMAXCONN				4096			4096			128			128			128			128			128			2147483647		# maximum backlog for listen()
syscon	limits	_ARG_MAX				128*1024		128*1024		1024*1024		1024*1024		512*1024		512*1024		256*1024		32767*2			# bsd consensus
syscon	limits	_NAME_MAX				255			255			255			255			255			255			511			255			# probably higher on windows?
syscon	limits	_PATH_MAX				4096			4096			1024			1024			1024			1024			1024			260			#
syscon	limits	_NSIG					64			64			32			32			128			32			64			64			# _SIG_MAXSIG on FreeBSD
syscon	limits	_MINSIGSTKSZ				2048			2048			32768			32768			4096			12288			8192			2048			#
syscon	limits	_SIGSTKSZ				8192			2048			131072			131072			36864			28672			40960			8192			#

#	unmount() flags
#	a.k.a. umount2() on linux
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	unmount	MNT_FORCE				1			1			0x00080000		0x00080000		0x00080000		0x00080000		0x00080000		2			# force unmount or readonly
syscon	unmount	MNT_DETACH				2			2			0			0			0			0			0			0			# just detach from the tree
syscon	unmount	MNT_EXPIRE				4			4			0			0			0			0			0			0			# mark for expiry
syscon	unmount	UMOUNT_NOFOLLOW				8			8			0			0			0			0			0			0			# don't follow symlinks on unmount
syscon	unmount	MNT_BYFSID				0			0			0			0			0x08000000		0			0			0			# if used pass "FSID:val0:val1", where val0 and val1 are the contents of the fsid_t val[] array in decimal

#	reboot() magnums
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	reboot	RB_AUTOBOOT				0x01234567		0x01234567		0			0			0			0			0			4			# reboots; SHUTDOWN_RESTART on NT
syscon	reboot	RB_POWER_OFF				0x4321fedc		0x4321fedc		0xffffffff		0xffffffff		0x4000			0x1000			0x808			8			# SHUTDOWN_POWEROFF on NT
syscon	reboot	RB_POWERDOWN				0x4321fedc		0x4321fedc		0xffffffff		0xffffffff		0x4000			0x1000			0x808			8			# openbsd/netbsd name
syscon	reboot	RB_POWEROFF				0x4321fedc		0x4321fedc		0xffffffff		0xffffffff		0x4000			0x1000			0x808			8			# freebsd name
syscon	reboot	RB_HALT_SYSTEM				0xcdef0123		0xcdef0123		8			8			8			8			8			16			# the processor is simply halted; SHUTDOWN_NOREBOOT on NT
syscon	reboot	RB_HALT					0xcdef0123		0xcdef0123		8			8			8			8			8			16			# the processor is simply halted; bsd name
syscon	reboot	RB_SW_SUSPEND				0xd000fce2		0xd000fce2		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xd000fce2		#
syscon	reboot	RB_KEXEC				0x45584543		0x45584543		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		#
syscon	reboot	RB_ENABLE_CAD				0x89abcdef		0x89abcdef		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		# enable control-alt-delete reboot
syscon	reboot	RB_DISABLE_CAD				0			0			0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		0xffffffff		# make control-alt-delete just eintr
syscon	reboot	RB_NOSYNC				0x20000000		0x20000000		4			4			4			4			4			0x20000000		# prevents implicit sync() beforehand; polyfilled linux; polyfilled on nt just in case

#	send() / recv() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	msg	MSG_OOB					1			1			1			1			1			1			1			1			# consensus
syscon	msg	MSG_PEEK				2			2			2			2			2			2			2			2			# consensus
syscon	msg	MSG_DONTROUTE				4			4			4			4			4			4			4			4			# consensus
syscon	msg	MSG_FASTOPEN				0x20000000		0x20000000		0			0			0			0			0			0			# TODO
syscon	msg	MSG_WAITALL				0x0100			0x0100			0x40			0x40			0x40			0x40			0x40			8			# bsd consensus
syscon	msg	MSG_MORE				0x8000			0x8000			0			0			0			0			0			0			# send/sendto: manual TCP_CORK hbasically
syscon	msg	MSG_NOSIGNAL				0x4000			0x4000			0x80000			0x80000			0x020000		0x0400			0x0400			0			# send/sendto: don't SIGPIPE on EOF
syscon	msg	MSG_DONTWAIT				0x40			0x40			0x80			0x80			0x80			0x80			0x80			0x40			# send/sendto: manual non-blocking
syscon	msg	MSG_TRUNC				0x20			0x20			0x10			0x10			0x10			0x10			0x10			0x0100			# bsd consensus
syscon	msg	MSG_CTRUNC				8			8			0x20			0x20			0x20			0x20			0x20			0x0200			# bsd consensus
syscon	msg	MSG_ERRQUEUE				0x2000			0x2000			0			0			0			0			0			0x1000			# bsd consensus
syscon	msg	MSG_NOERROR				0x1000			0x1000			0x1000			0x1000			0x1000			0x1000			0x1000			0			# unix consensus
syscon	msg	MSG_EOR					0x80			0x80			8			8			8			8			8			0			# bsd consensus
syscon	msg	MSG_CMSG_CLOEXEC			0x40000000		0x40000000		0			0			0x040000		0x0800			0x0800			0
syscon	msg	MSG_WAITFORONE				0x010000		0x010000		0			0			0x080000		0			0x2000			0
syscon	msg	MSG_BATCH				0x040000		0x040000		0			0			0			0			0			0
syscon	msg	MSG_CONFIRM				0x0800			0x0800			0			0			0			0			0			0
syscon	msg	MSG_EXCEPT				0x2000			0x2000			0			0			0			0			0			0
syscon	msg	MSG_FIN					0x0200			0x0200			0x0100			0x0100			0x0100			0			0			0
syscon	msg	MSG_EOF					0x0200			0x0200			0x0100			0x0100			0x0100			0			0			0
syscon	msg	MSG_INFO				12			12			0			0			0			0			0			0
syscon	msg	MSG_PARITY_ERROR			9			9			0			0			0			0			0			0
syscon	msg	MSG_PROXY				0x10			0x10			0			0			0			0			0			0
syscon	msg	MSG_RST					0x1000			0x1000			0			0			0			0			0			0
syscon	msg	MSG_STAT				11			11			0			0			0			0			0			0
syscon	msg	MSG_SYN					0x0400			0x0400			0			0			0			0			0			0
syscon	msg	MSG_BCAST				0			0			0			0			0			0x100			0x100			0
syscon	msg	MSG_MCAST				0			0			0			0			0			0x200			0x200			0
syscon	msg	MSG_NOTIFICATION			0x8000			0x8000			0			0			0x2000			0			0x4000			0

#	getpriority() / setpriority() magnums (a.k.a. nice)
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	prio	PRIO_PROCESS				0			0			0			0			0			0			0			0			# consensus / poly nt
syscon	prio	PRIO_PGRP				1			1			1			1			1			1			1			1			# unix consensus / poly nt
syscon	prio	PRIO_USER				2			2			2			2			2			2			2			2			# unix consensus / poly nt
syscon	prio	PRIO_MIN				-20			-20			-20			-20			-20			-20			-20			-20			# unix consensus / poly nt
syscon	prio	PRIO_MAX				20			20			20			20			20			20			20			20			# unix consensus / poly nt

#	getrusage() who
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	rusage	RUSAGE_SELF				0			0			0			0			0			0			0			0			# unix consensus & faked nt
syscon	rusage	RUSAGE_THREAD				1			1			99			99			1			1			1			1			# faked nt & unavailable on xnu
syscon	rusage	RUSAGE_CHILDREN				-1			-1			-1			-1			-1			-1			-1			-1			# unix consensus & unavailable on nt
syscon	rusage	RUSAGE_BOTH				-2			-2			99			99			99			99			99			-2			# woop

#	fast userspace mutexes
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	futex	FUTEX_WAIT				0			0			0			0			0			1			0			0
syscon	futex	FUTEX_WAKE				1			1			0			0			1			2			1			1
syscon	futex	FUTEX_REQUEUE				3			3			0			0			0			3			0			0
syscon	futex	FUTEX_PRIVATE_FLAG			128			128			0			0			128			128			0			0

#	lio_listio() magnums
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	lio	LIO_WRITE				127			127			2			2			1			127			1			127
syscon	lio	LIO_NOWAIT				127			127			1			1			0			127			0			127
syscon	lio	LIO_READ				127			127			1			1			2			127			2			127
syscon	lio	LIO_WAIT				127			127			2			2			1			127			1			127
syscon	lio	LIO_NOP					127			127			0			0			0			127			0			127

#	posix scheduling
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	sched	SCHED_OTHER				0			0			127			127			2			127			0			127			# standard round-robin time-sharing policy
syscon	sched	SCHED_FIFO				1			1			127			127			1			127			1			127			# [real-time] first-in, first-out policy
syscon	sched	SCHED_RR				2			2			127			127			3			127			2			127			# [real-time] round-robin policy
syscon	sched	SCHED_BATCH				3			3			127			127			2			127			0			127			# for "batch" style execution of processes; polyfilled as SCHED_OTHER on non-Linux
syscon	sched	SCHED_IDLE				5			5			127			127			2			127			0			127			# for running very low priority background jobs; polyfilled as SCHED_OTHER on non-Linux
syscon	sched	SCHED_DEADLINE				6			6			127			127			127			127			127			127			# can only be set by sched_setattr()
syscon	sched	SCHED_RESET_ON_FORK			0x40000000		0x40000000		0			0			0			0			0			0			# Can be ORed in to make sure the process is reverted back to SCHED_NORMAL on fork(); no-op on non-Linux

#	Teletypewriter Control, e.g.
#
#		  TCSETS   → About 70,800 results (0.31 seconds)
#		= TCSETNOW → About 47,700 results (0.31 seconds)
#		≈ TCSETA   → About 12,600 results (0.32 seconds)
#		= TIOCSETA → About  3,110 results (0.41 seconds)
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	termios	TCGETS					0x5401			0x5401			0x40487413		0x40487413		0x402c7413		0x402c7413		0x402c7413		0x5401			# Gets console settings; tcgetattr(tty, argp) → ioctl(tty, TCGETS, struct termios *argp); polyfilled NT
syscon	termios	TCSETS					0x5402			0x5402			0x80487414		0x80487414		0x802c7414		0x802c7414		0x802c7414		0x5402			# Sets console settings; = ioctl(tty, TCSETS, const struct termios *argp); polyfilled NT
syscon	termios	TIOCGWINSZ				0x5413			0x5413			1074295912		1074295912		1074295912		1074295912		1074295912		0x5413			# ioctl(tty, TIOCGWINSZ, struct winsize *argp); polyfilled NT
syscon	termios	TIOCSWINSZ				0x5414			0x5414			0x80087467		0x80087467		0x80087467		0x80087467		0x80087467		0x5414			# ioctl(tty, TIOCSWINSZ, const struct winsize *argp) (faked NT)
syscon	termios	TIOCINQ					0x541b			0x541b			0x4004667f		0x4004667f		0x4004667f		0x4004667f		0x4004667f		0x4004667f		# [Linuxism] same as FIONREAD
syscon	termios	TIOCOUTQ				0x5411			0x5411			0x40047473		0x40047473		0x40047473		0x40047473		0x40047473		0			# get # bytes queued in TTY's output buffer ioctl(tty, TIOCSWINSZ, const struct winsize *argp)
syscon	termios	TIOCSPGRP				0x5410			0x5410			0x80047476		0x80047476		0x80047476		0x80047476		0x80047476		0			# tcsetpgrp(): set pgrp of tty
syscon	termios	TIOCCONS				0x541d			0x541d			0x80047462		0x80047462		0x80047462		0x80047462		0x80047462		0			# boop
syscon	termios	TIOCGETD				0x5424			0x5424			0x4004741a		0x4004741a		0x4004741a		0x4004741a		0x4004741a		0			# boop
syscon	termios	TIOCNOTTY				0x5422			0x5422			0x20007471		0x20007471		0x20007471		0x20007471		0x20007471		0			# boop
syscon	termios	TIOCNXCL				0x540d			0x540d			0x2000740e		0x2000740e		0x2000740e		0x2000740e		0x2000740e		0			# boop
syscon	termios	TIOCSCTTY				0x540e			0x540e			0x20007461		0x20007461		0x20007461		0x20007461		0x20007461		0			# makes terminal controlling terminal of calling process (see login_tty)
syscon	termios	TIOCSETD				0x5423			0x5423			0x8004741b		0x8004741b		0x8004741b		0x8004741b		0x8004741b		0			# boop
syscon	termios	TIOCSIG					0x40045436		0x40045436		0x2000745f		0x2000745f		0x2004745f		0x8004745f		0x8004745f		0			# boop
syscon	termios	TIOCSTI					0x5412			0x5412			0x80017472		0x80017472		0x80017472		0			0x80017472		0			# boop

#	Teletypewriter Control Modes
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	termios	CS5					0b0000000000000000	0b0000000000000000	0b000000000000000000	0b000000000000000000	0b000000000000000000	0b0000000000000000	0b0000000000000000	0b0000000000000000	# termios.c_cflag; consensus
syscon	termios	CS6					0b0000000000010000	0b0000000000010000	0b000000000100000000	0b000000000100000000	0b000000000100000000	0b0000000100000000	0b0000000100000000	0b0000000000010000	# termios.c_cflag; 6-bit characters
syscon	termios	CS7					0b0000000000100000	0b0000000000100000	0b000000001000000000	0b000000001000000000	0b000000001000000000	0b0000001000000000	0b0000001000000000	0b0000000000100000	# termios.c_cflag; 7-bit characters
syscon	termios	CS8					0b0000000000110000	0b0000000000110000	0b000000001100000000	0b000000001100000000	0b000000001100000000	0b0000001100000000	0b0000001100000000	0b0000000000110000	# termios.c_cflag; 8-bit characters
syscon	termios	CSIZE					0b0000000000110000	0b0000000000110000	0b000000001100000000	0b000000001100000000	0b000000001100000000	0b0000001100000000	0b0000001100000000	0b0000000000110000	# termios.c_cflag; mask for CS𝑥 flags
syscon	termios	CSTOPB					0b0000000001000000	0b0000000001000000	0b000000010000000000	0b000000010000000000	0b000000010000000000	0b0000010000000000	0b0000010000000000	0b0000000001000000	# termios.c_cflag; bsd consensus
syscon	termios	CREAD					0b0000000010000000	0b0000000010000000	0b000000100000000000	0b000000100000000000	0b000000100000000000	0b0000100000000000	0b0000100000000000	0b0000000010000000	# termios.c_cflag; bsd consensus
syscon	termios	PARENB					0b0000000100000000	0b0000000100000000	0b000001000000000000	0b000001000000000000	0b000001000000000000	0b0001000000000000	0b0001000000000000	0b0000000100000000	# termios.c_cflag
syscon	termios	PARODD					0b0000001000000000	0b0000001000000000	0b000010000000000000	0b000010000000000000	0b000010000000000000	0b0010000000000000	0b0010000000000000	0b0000001000000000	# termios.c_cflag
syscon	termios	HUPCL					0b0000010000000000	0b0000010000000000	0b000100000000000000	0b000100000000000000	0b000100000000000000	0b0100000000000000	0b0100000000000000	0b0000010000000000	# termios.c_cflag; bsd consensus
syscon	termios	CLOCAL					0b0000100000000000	0b0000100000000000	0b001000000000000000	0b001000000000000000	0b001000000000000000	0b1000000000000000	0b1000000000000000	0b0000100000000000	# termios.c_cflag; consensus
syscon	termios	CMSPAR					0x40000000		0x40000000		0			0			0			0			0			0			# termios.c_cflag; not in POSIX
syscon	termios	CRTSCTS					0x80000000		0x80000000		0x00030000		0x00030000		0x00030000		0x00010000		0x00010000		0x80000000		# termios.c_cflag

#	Teletypewriter Local Modes
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	termios	ISIG					0b0000000000000001	0b0000000000000001	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000000000001	# termios.c_lflag|=ISIG makes Ctrl-C, Ctrl-\, etc. generate signals
syscon	termios	ICANON					0b0000000000000010	0b0000000000000010	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000000000010	# termios.c_lflag&=~ICANON disables 1960's version of gnu readline (see also VMIN)
syscon	termios	XCASE					0b0000000000000100	0b0000000000000100	0			0			0			16777216		0			0b0000000000000100	# termios.c_lflag
syscon	termios	ECHO					0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	# termios.c_lflag&=~ECHO is for passwords and raw mode
syscon	termios	ECHOE					0b0000000000010000	0b0000000000010000	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000010000	# termios.c_lflag
syscon	termios	ECHOK					0b0000000000100000	0b0000000000100000	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000100000	# termios.c_lflag
syscon	termios	ECHONL					0b0000000001000000	0b0000000001000000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000001000000	# termios.c_lflag
syscon	termios	NOFLSH					0b0000000010000000	0b0000000010000000	2147483648		2147483648		2147483648		2147483648		2147483648		0b0000000010000000	# termios.c_lflag|=NOFLSH means don't flush on INT/QUIT/SUSP
syscon	termios	TOSTOP					0b0000000100000000	0b0000000100000000	4194304			4194304			4194304			4194304			4194304			0b0000000100000000	# termios.c_lflag|=TOSTOP raises SIGTTOU to process group if background job tries to write to controlling terminal
syscon	termios	ECHOCTL					0b0000001000000000	0b0000001000000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000001000000000	# termios.c_lflag|=ECHOCTL prints ^𝑥 codes for monotonic motion
syscon	termios	ECHOPRT					0b0000010000000000	0b0000010000000000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000010000000000	# termios.c_lflag|=ECHOPRT includes the parity bit
syscon	termios	ECHOKE					0b0000100000000000	0b0000100000000000	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000100000000000	# termios.c_lflag
syscon	termios	FLUSHO					0b0001000000000000	0b0001000000000000	8388608			8388608			8388608			8388608			8388608			0b0001000000000000	# termios.c_lflag
syscon	termios	PENDIN					0b0100000000000000	0b0100000000000000	536870912		536870912		536870912		536870912		536870912		0b0100000000000000	# termios.c_lflag
syscon	termios	IEXTEN					0b1000000000000000	0b1000000000000000	0b0000010000000000	0b0000010000000000	0b0000010000000000	0b0000010000000000	0b0000010000000000	0b1000000000000000	# termios.c_lflag&=~IEXTEN disables platform input processing magic
syscon	termios	EXTPROC					65536			65536			0b0000100000000000	0b0000100000000000	0b0000100000000000	0b0000100000000000	0b0000100000000000	65536			# termios.c_lflag

#	Teletypewriter Input Modes
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	termios	IGNBRK					0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	# termios.c_iflag it's complicated, uart only?                          UNIXCONSENSUS
syscon	termios	BRKINT					0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	# termios.c_iflag it's complicated, uart only?                          UNIXCONSENSUS
syscon	termios	IGNPAR					0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	# termios.c_iflag|=IGNPAR ignores parity and framing errors; see PARMRK UNIXCONSENSUS
syscon	termios	PARMRK					0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	# termios.c_iflag|=PARMRK passes-through parity bit                     UNIXCONSENSUS
syscon	termios	INPCK					0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	# termios.c_iflag|=INPCK enables parity checking                        UNIXCONSENSUS
syscon	termios	ISTRIP					0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	# termios.c_iflag|=ISTRIP automates read(1)&0x7f                        UNIXCONSENSUS
syscon	termios	INLCR					0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	# termios.c_iflag|=INLCR maps \n → \r input                             UNIXCONSENSUS
syscon	termios	IGNCR					0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	# termios.c_iflag|=IGNCR maps \r → ∅ input                              UNIXCONSENSUS
syscon	termios	ICRNL					0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	# termios.c_iflag|=ICRNL maps \r → \n input                             UNIXCONSENSUS
syscon	termios	IUCLC					0b0000001000000000	0b0000001000000000	0			0			0			0b0001000000000000	0			0b0000001000000000	# termios.c_iflag|=IUCLC maps A-Z → a-z input
syscon	termios	IXON					0b0000010000000000	0b0000010000000000	0b0000001000000000	0b0000001000000000	0b0000001000000000	0b0000001000000000	0b0000001000000000	0b0000010000000000	# termios.c_iflag|=IXON enables flow rida
syscon	termios	IXANY					0b0000100000000000	0b0000100000000000	0b0000100000000000	0b0000100000000000	0b0000100000000000	0b0000100000000000	0b0000100000000000	0b0000100000000000	# termios.c_iflag|=IXANY typing will un-stuck teletype                  UNIXCONSENSUS
syscon	termios	IXOFF					0b0001000000000000	0b0001000000000000	0b0000010000000000	0b0000010000000000	0b0000010000000000	0b0000010000000000	0b0000010000000000	0b0001000000000000	# termios.c_iflag|=IXOFF disables annoying display freeze keys
syscon	termios	IMAXBEL					0b0010000000000000	0b0010000000000000	0b0010000000000000	0b0010000000000000	0b0010000000000000	0b0010000000000000	0b0010000000000000	0b0010000000000000	# termios.c_iflag|=IMAXBEL rings when queue full                        UNIXCONSENSUS
syscon	termios	IUTF8					0b0100000000000000	0b0100000000000000	0b0100000000000000	0b0100000000000000	0			0			0			0b0100000000000000	# termios.c_iflag|=IUTF8 helps w/ rubout on UTF-8 input

#	Teletypewriter Output Modes
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	termios	OPOST					0b0000000000000001	0b0000000000000001	0b000000000000000001	0b000000000000000001	0b000000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	# termios.c_oflag&=~OPOST disables output processing magic, e.g. MULTICS newlines
syscon	termios	OLCUC					0b0000000000000010	0b0000000000000010	0			0			0			0b0000000000100000	0			0b0000000000000010	# termios.c_oflag|=OLCUC maps a-z → A-Z output (SHOUTING)
syscon	termios	ONLCR					0b0000000000000100	0b0000000000000100	0b000000000000000010	0b000000000000000010	0b000000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000100	# termios.c_oflag|=ONLCR map \n → \r\n output (MULTICS newline) and requires OPOST
syscon	termios	OCRNL					0b0000000000001000	0b0000000000001000	0b000000000000010000	0b000000000000010000	0b000000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000001000	# termios.c_oflag|=OCRNL maps \r → \n output
syscon	termios	ONOCR					0b0000000000010000	0b0000000000010000	0b000000000000100000	0b000000000000100000	0b000000000000100000	0b0000000001000000	0b0000000001000000	0b0000000000010000	# termios.c_oflag|=ONOCR maps \r → ∅ output iff column 0
syscon	termios	ONLRET					0b0000000000100000	0b0000000000100000	0b000000000001000000	0b000000000001000000	0b000000000001000000	0b0000000010000000	0b0000000010000000	0b0000000000100000	# termios.c_oflag|=ONLRET maps \r → ∅ output
syscon	termios	OFILL					0b0000000001000000	0b0000000001000000	0b000000000010000000	0b000000000010000000	0			0			0			0b0000000001000000	# termios.c_oflag
syscon	termios	OFDEL					0b0000000010000000	0b0000000010000000	0b100000000000000000	0b100000000000000000	0			0			0			0b0000000010000000	# termios.c_oflag
syscon	termios	NLDLY					0b0000000100000000	0b0000000100000000	0b000000001100000000	0b000000001100000000	0b000000001100000000	0			0			0b0000000100000000	# (termios.c_oflag & NLDLY) ∈ {NL0,NL1,NL2,NL3}
syscon	termios	  NL1					0b0000000100000000	0b0000000100000000	0b000000000100000000	0b000000000100000000	0b000000000100000000	0			0b000000000100000000	0b0000000100000000	# (termios.c_oflag & NLDLY) == NL1
syscon	termios	  NL2					0			0			0b000000001000000000	0b000000001000000000	0b000000001000000000	0			0b000000001000000000	0			# (termios.c_oflag & NLDLY) == NL2
syscon	termios	  NL3					0			0			0b000000001100000000	0b000000001100000000	0b000000001100000000	0			0b000000001100000000	0			# (termios.c_oflag & NLDLY) == NL3
syscon	termios	CRDLY					0b0000011000000000	0b0000011000000000	0b000011000000000000	0b000011000000000000	0b000011000000000000	0			0			0b0000011000000000	# (termios.c_oflag & CRDLY) ∈ {CR0,CR1,CR2,CR3}
syscon	termios	  CR1					0b0000001000000000	0b0000001000000000	0b000001000000000000	0b000001000000000000	0b000001000000000000	0			0b000001000000000000	0b0000001000000000	# (termios.c_oflag & CRDLY) == CR1
syscon	termios	  CR2					0b0000010000000000	0b0000010000000000	0b000010000000000000	0b000010000000000000	0b000010000000000000	0			0b000000010000000000	0b0000010000000000	# (termios.c_oflag & CRDLY) == CR2
syscon	termios	  CR3					0b0000011000000000	0b0000011000000000	0b000011000000000000	0b000011000000000000	0b000011000000000000	0			0b000000011000000000	0b0000011000000000	# (termios.c_oflag & CRDLY) == CR3
syscon	termios	TABDLY					0b0001100000000000	0b0001100000000000	0b000000110000000100	0b000000110000000100	0b000000000000000100	0			0			0b0001100000000000	# (termios.c_oflag & TABDLY) ∈ {TAB0,TAB1,TAB2,TAB3,XTABS}
syscon	termios	  TAB1					0b0000100000000000	0b0000100000000000	0b000000010000000000	0b000000010000000000	0b000000010000000000	0			0b000000010000000000	0b0000100000000000	# (termios.c_oflag & TABDLY) == TAB1
syscon	termios	  TAB2					0b0001000000000000	0b0001000000000000	0b000000100000000000	0b000000100000000000	0b000000100000000000	0			0b000000100000000000	0b0001000000000000	# (termios.c_oflag & TABDLY) == TAB2
syscon	termios	  TAB3					0b0001100000000000	0b0001100000000000	0b000000000000000100	0b000000000000000100	0b000000000000000100	0			0b000000000000000100	0b0001100000000000	# (termios.c_oflag & TABDLY) == TAB3
syscon	termios	  XTABS					0b0001100000000000	0b0001100000000000	0b000000110000000000	0b000000110000000000	0b000000110000000000	0			0b000000110000000000	0b0001100000000000	# (termios.c_oflag & TABDLY) == XTABS
syscon	termios	BSDLY					0b0010000000000000	0b0010000000000000	0b001000000000000000	0b001000000000000000	0b001000000000000000	0			0			0b0010000000000000	# termios.c_oflag
syscon	termios	  BS1					0b0010000000000000	0b0010000000000000	0b001000000000000000	0b001000000000000000	0b001000000000000000	0			0			0b0010000000000000	# termios.c_oflag
syscon	termios	VTDLY					0b0100000000000000	0b0100000000000000	0b010000000000000000	0b010000000000000000	0b010000000000000000	0			0			0b0100000000000000	# termios.c_oflag
syscon	termios	  VT1					0b0100000000000000	0b0100000000000000	0b010000000000000000	0b010000000000000000	0b010000000000000000	0			0			0b0100000000000000	# termios.c_oflag
syscon	termios	FFDLY					0b1000000000000000	0b1000000000000000	0b000100000000000000	0b000100000000000000	0b000100000000000000	0			0			0b1000000000000000	# termios.c_oflag
syscon	termios	  FF1					0b1000000000000000	0b1000000000000000	0b000100000000000000	0b000100000000000000	0b000100000000000000	0			0			0b1000000000000000	# termios.c_oflag

#	Teletypewriter Special Control Character Assignments
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	termios	VMIN					6+1			6+1			16			16			16			16			16			6+1			# termios.c_cc[VMIN]=𝑥 in non-canonical mode can be set to 0 for non-blocking reads, 1 for single character raw mode reads, or higher to buffer
syscon	termios	VTIME					5+1			5+1			17			17			17			17			17			5+1			# termios.c_cc[VTIME]=𝑥 sets non-canonical read timeout to 𝑥×𝟷𝟶𝟶ms which is needed when entering escape sequences manually with the escape key
syscon	termios	VINTR					0+1			0+1			8			8			8			8			8			0+1			# termios.c_cc[VINTR]=𝑥
syscon	termios	VQUIT					1+1			1+1			9			9			9			9			9			1+1			# termios.c_cc[VQUIT]=𝑥
syscon	termios	VERASE					2+1			2+1			3			3			3			3			3			2+1			# termios.c_cc[VERASE]=𝑥
syscon	termios	VKILL					3+1			3+1			5			5			5			5			5			3+1			# termios.c_cc[VKILL]=𝑥
syscon	termios	VEOF					4+1			4+1			0			0			0			0			0			4+1			# termios.c_cc[VEOF]=𝑥
syscon	termios	VSWTC					7+1			7+1			0			0			0			0			0			7+1			# termios.c_cc[VSWTC]=𝑥
syscon	termios	VSTART					8+1			8+1			12			12			12			12			12			8+1			# termios.c_cc[VSTART]=𝑥
syscon	termios	VSTOP					9+1			9+1			13			13			13			13			13			9+1			# termios.c_cc[VSTOP]=𝑥
syscon	termios	VSUSP					10+1			10+1			10			10			10			10			10			10+1			# termios.c_cc[VSUSP]=𝑥 defines suspend, i.e. Ctrl-Z (a.k.a. →, ^Z, SUB, 26, 032, 0x1A, ord('Z')^0b01000000); unix consensus
syscon	termios	VEOL					11+1			11+1			1			1			1			1			1			11+1			# termios.c_cc[VEOL]=𝑥
syscon	termios	VREPRINT				12+1			12+1			6			6			6			6			6			12+1			# termios.c_cc[VREPRINT]=𝑥
syscon	termios	VDISCARD				13+1			13+1			15			15			15			15			15			13+1			# termios.c_cc[VDISCARD]=𝑥
syscon	termios	VWERASE					14+1			14+1			4			4			4			4			4			14+1			# termios.c_cc[VWERASE]=𝑥
syscon	termios	VLNEXT					15+1			15+1			14			14			14			14			14			15+1			# termios.c_cc[VLNEXT]=𝑥
syscon	termios	VEOL2					16+1			16+1			2			2			2			2			2			16+1			# termios.c_cc[VEOL2]=𝑥
syscon	termios	_POSIX_VDISABLE				0			0			255			255			255			255			255			0			# termios.c_cc tombstone value

#	tcflush() magic numbers
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	termios	TCIFLUSH				0			0			1			1			1			1			1			0			# see tcflush; FREAD on BSD; faked nt
syscon	termios	TCOFLUSH				1			1			2			2			2			2			2			1			# see tcflush; FWRITE on BSD; faked nt
syscon	termios	TCIOFLUSH				2			2			3			3			3			3			3			2			# see tcflush; FREAD|FWRITE on BSD; faked nt

syscon	termios	CSTART					17			17			17			17			17			17			17			0			# unix consensus
syscon	termios	CSTOP					19			19			19			19			19			19			19			0			# unix consensus

#	Pseudoteletypewriter Control
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	pty	TIOCPKT					0x5420			0x5420			0x80047470		0x80047470		0x80047470		0x80047470		0x80047470		-1			# boop
syscon	pty	TIOCPKT_DATA				0			0			0			0			0			0			0			0			# consensus
syscon	pty	TIOCPKT_FLUSHREAD			1			1			1			1			1			1			1			1			# unix consensus
syscon	pty	TIOCPKT_FLUSHWRITE			2			2			2			2			2			2			2			2			# unix consensus
syscon	pty	TIOCPKT_STOP				4			4			4			4			4			4			4			4			# unix consensus
syscon	pty	TIOCPKT_START				8			8			8			8			8			8			8			8			# unix consensus
syscon	pty	TIOCPKT_NOSTOP				16			16			16			16			16			16			16			16			# unix consensus
syscon	pty	TIOCPKT_DOSTOP				32			32			32			32			32			32			32			32			# unix consensus
syscon	pty	TIOCPKT_IOCTL				64			64			64			64			64			64			64			64			# unix consensus

#	Modem Control
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	modem	TIOCMGET				0x5415			0x5415			0x4004746a		0x4004746a		0x4004746a		0x4004746a		0x4004746a		-1			# get status of modem bits; ioctl(fd, TIOCMGET, int *argp)
syscon	modem	TIOCMSET				0x5418			0x5418			0x8004746d		0x8004746d		0x8004746d		0x8004746d		0x8004746d		-1			# set status of modem bits; ioctl(fd, TIOCMSET, const int *argp)
syscon	modem	TIOCMBIC				0x5417			0x5417			0x8004746b		0x8004746b		0x8004746b		0x8004746b		0x8004746b		-1			# clear indicated modem bits; ioctl(fd, TIOCMBIC, int *argp)
syscon	modem	TIOCMBIS				0x5416			0x5416			0x8004746c		0x8004746c		0x8004746c		0x8004746c		0x8004746c		-1			# set indicated modem bits; ioctl(fd, TIOCMBIS, int *argp)
syscon	modem	TIOCM_LE				0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	0b0000000000000001	# consensus
syscon	modem	TIOCM_DTR				0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	0b0000000000000010	# consensus
syscon	modem	TIOCM_RTS				0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	0b0000000000000100	# consensus
syscon	modem	TIOCM_ST				0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	0b0000000000001000	# consensus
syscon	modem	TIOCM_SR				0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	0b0000000000010000	# consensus
syscon	modem	TIOCM_CTS				0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	0b0000000000100000	# consensus
syscon	modem	TIOCM_CAR				0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	# consensus
syscon	modem	TIOCM_CD				0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	0b0000000001000000	# boop
syscon	modem	TIOCM_RI				0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	# boop
syscon	modem	TIOCM_RNG				0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	0b0000000010000000	# boop
syscon	modem	TIOCM_DSR				0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	0b0000000100000000	# consensus
syscon	modem	TIOCM_DCD				0			0			0			0			0x40			0			0			-1			# wut
syscon	modem	TIOCMODG				0			0			0x40047403		0x40047403		0			0x4004746a		0x4004746a		-1			# wut
syscon	modem	TIOCMODS				0			0			0x80047404		0x80047404		0			0x8004746d		0x8004746d		-1			# wut
syscon	modem	TIOCMSDTRWAIT				0			0			0x8004745b		0x8004745b		0x8004745b		0			0			-1			# wut

#	ioctl(SIOCGIFFLAGS) Network Interface Flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	iff	IFF_UP					1			1			1			1			1			1			1			1			# unix consensus
syscon	iff	IFF_BROADCAST				2			2			2			2			2			2			2			2			# valid broadcast address set; consensus
syscon	iff	IFF_DEBUG				4			4			4			4			4			4			4			4			# faked nt; consensus
syscon	iff	IFF_LOOPBACK				8			8			8			8			8			8			8			4			# is loopback device; consensus
syscon	iff	IFF_MULTICAST				0x1000			0x1000			0x8000			0x8000			0x8000			0x8000			0x8000			0x1000			# supports multicast; faked nt; bsd consensus
syscon	iff	IFF_ALLMULTI				0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			# receive all multicast packets; faked nt; unix consensus
syscon	iff	IFF_NOARP				0x80			0x80			0x80			0x80			0x80			0x80			0x80			0x80			# faked nt as linux; unix consensus
syscon	iff	IFF_POINTOPOINT				0x10			0x10			0x10			0x10			0x10			0x10			0x10			0x10			# point-to-point; faked nt as linux; unix consensus
syscon	iff	IFF_PROMISC				0x100			0x100			0x100			0x100			0x100			0x100			0x100			0			# in packet capture mode; unix consensus
syscon	iff	IFF_RUNNING				0x40			0x40			0x40			0x40			0x40			0x40			0x40			0			# unix consensus (mostly for bsd compatibility?)
syscon	iff	IFF_NOTRAILERS				0x20			0x20			0x20			0x20			0			0			0			0
syscon	iff	IFF_AUTOMEDIA				0x4000			0x4000			0			0			0			0			0			0
syscon	iff	IFF_DYNAMIC				0x8000			0x8000			0			0			0			0			0			0
syscon	iff	IFF_MASTER				0x0400			0x0400			0			0			0			0			0			0
syscon	iff	IFF_PORTSEL				0x2000			0x2000			0			0			0			0			0			0
syscon	iff	IFF_SLAVE				0x0800			0x0800			0			0			0			0			0			0

syscon	sock	SOCK_STREAM				1			1			1			1			1			1			1			1			# consensus
syscon	sock	SOCK_DGRAM				2			2			2			2			2			2			2			2			# consensus
syscon	sock	SOCK_RAW				3			3			3			3			3			3			3			3			# consensus
syscon	sock	SOCK_RDM				4			4			4			4			4			4			4			4			# consensus
syscon	sock	SOCK_SEQPACKET				5			5			5			5			5			5			5			5			# consensus
syscon	sock	SOCK_CLOEXEC				0x080000		0x080000		0x080000		0x080000		0x10000000		0x8000			0x10000000		0x00080000		# faked xnu & faked nt to be some as O_CLOEXEC
syscon	sock	SOCK_NONBLOCK				0x0800			0x0800			0x0800			0x0800			0x20000000		0x4000			0x20000000		0x00000800		# faked xnu & faked nt to be same as O_NONBLOC and socket() will ioctl(FIONBIO=1)
syscon	sock	SOCK_DCCP				6			6			0			0			0			0			0			0			# what is it?
syscon	sock	SOCK_PACKET				10			10			0			0			0			0			0			0			# what is it?

syscon	shm	SHM_R					0x0100			0x0100			0x0100			0x0100			0x0100			0x0100			0x0100			0			# unix consensus
syscon	shm	SHM_RDONLY				0x1000			0x1000			0x1000			0x1000			0x1000			0x1000			0x1000			0			# unix consensus
syscon	shm	SHM_RND					0x2000			0x2000			0x2000			0x2000			0x2000			0x2000			0x2000			0			# unix consensus
syscon	shm	SHM_W					0x80			0x80			0x80			0x80			0x80			0x80			0x80			0			# unix consensus
syscon	shm	SHM_LOCK				11			11			0			0			11			3			3			0
syscon	shm	SHM_UNLOCK				12			12			0			0			12			4			4			0
syscon	shm	SHM_INFO				14			14			0			0			14			0			0			0
syscon	shm	SHM_STAT				13			13			0			0			13			0			0			0
syscon	shm	SHM_DEST				0x0200			0x0200			0			0			0			0			0			0
syscon	shm	SHM_EXEC				0x8000			0x8000			0			0			0			0			0			0
syscon	shm	SHM_HUGETLB				0x0800			0x0800			0			0			0			0			0			0
syscon	shm	SHM_LOCKED				0x0400			0x0400			0			0			0			0			0			0
syscon	shm	SHM_NORESERVE				0x1000			0x1000			0			0			0			0			0			0
syscon	shm	SHM_REMAP				0x4000			0x4000			0			0			0			0			0			0
syscon	shm	SHM_ANON				0			0			0			0			1			0			0			0

syscon	lock	LOCK_UNLOCK_CACHE			54			54			0			0			0			0			0			0			# wut

syscon	misc	IP6F_MORE_FRAG				0x0100			0x0100			0x0100			0x0100			0x0100			0x0100			0x0100			0x0100			# consensus
syscon	misc	IP6F_OFF_MASK				0xf8ff			0xf8ff			0xf8ff			0xf8ff			0xf8ff			0xf8ff			0xf8ff			0xf8ff			# consensus
syscon	misc	IP6F_RESERVED_MASK			0x0600			0x0600			0x0600			0x0600			0x0600			0x0600			0x0600			0x0600			# consensus

syscon	misc	L_SET					0			0			0			0			0			0			0			0			# consensus
syscon	misc	L_INCR					1			1			1			1			1			1			1			0			# unix consensus
syscon	misc	L_XTND					2			2			2			2			2			2			2			0			# unix consensus

syscon	misc	SHUT_RD					0			0			0			0			0			0			0			0			# consensus (SD_RECEIVE)
syscon	misc	SHUT_WR					1			1			1			1			1			1			1			1			# consensus (SD_SEND)
syscon	misc	SHUT_RDWR				2			2			2			2			2			2			2			2			# consensus (SD_BOTH)

syscon	misc	Q_QUOTAOFF				0x800003		0x800003		0x0200			0x0200			0x0200			0x0200			0x0200			0			# bsd consensus
syscon	misc	Q_QUOTAON				0x800002		0x800002		0x0100			0x0100			0x0100			0x0100			0x0100			0			# bsd consensus
syscon	misc	Q_SYNC					0x800001		0x800001		0x0600			0x0600			0x0600			0x0600			0x0600			0			# bsd consensus
syscon	misc	Q_GETQUOTA				0x800007		0x800007		768			768			0x0700			768			768			0
syscon	misc	Q_SETQUOTA				0x800008		0x800008		0x0400			0x0400			0x0800			0x0400			0x0400			0
syscon	misc	Q_GETFMT				0x800004		0x800004		0			0			0			0			0			0
syscon	misc	Q_GETINFO				0x800005		0x800005		0			0			0			0			0			0
syscon	misc	Q_SETINFO				0x800006		0x800006		0			0			0			0			0			0

syscon	misc	RPM_PCO_ADD				1			1			1			1			1			1			1			0			# unix consensus
syscon	misc	RPM_PCO_CHANGE				2			2			2			2			2			2			2			0			# unix consensus
syscon	misc	RPM_PCO_SETGLOBAL			3			3			3			3			3			3			3			0			# unix consensus

syscon	misc	FSETLOCKING_QUERY			0			0			0			0			0			0			0			0			# consensus
syscon	misc	FSETLOCKING_BYCALLER			2			2			0			0			0			0			0			0
syscon	misc	FSETLOCKING_INTERNAL			1			1			0			0			0			0			0			0

syscon	misc	MAX_DQ_TIME				0x093a80		0x093a80		0x093a80		0x093a80		0x093a80		0x093a80		0x093a80		0			# unix consensus
syscon	misc	MAX_IQ_TIME				0x093a80		0x093a80		0x093a80		0x093a80		0x093a80		0x093a80		0x093a80		0			# unix consensus
syscon	misc	MAX_HANDLE_SZ				0x80			0x80			0			0			0			0			0			0

syscon	mlock	MCL_CURRENT				1			1			1			1			1			1			1			0			# unix consensus
syscon	mlock	MCL_FUTURE				2			2			2			2			2			2			2			0			# unix consensus
syscon	mlock	MCL_ONFAULT				4			4			0			0			0			0			0			0

syscon	misc	MLD_LISTENER_QUERY			130			130			130			130			130			130			130			0			# unix consensus
syscon	misc	MLD_LISTENER_REPORT			131			131			131			131			131			131			131			0			# unix consensus
syscon	misc	MLD_LISTENER_REDUCTION			132			132			132			132			132			0			0			0

syscon	misc	TTYDEF_CFLAG				0x05a0			0x05a0			0x4b00			0x4b00			0x4b00			0x4b00			0x4b00			0			# bsd consensus
syscon	misc	TTYDEF_IFLAG				0x2d22			0x2d22			0x2b02			0x2b02			0x2b02			0x2b02			0x2b02			0			# bsd consensus
syscon	misc	TTYDEF_LFLAG				0x8a1b			0x8a1b			0x05cb			0x05cb			0x05cb			0x05cb			0x05cb			0			# bsd consensus
syscon	misc	TTYDEF_SPEED				13			13			0x2580			0x2580			0x2580			0x2580			0x2580			0			# bsd consensus
syscon	misc	TTYDEF_OFLAG				0x1805			0x1805			3			3			3			7			7			0

syscon	misc	ACCT_BYTEORDER				0			0			0			0			0			0			0			0			# consensus
syscon	misc	ACCT_COMM				0x10			0x10			0			0			0			0			0			0

syscon	misc	COMMAND_COMPLETE			0			0			0			0			0			0			0			0			# consensus
syscon	misc	COMMAND_TERMINATED			17			17			0			0			0			0			0			0

syscon	select	FD_SETSIZE				0x0400			0x0400			0x0400			0x0400			0x0400			0x0400			0x0400			0x0400			# forced consensus (0x40 on NT)

syscon	misc	MATH_ERREXCEPT				2			2			2			2			2			2			2			0			# unix consensus
syscon	misc	MATH_ERRNO				1			1			1			1			1			1			1			0			# unix consensus

syscon	misc	MCAST_BLOCK_SOURCE			43			43			84			84			84			0			0			43
syscon	misc	MCAST_JOIN_GROUP			42			42			80			80			80			0			0			41
syscon	misc	MCAST_JOIN_SOURCE_GROUP			46			46			82			82			82			0			0			45
syscon	misc	MCAST_LEAVE_GROUP			45			45			81			81			81			0			0			42
syscon	misc	MCAST_LEAVE_SOURCE_GROUP		47			47			83			83			83			0			0			46
syscon	misc	MCAST_UNBLOCK_SOURCE			44			44			85			85			85			0			0			44
syscon	misc	MCAST_INCLUDE				1			1			1			1			1			0			0			0
syscon	misc	MCAST_EXCLUDE				0			0			2			2			2			0			0			0
syscon	misc	MCAST_MSFILTER				48			48			0			0			0			0			0			0

syscon	misc	AREGTYPE				0			0			0			0			0			0			0			0			# consensus
syscon	misc	CTIME					0			0			0			0			0			0			0			0			# consensus
syscon	misc	EFD_CLOEXEC				0x080000		0x080000		0			0			0			0			0			0
syscon	misc	EFD_NONBLOCK				0x0800			0x0800			0			0			0			0			0			0
syscon	misc	EFD_SEMAPHORE				1			1			0			0			0			0			0			0

syscon	misc	TEST_UNIT_READY				0			0			0			0			0			0			0			0
syscon	misc	TFD_CLOEXEC				0x080000		0x080000		0			0			0			0			0			0
syscon	misc	TFD_NONBLOCK				0x0800			0x0800			0			0			0			0			0			0
syscon	misc	TFD_TIMER_ABSTIME			1			1			0			0			0			0			0			0

syscon	misc	USRQUOTA				0			0			0			0			0			0			0			0

syscon	misc	HOST_NAME_MAX				0x40			0x40			0			0			0			255			255			0

syscon	misc	UDP_ENCAP_ESPINUDP_NON_IKE		1			1			0			0			1			0			0			0
syscon	misc	UDP_NO_CHECK6_RX			102			102			0			0			0			0			0			0
syscon	misc	UDP_NO_CHECK6_TX			101			101			0			0			0			0			0			0

syscon	misc	ACK					4			4			4			4			4			4			4			0			# unix consensus
syscon	misc	CDISCARD				15			15			15			15			15			15			15			0			# unix consensus
syscon	misc	CEOT					4			4			4			4			4			4			4			0			# unix consensus
syscon	misc	CERASE					127			127			127			127			127			127			127			0			# unix consensus
syscon	misc	CHRTYPE					51			51			51			51			51			51			51			0			# unix consensus
syscon	misc	CKILL					21			21			21			21			21			21			21			0			# unix consensus
syscon	misc	CLNEXT					22			22			22			22			22			22			22			0			# unix consensus
syscon	misc	CMIN					1			1			1			1			1			1			1			0			# unix consensus
syscon	misc	COMPLETE				2			2			2			2			2			2			2			0			# unix consensus
syscon	misc	CONTINUE				3			3			3			3			3			3			3			0			# unix consensus
syscon	misc	CONTTYPE				55			55			55			55			55			55			55			0			# unix consensus
syscon	misc	COPY_ABORTED				10			10			0			0			0			0			0			0
syscon	misc	COPY_VERIFY				58			58			0			0			0			0			0			0

syscon	misc	CQUIT					28			28			28			28			28			28			28			0			# unix consensus
syscon	misc	CREPRINT				18			18			18			18			18			18			18			0			# unix consensus
syscon	misc	CRPRNT					18			18			18			18			18			18			18			0			# unix consensus
syscon	misc	DATA					3			3			3			3			3			3			3			0			# unix consensus
syscon	misc	DEV_BSIZE				0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			0			# unix consensus
syscon	misc	DIRTYPE					53			53			53			53			53			53			53			0			# unix consensus
syscon	misc	ELF_NGREG				27			27			0			0			0			0			0			0
syscon	misc	ELF_PRARGSZ				80			80			0			0			0			0			0			0

syscon	misc	EM_ALTERA_NIOS2				113			113			0			0			0			0			0			0
syscon	misc	EM_LATTICEMICO32			138			138			0			0			0			0			0			0

syscon	misc	EXPR_NEST_MAX				0x20			0x20			0x20			0x20			0x20			0x20			0x20			0			# unix consensus

#	linux fallocate() flags
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	misc	FALLOC_FL_KEEP_SIZE			0x01			0x01			-1			-1			-1			-1			-1			-1			# bsd consensus
syscon	misc	FALLOC_FL_PUNCH_HOLE			0x02			0x02			-1			-1			-1			-1			-1			-1			# bsd consensus
syscon	misc	FALLOC_FL_NO_HIDE_STALE			0x04			0x04			-1			-1			-1			-1			-1			-1			# bsd consensus
syscon	misc	FALLOC_FL_COLLAPSE_RANGE		0x08			0x08			-1			-1			-1			-1			-1			-1			# bsd consensus
syscon	misc	FALLOC_FL_ZERO_RANGE			0x10			0x10			-1			-1			-1			-1			-1			0x000980C8		# bsd consensus & kNtFsctlSetZeroData
syscon	misc	FALLOC_FL_INSERT_RANGE			0x20			0x20			-1			-1			-1			-1			-1			-1			# bsd consensus
syscon	misc	FALLOC_FL_UNSHARE_RANGE			0x40			0x40			-1			-1			-1			-1			-1			-1			# bsd consensus

#	Video Output Control Ioctls (mostly platform-specific)
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	vid	FBIOGET_VSCREENINFO			0x4600			0x4600			0			0			0			0			0			0
syscon	vid	FBIOPUT_VSCREENINFO			0x4601			0x4601			0			0			0			0			0			0
syscon	vid	FBIOGET_FSCREENINFO			0x4602			0x4602			0			0			0			0			0			0
syscon	vid	FBIOGETCMAP				0x4604			0x4604			0			0			0			0			0			0
syscon	vid	FBIOPUTCMAP				0x4605			0x4605			0			0			0			0			0			0
syscon	vid	FBIOPAN_DISPLAY				0x4606			0x4606			0			0			0			0			0			0
syscon	vid	FBIO_WAITFORVSYNC			0x40044620		0x40044620		0			0			0			0			0			0
#syscon	vid	FBIO_CACHE_SYNC				0x4630			0x4630			0			0			0			0			0			0			# does any Linux kernel driver actually implement this ioctl? only Chinese sources mention it
syscon	vid	GIO_FONT				0x4b60			0x4b60			0			0			0			0			0			0
syscon	vid	PIO_FONT				0x4b61			0x4b61			0			0			0			0			0			0
syscon	vid	GIO_FONTX				0x4b6b			0x4b6b			0			0			0			0			0			0
syscon	vid	PIO_FONTX				0x4b6c			0x4b6c			0			0			0			0			0			0
syscon	vid	PIO_FONT8x8				0			0			0			0			0x88006340		0			0			0
syscon	vid	PIO_FONT8x14				0			0			0			0			0x8e006342		0			0			0
syscon	vid	PIO_FONT8x16				0			0			0			0			0x90006344		0			0			0
syscon	vid	PIO_VFONT				0			0			0			0			0x84086346		0			0			0
syscon	vid	PIO_FONTRESET				0x4b6d			0x4b6d			0			0			0x20006348		0			0			0			# PIO_VFONT_DEFAULT on FreeBSD, absent on OpenBSD
syscon	vid	KDFONTOP				0x4b72			0x4b72			0			0			0			0			0			0

#	System Call Numbers.
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology
syscon	nr	__NR_exit				0x003c			0x005d			0x2000169		0x0169			0x01af			0x012e			0x136			0xfff			# __bsdthread_terminate() on XNU, thr_exit() on FreeBSD, __threxit() on OpenBSD, __lwp_exit() on NetBSD
syscon	nr	__NR_exit_group				0x00e7			0x005e			0x2000001		0x0001			0x0001			0x0001			0x001			0xfff
syscon	nr	__NR_read				0x0000			0x003f			0x2000003		0x0003			0x0003			0x0003			0x003			0xfff
syscon	nr	__NR_write				0x0001			0x0040			0x200018d		0x0004			0x0004			0x0004			0x004			0xfff			# write_nocancel() on XNU
syscon	nr	__NR_open				0x0002			0x00b4			0x200018e		0x0005			0x0005			0x0005			0x005			0xfff			# open_nocancel() on XNU
syscon	nr	__NR_close				0x0003			0x0039			0x2000006		0x0006			0x0006			0x0006			0x006			0xfff
syscon	nr	__NR_stat				0x0004			0x004f			0x2000152		0x0152			0xfff			0x0026			0x1b7			0xfff
syscon	nr	__NR_fstat				0x0005			0x0050			0x2000153		0x0153			0x0227			0x0035			0x1b8			0xfff
syscon	nr	__NR_lstat				0x0006			0x0fff			0x2000154		0x0154			0x0028			0x0028			0x1b9			0xfff
syscon	nr	__NR_poll				0x0007			0x0fff			0x20000e6		0x00e6			0x00d1			0x00fc			0x0d1			0xfff
syscon	nr	__NR_ppoll				0x010f			0x0049			0xfff			0xfff			0x0221			0x006d			0xfff			0xfff
syscon	nr	__NR_lseek				0x0008			0x003e			0x20000c7		0x00c7			0x01de			0x00a6			0x0c7			0xfff			# OpenBSD 7.3+
syscon	nr	__NR_mmap				0x0009			0x00de			0x20000c5		0x00c5			0x01dd			0x0031			0x0c5			0xfff			# OpenBSD 7.3+
syscon	nr	__NR_msync				0x001a			0x00e3			0x2000041		0x0041			0x0041			0x0100			0x115			0xfff
syscon	nr	__NR_mprotect				0x000a			0x00e2			0x200004a		0x004a			0x004a			0x004a			0x04a			0xfff
syscon	nr	__NR_munmap				0x000b			0x00d7			0x2000049		0x0049			0x0049			0x0049			0x049			0xfff
syscon	nr	__NR_sigaction				0x000d			0x0086			0x200002e		0x002e			0x01a0			0x002e			0x154			0xfff
syscon	nr	__NR_sigprocmask			0x000e			0x0087			0x2000149		0x0149			0x0154			0x0030			0x125			0xfff
syscon	nr	__NR_ioctl				0x0010			0x001d			0x2000036		0x0036			0x0036			0x0036			0x036			0xfff
syscon	nr	__NR_pread				0x0011			0x0043			0x2000099		0x0099			0x01db			0x00a9			0x0ad			0xfff			# OpenBSD 7.3+
syscon	nr	__NR_pwrite				0x0012			0x0044			0x200009a		0x009a			0x01dc			0x00aa			0x0ae			0xfff			# OpenBSD 7.3+
syscon	nr	__NR_readv				0x0013			0x0041			0x2000078		0x0078			0x0078			0x0078			0x078			0xfff
syscon	nr	__NR_writev				0x0014			0x0042			0x2000079		0x0079			0x0079			0x0079			0x079			0xfff
syscon	nr	__NR_access				0x0015			0x0fff			0x2000021		0x0021			0x0021			0x0021			0x021			0xfff
syscon	nr	__NR_pipe				0x0016			0x0fff			0x200002a		0x002a			0x021e			0x0107			0x02a			0xfff
syscon	nr	__NR_select				0x0017			0x0fff			0x200005d		0x005d			0x005d			0x0047			0x1a1			0xfff
syscon	nr	__NR_pselect				0xfff			0x0fff			0x200018a		0x018a			0x020a			0x006e			0x1b4			0xfff
syscon	nr	__NR_pselect6				0x010e			0x0048			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_yield			0x0018			0x007c			0x2000197		0x005d			0x014b			0x012a			0x15e			0xfff			# select_nocancel() on XNU (previously swtch() but removed in 12.4)
syscon	nr	__NR_mremap				0x0019			0x00d8			0xfff			0xfff			0xfff			0xfff			0x19b			0xfff
syscon	nr	__NR_mincore				0x001b			0x00e8			0x200004e		0x004e			0x004e			0x004e			0x04e			0xfff
syscon	nr	__NR_madvise				0x001c			0x00e9			0x200004b		0x004b			0x004b			0x004b			0x04b			0xfff
syscon	nr	__NR_shmget				0x001d			0x00c2			0x2000109		0x0109			0x00e7			0x0121			0x0e7			0xfff
syscon	nr	__NR_shmat				0x001e			0x00c4			0x2000106		0x0106			0x00e4			0x00e4			0x0e4			0xfff
syscon	nr	__NR_shmctl				0x001f			0x00c3			0x2000107		0x0107			0x0200			0x0128			0x1bb			0xfff
syscon	nr	__NR_dup				0x0020			0x0017			0x2000029		0x0029			0x0029			0x0029			0x029			0xfff
syscon	nr	__NR_dup2				0x0021			0x0fff			0x200005a		0x005a			0x005a			0x005a			0x05a			0xfff
syscon	nr	__NR_pause				0x0022			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_nanosleep				0x0023			0x0065			0xfff			0xfff			0x00f0			0x005b			0x1ae			0xfff
syscon	nr	__NR_getitimer				0x0024			0x0066			0x2000056		0x0056			0x0056			0x0046			0x1aa			0xfff
syscon	nr	__NR_setitimer				0x0026			0x0067			0x2000053		0x0053			0x0053			0x0045			0x1a9			0xfff
syscon	nr	__NR_alarm				0x0025			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_getpid				0x0027			0x00ac			0x2000014		0x0014			0x0014			0x0014			0x014			0xfff
syscon	nr	__NR_sendfile				0x0028			0x0047			0x2000151		0x0151			0x0189			0xfff			0xfff			0xfff
syscon	nr	__NR_socket				0x0029			0x00c6			0x2000061		0x0061			0x0061			0x0061			0x18a			0xfff
syscon	nr	__NR_connect				0x002a			0x00cb			0x2000062		0x0062			0x0062			0x0062			0x062			0xfff
syscon	nr	__NR_accept				0x002b			0x00ca			0x200001e		0x001e			0x0063			0x001e			0x01e			0xfff
syscon	nr	__NR_sendto				0x002c			0x00ce			0x2000085		0x0085			0x0085			0x0085			0x085			0xfff
syscon	nr	__NR_recvfrom				0x002d			0x00cf			0x200001d		0x001d			0x001d			0x001d			0x01d			0xfff
syscon	nr	__NR_sendmsg				0x002e			0x00d3			0x200001c		0x001c			0x001c			0x001c			0x01c			0xfff
syscon	nr	__NR_recvmsg				0x002f			0x00d4			0x200001b		0x001b			0x001b			0x001b			0x01b			0xfff
syscon	nr	__NR_shutdown				0x0030			0x00d2			0x2000086		0x0086			0x0086			0x0086			0x086			0xfff
syscon	nr	__NR_bind				0x0031			0x00c8			0x2000068		0x0068			0x0068			0x0068			0x068			0xfff
syscon	nr	__NR_listen				0x0032			0x00c9			0x200006a		0x006a			0x006a			0x006a			0x06a			0xfff
syscon	nr	__NR_getsockname			0x0033			0x00cc			0x2000020		0x0020			0x0020			0x0020			0x020			0xfff
syscon	nr	__NR_getpeername			0x0034			0x00cd			0x200001f		0x001f			0x008d			0x001f			0x01f			0xfff
syscon	nr	__NR_socketpair				0x0035			0x00c7			0x2000087		0x0087			0x0087			0x0087			0x087			0xfff
syscon	nr	__NR_setsockopt				0x0036			0x00d0			0x2000069		0x0069			0x0069			0x0069			0x069			0xfff
syscon	nr	__NR_getsockopt				0x0037			0x00d1			0x2000076		0x0076			0x0076			0x0076			0x076			0xfff
syscon	nr	__NR_fork				0x0039			0x0fff			0x2000002		0x0002			0x0002			0x0002			0x002			0xfff
syscon	nr	__NR_vfork				0x003a			0x0fff			0x2000042		0x0042			0x0042			0x0042          	0x042           	0xfff
syscon	nr	__NR_posix_spawn			0xfff			0x0fff			0x20000f4		0x00f4			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_execve				0x003b			0x00dd			0x200003b		0x003b			0x003b			0x003b			0x03b			0xfff	# DING DING DING DING DING
syscon	nr	__NR_wait4				0x003d			0x0104			0x2000007		0x0007			0x0007			0x000b			0x1c1			0xfff
syscon	nr	__NR_kill				0x003e			0x0081			0x2000025		0x0025			0x0025			0x007a			0x025			0xfff
syscon	nr	__NR_killpg				0xfff			0x0fff			0xfff			0xfff			0x0092			0xfff			0xfff			0xfff
syscon	nr	__NR_clone				0x0038			0x00dc			0xfff			0xfff			0xfff			0xfff			0x11f			0xfff
syscon	nr	__NR_tkill				0x00c8			0x0082			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_futex				0x00ca			0x0062			0xfff			0xfff			0x1c6			0x0053			0x0a6			0xfff
syscon	nr	__NR_set_robust_list			0x0111			0x0063			0xfff			0xfff			0xfff			0xfff			0x0a7			0xfff
syscon	nr	__NR_get_robust_list			0x0112			0x0064			0xfff			0xfff			0xfff			0xfff			0x0a8			0xfff
syscon	nr	__NR_uname				0x003f			0x00a0			0xfff			0xfff			0x00a4			0xfff			0xfff			0xfff
syscon	nr	__NR_semget				0x0040			0x00be			0x20000ff		0x00ff			0x00dd			0x00dd			0x0dd			0xfff
syscon	nr	__NR_semop				0x0041			0x00c1			0x2000100		0x0100			0x00de			0x0122			0x0de			0xfff
syscon	nr	__NR_semctl				0x0042			0x00bf			0x20000fe		0x00fe			0x01fe			0x0127			0xfff			0xfff
syscon	nr	__NR_shmdt				0x0043			0x00c5			0x2000108		0x0108			0x00e6			0x00e6			0x0e6			0xfff
syscon	nr	__NR_msgget				0x0044			0x00ba			0x2000103		0x0103			0x00e1			0x00e1			0x0e1			0xfff
syscon	nr	__NR_msgsnd				0x0045			0x00bd			0x2000104		0x0104			0x00e2			0x00e2			0x0e2			0xfff
syscon	nr	__NR_msgrcv				0x0046			0x00bc			0x2000105		0x0105			0x00e3			0x00e3			0x0e3			0xfff
syscon	nr	__NR_msgctl				0x0047			0x00bb			0x2000102		0x0102			0x01ff			0x0129			0x1bc			0xfff
syscon	nr	__NR_fcntl				0x0048			0x0019			0x200005c		0x005c			0x005c			0x005c			0x05c			0xfff
syscon	nr	__NR_flock				0x0049			0x0020			0x2000083		0x0083			0x0083			0x0083			0x083			0xfff
syscon	nr	__NR_fsync				0x004a			0x0052			0x200005f		0x005f			0x005f			0x005f			0x05f			0xfff
syscon	nr	__NR_fdatasync				0x004b			0x0053			0x20000bb		0x00bb			0x0226			0x005f			0x0f1			0xfff
syscon	nr	__NR_truncate				0x004c			0x002d			0x20000c8		0x00c8			0x01df			0x00a7			0x0c8			0xfff			# OpenBSD 7.3+
syscon	nr	__NR_ftruncate				0x004d			0x002e			0x20000c9		0x00c9			0x01e0			0x00a8			0x0c9			0xfff			# OpenBSD 7.3+
syscon	nr	__NR_getcwd				0x004f			0x0011			0xfff			0xfff			0x0146			0x0130			0x128			0xfff
syscon	nr	__NR_chdir				0x0050			0x0031			0x200000c		0x000c			0x000c			0x000c			0x00c			0xfff
syscon	nr	__NR_fchdir				0x0051			0x0032			0x200000d		0x000d			0x000d			0x000d			0x00d			0xfff
syscon	nr	__NR_rename				0x0052			0x0fff			0x2000080		0x0080			0x0080			0x0080			0x080			0xfff
syscon	nr	__NR_mkdir				0x0053			0x0fff			0x2000088		0x0088			0x0088			0x0088			0x088			0xfff
syscon	nr	__NR_rmdir				0x0054			0x0fff			0x2000089		0x0089			0x0089			0x0089			0x089			0xfff
syscon	nr	__NR_creat				0x0055			0x0fff			0xfff			0xfff			0x0008			0xfff			0x008			0xfff
syscon	nr	__NR_link				0x0056			0x0fff			0x2000009		0x0009			0x0009			0x0009			0x009			0xfff
syscon	nr	__NR_unlink				0x0057			0x00b5			0x200000a		0x000a			0x000a			0x000a			0x00a			0xfff
syscon	nr	__NR_symlink				0x0058			0x0fff			0x2000039		0x0039			0x0039			0x0039			0x039			0xfff
syscon	nr	__NR_readlink				0x0059			0x0fff			0x200003a		0x003a			0x003a			0x003a			0x03a			0xfff
syscon	nr	__NR_chmod				0x005a			0x0fff			0x200000f		0x000f			0x000f			0x000f			0x00f			0xfff
syscon	nr	__NR_fchmod				0x005b			0x0034			0x200007c		0x007c			0x007c			0x007c			0x07c			0xfff
syscon	nr	__NR_chown				0x005c			0x0fff			0x2000010		0x0010			0x0010			0x0010			0x010			0xfff
syscon	nr	__NR_fchown				0x005d			0x0037			0x200007b		0x007b			0x007b			0x007b			0x07b			0xfff
syscon	nr	__NR_lchown				0x005e			0x0fff			0x200016c		0x016c			0x00fe			0x00fe			0x113			0xfff
syscon	nr	__NR_umask				0x005f			0x00a6			0x200003c		0x003c			0x003c			0x003c			0x03c			0xfff
syscon	nr	__NR_gettimeofday			0x0060			0x00a9			0x2000074		0x0074			0x0074			0x0043			0x1a2			0xfff
syscon	nr	__NR_getrlimit				0x0061			0x00a3			0x20000c2		0x00c2			0x00c2			0x00c2			0x0c2			0xfff
syscon	nr	__NR_getrusage				0x0062			0x00a5			0x2000075		0x0075			0x0075			0x0013			0x1bd			0xfff
syscon	nr	__NR_sysinfo				0x0063			0x00b3			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_times				0x0064			0x0099			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_ptrace				0x0065			0x0075			0x200001a		0x001a			0x001a			0x001a			0x01a			0xfff
syscon	nr	__NR_syslog				0x0067			0x0074			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_getuid				0x0066			0x00ae			0x2000018		0x0018			0x0018			0x0018			0x018			0xfff
syscon	nr	__NR_getgid				0x0068			0x00b0			0x200002f		0x002f			0x002f			0x002f			0x02f			0xfff
syscon	nr	__NR_getppid				0x006e			0x00ad			0x2000027		0x0027			0x0027			0x0027			0xfff			0xfff
syscon	nr	__NR_getpgrp				0x006f			0x0fff			0x2000051		0x0051			0x0051			0x0051			0x051			0xfff
syscon	nr	__NR_setsid				0x0070			0x009d			0x2000093		0x0093			0x0093			0x0093			0x093			0xfff
syscon	nr	__NR_getsid				0x007c			0x009c			0x2000136		0x0136			0x0136			0x00ff			0x11e			0xfff
syscon	nr	__NR_getpgid				0x0079			0x009b			0x2000097		0x0097			0x00cf			0x00cf			0x0cf			0xfff
syscon	nr	__NR_setpgid				0x006d			0x009a			0x2000052		0x0052			0x0052			0x0052			0x052			0xfff
syscon	nr	__NR_geteuid				0x006b			0x00af			0x2000019		0x0019			0x0019			0x0019			0xfff			0xfff
syscon	nr	__NR_getegid				0x006c			0x00b1			0x200002b		0x002b			0x002b			0x002b			0xfff			0xfff
syscon	nr	__NR_getgroups				0x0073			0x009e			0x200004f		0x004f			0x004f			0x004f			0x04f			0xfff
syscon	nr	__NR_setgroups				0x0074			0x009f			0x2000050		0x0050			0x0050			0x0050			0x050			0xfff
syscon	nr	__NR_setreuid				0x0071			0x0091			0x200007e		0x007e			0x007e			0x007e			0x07e			0xfff
syscon	nr	__NR_setregid				0x0072			0x008f			0x200007f		0x007f			0x007f			0x007f			0x07f			0xfff
syscon	nr	__NR_setuid				0x0069			0x0092			0x2000017		0x0017			0x0017			0x0017			0x017			0xfff
syscon	nr	__NR_setgid				0x006a			0x0090			0x20000b5		0x00b5			0x00b5			0x00b5			0x0b5			0xfff
syscon	nr	__NR_setresuid				0x0075			0x0093			0xfff			0xfff			0x0137			0x011a			0xfff			0xfff
syscon	nr	__NR_setresgid				0x0077			0x0095			0xfff			0xfff			0x0138			0x011c			0xfff			0xfff
syscon	nr	__NR_getresuid				0x0076			0x0094			0xfff			0xfff			0x0168			0x0119			0xfff			0xfff
syscon	nr	__NR_getresgid				0x0078			0x0096			0xfff			0xfff			0x0169			0x011b			0xfff			0xfff
syscon	nr	__NR_sigpending				0x007f			0x0088			0x2000034		0x0034			0x0034			0x0034			0x124			0xfff
syscon	nr	__NR_sigsuspend				0x0082			0x0085			0x200006f		0x006f			0x0155			0x006f			0x126			0xfff
syscon	nr	__NR_sigaltstack			0x0083			0x0084			0x2000035		0x0035			0x0035			0x0120			0x119			0xfff
syscon	nr	__NR_mknod				0x0085			0x0fff			0x200000e		0x000e			0x000e			0x000e			0x1c2			0xfff
syscon	nr	__NR_mknodat				0x0103			0x0021			0xfff			0xfff			0x22ff			0x0140			0x1cc			0xfff
syscon	nr	__NR_mkfifo				0xfff			0x0fff			0x2000084		0x0084			0x0084			0x0084			0x084			0xfff
syscon	nr	__NR_mkfifoat				0xfff			0x0fff			0xfff			0xfff			0x01f1			0x013f			0x1cb			0xfff
syscon	nr	__NR_statfs				0x0089			0x002b			0x2000159		0x0159			0x022b			0x003f			0xfff			0xfff
syscon	nr	__NR_fstatfs				0x008a			0x002c			0x200015a		0x015a			0x022c			0x0040			0xfff			0xfff
syscon	nr	__NR_getpriority			0x008c			0x008d			0x2000064		0x0064			0x0064			0x0064			0x064			0xfff
syscon	nr	__NR_setpriority			0x008d			0x008c			0x2000060		0x0060			0x0060			0x0060			0x060			0xfff
syscon	nr	__NR_mlock				0x0095			0x00e4			0x20000cb		0x00cb			0x00cb			0x00cb			0x0cb			0xfff
syscon	nr	__NR_munlock				0x0096			0x00e5			0x20000cc		0x00cc			0x00cc			0x00cc			0x0cc			0xfff
syscon	nr	__NR_mlockall				0x0097			0x00e6			0x2000144		0x0144			0x0144			0x010f			0x0f2			0xfff
syscon	nr	__NR_munlockall				0x0098			0x00e7			0x2000145		0x0145			0x0145			0x0110			0x0f3			0xfff
syscon	nr	__NR_setrlimit				0x00a0			0x00a4			0x20000c3		0x00c3			0x00c3			0x00c3			0x0c3			0xfff
syscon	nr	__NR_chroot				0x00a1			0x0033			0x200003d		0x003d			0x003d			0x003d			0x03d			0xfff
syscon	nr	__NR_sync				0x00a2			0x0051			0x2000024		0x0024			0x0024			0x0024			0x024			0xfff
syscon	nr	__NR_acct				0x00a3			0x0059			0x2000033		0x0033			0x0033			0x0033			0x033			0xfff
syscon	nr	__NR_settimeofday			0x00a4			0x00aa			0x200007a		0x007a			0x007a			0x0044			0x1a3			0xfff
syscon	nr	__NR_mount				0x00a5			0x0028			0x20000a7		0x00a7			0x0015			0x0015			0x19a			0xfff
syscon	nr	__NR_reboot				0x00a9			0x008e			0x2000037		0x0037			0x0037			0x0037			0x0d0			0xfff
syscon	nr	__NR_quotactl				0x00b3			0x003c			0x20000a5		0x00a5			0x0094			0x0094			0xfff			0xfff
syscon	nr	__NR_setfsuid				0x007a			0x0097			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_setfsgid				0x007b			0x0098			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_capget				0x007d			0x005a			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_capset				0x007e			0x005b			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_sigtimedwait			0x0080			0x0089			0xfff			0xfff			0x0159			0xfff			0xfff			0xfff
syscon	nr	__NR_sigqueueinfo			0x0081			0x008a			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_personality			0x0087			0x005c			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_ustat				0x0088			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_sysfs				0x008b			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_setparam			0x008e			0x0076			0xfff			0xfff			0x0147			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_getparam			0x008f			0x0079			0xfff			0xfff			0x0148			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_setscheduler			0x0090			0x0077			0xfff			0xfff			0x0149			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_getscheduler			0x0091			0x0078			0xfff			0xfff			0x014a			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_get_priority_max		0x0092			0x007d			0xfff			0xfff			0x014c			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_get_priority_min		0x0093			0x007e			0xfff			0xfff			0x014d			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_rr_get_interval		0x0094			0x007f			0xfff			0xfff			0x014e			0xfff			0xfff			0xfff
syscon	nr	__NR_vhangup				0x0099			0x003a			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_modify_ldt				0x009a			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_pivot_root				0x009b			0x0029			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR__sysctl				0x009c			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_prctl				0x009d			0x00a7			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_arch_prctl				0x009e			0x0fff			0x3000003		0xfff			0x00a5			0x149			0x13d			0xfff		# sysarch on freebsd, _lwp_setprivate on netbsd, __set_tcb on openbsd, thread_fast_set_cthread_self on xnu
syscon	nr	__NR_adjtimex				0x009f			0x00ab			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_umount2				0x00a6			0x0027			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_swapon				0x00a7			0x00e0			0x2000055		0x0055			0x0055			0xfff			0xfff			0xfff
syscon	nr	__NR_swapoff				0x00a8			0x00e1			0xfff			0xfff			0x01a8			0xfff			0xfff			0xfff
syscon	nr	__NR_sethostname			0x00aa			0x00a1			0xfff			0xfff			0x0058			0xfff			0xfff			0xfff
syscon	nr	__NR_setdomainname			0x00ab			0x00a2			0xfff			0xfff			0x00a3			0xfff			0xfff			0xfff
syscon	nr	__NR_iopl				0x00ac			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_ioperm				0x00ad			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_init_module			0x00af			0x0069			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_delete_module			0x00b0			0x006a			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_gettid				0x00ba			0x00b2			0x100001b		0xfff			432			299			311			0xfff		# TODO(jart): thread_self_trap vs. gettid?
syscon	nr	__NR_set_tls				0x009e			0xfff			0x3000003		0xfff			0x00a5			0x0149			0x13d			0xfff
syscon	nr	__NR_readahead				0x00bb			0x00d5			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_setxattr				0x00bc			0x0005			0x20000ec		0x00ec			0xfff			0xfff			0x177			0xfff
syscon	nr	__NR_fsetxattr				0x00be			0x0007			0x20000ed		0x00ed			0xfff			0xfff			0x179			0xfff
syscon	nr	__NR_getxattr				0x00bf			0x0008			0x20000ea		0x00ea			0xfff			0xfff			0x17a			0xfff
syscon	nr	__NR_fgetxattr				0x00c1			0x000a			0x20000eb		0x00eb			0xfff			0xfff			0x17c			0xfff
syscon	nr	__NR_listxattr				0x00c2			0x000b			0x20000f0		0x00f0			0xfff			0xfff			0x17d			0xfff
syscon	nr	__NR_flistxattr				0x00c4			0x000d			0x20000f1		0x00f1			0xfff			0xfff			0x17f			0xfff
syscon	nr	__NR_removexattr			0x00c5			0x000e			0x20000ee		0x00ee			0xfff			0xfff			0x180			0xfff
syscon	nr	__NR_fremovexattr			0x00c7			0x0010			0x20000ef		0x00ef			0xfff			0xfff			0x182			0xfff
syscon	nr	__NR_lsetxattr				0x00bd			0x0006			0xfff			0xfff			0xfff			0xfff			0x178			0xfff
syscon	nr	__NR_lgetxattr				0x00c0			0x0009			0xfff			0xfff			0xfff			0xfff			0x17b			0xfff
syscon	nr	__NR_llistxattr				0x00c3			0x000c			0xfff			0xfff			0xfff			0xfff			0x17e			0xfff
syscon	nr	__NR_lremovexattr			0x00c6			0x000f			0xfff			0xfff			0xfff			0xfff			0x181			0xfff
syscon	nr	__NR_sched_setaffinity			0x00cb			0x007a			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_getaffinity			0x00cc			0x007b			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_cpuset_getaffinity			0xfff			0xfff			0xfff			0xfff			0x01e7			0xfff			0xfff			0xfff
syscon	nr	__NR_cpuset_setaffinity			0xfff			0xfff			0xfff			0xfff			0x01e8			0xfff			0xfff			0xfff
syscon	nr	__NR_io_setup				0x00ce			0x0000			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_io_destroy				0x00cf			0x0001			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_io_getevents			0x00d0			0x0004			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_io_submit				0x00d1			0x0002			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_io_cancel				0x00d2			0x0003			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_lookup_dcookie			0x00d4			0x0012			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_epoll_create			0x00d5			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_epoll_wait				0x00e8			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_epoll_ctl				0x00e9			0x0015			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_getdents				0x00d9			0x003d			0xfff			0xfff			0x0110			0x0063			0x186			0xfff
syscon	nr	__NR_set_tid_address			0x00da			0x0060			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_restart_syscall			0x00db			0x0080			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_semtimedop				0x00dc			0x00c0			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_fadvise				0x00dd			0x00df			0xfff			0xfff			0x0213			0xfff			0xfff			0xfff
syscon	nr	__NR_timer_create			0x00de			0x006b			0xfff			0xfff			0xfff			0xfff			0x0eb			0xfff
syscon	nr	__NR_timer_settime			0x00df			0x006e			0xfff			0xfff			0xfff			0xfff			0x1be			0xfff
syscon	nr	__NR_timer_gettime			0x00e0			0x006c			0xfff			0xfff			0xfff			0xfff			0x1bf			0xfff
syscon	nr	__NR_timer_getoverrun			0x00e1			0x006d			0xfff			0xfff			0xfff			0xfff			0x0ef			0xfff
syscon	nr	__NR_timer_delete			0x00e2			0x006f			0xfff			0xfff			0xfff			0xfff			0x0ec			0xfff
syscon	nr	__NR_clock_settime			0x00e3			0x0070			0xfff			0xfff			0x00e9			0x0058			0x1ac			0xfff
syscon	nr	__NR_clock_gettime			0x00e4			0x0071			0xfff			0xfff			0x00e8			0x0057			0x1ab			0xfff
syscon	nr	__NR_clock_getres			0x00e5			0x0072			0xfff			0xfff			0x00ea			0x0059			0x1ad			0xfff
syscon	nr	__NR_clock_nanosleep			0x00e6			0x0073			0xfff			0xfff			0x00f4			0xfff			0x1dd			0xfff
syscon	nr	__NR_tgkill				0x00ea			0x0083			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_mbind				0x00ed			0x00eb			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_set_mempolicy			0x00ee			0x00ed			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_get_mempolicy			0x00ef			0x00ec			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_mq_open				0x00f0			0x00b4			0xfff			0xfff			0xfff			0xfff			0x101			0xfff
syscon	nr	__NR_mq_unlink				0x00f1			0x00b5			0xfff			0xfff			0xfff			0xfff			0x103			0xfff
syscon	nr	__NR_mq_timedsend			0x00f2			0x00b6			0xfff			0xfff			0xfff			0xfff			0x1b0			0xfff
syscon	nr	__NR_mq_timedreceive			0x00f3			0x00b7			0xfff			0xfff			0xfff			0xfff			0x1b1			0xfff
syscon	nr	__NR_mq_notify				0x00f4			0x00b8			0xfff			0xfff			0xfff			0xfff			0x106			0xfff
syscon	nr	__NR_mq_getsetattr			0x00f5			0x00b9			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_kexec_load				0x00f6			0x0068			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_waitid				0x00f7			0x005f			0x20000ad		0x00ad			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_add_key				0x00f8			0x00d9			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_request_key			0x00f9			0x00da			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_keyctl				0x00fa			0x00db			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_ioprio_set				0x00fb			0x001e			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_ioprio_get				0x00fc			0x001f			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_inotify_init			0x00fd			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_inotify_add_watch			0x00fe			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_inotify_rm_watch			0x00ff			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_openat				0x0101			0x0038			0x20001d0		0x01cf			0x01f3			0x0141			0x1d4			0xfff			# openat_nocancel() on XNU
syscon	nr	__NR_mkdirat				0x0102			0x0022			0x20001db		0x01db			0x01f0			0x013e			0x1cd			0xfff
syscon	nr	__NR_fchownat				0x0104			0x0036			0x20001d4		0x01d4			0x01eb			0x013b			0x1d0			0xfff
syscon	nr	__NR_utime				0x0084			0x0062			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_utimes				0x00eb			0x0058			0x200008a		0x008a			0x008a			0x004c			0x1a4			0xfff
syscon	nr	__NR_futimesat				0x0105			0x0fff			0xfff			0xfff			0x01ee			0xfff			0xfff			0xfff
syscon	nr	__NR_futimes				0xfff			0x0fff			0x200008b		0x008b			0x00ce			0x004d			0x1a7			0xfff
syscon	nr	__NR_futimens				0xfff			0x0fff			0xfff			0xfff			0x0222			0x0055			0x1d8			0xfff
syscon	nr	__NR_fstatat				0x0106			0x004f			0x20001d6		0x01d6			0x0228			0x002a			0x1d2			0xfff
syscon	nr	__NR_unlinkat				0x0107			0x0023			0x20001d8		0x01d8			0x01f7			0x0145			0x1d7			0xfff
syscon	nr	__NR_renameat				0x0108			0x0026			0x20001d1		0x01d1			0x01f5			0x0143			0x1ca			0xfff
syscon	nr	__NR_linkat				0x0109			0x0025			0x20001d7		0x01d7			0x01ef			0x013d			0x1c9			0xfff
syscon	nr	__NR_symlinkat				0x010a			0x0024			0x20001da		0x01da			0x01f6			0x0144			0x1d6			0xfff
syscon	nr	__NR_readlinkat				0x010b			0x004e			0x20001d9		0x01d9			0x01f4			0x0142			0x1d5			0xfff
syscon	nr	__NR_fchmodat				0x010c			0x0035			0x20001d3		0x01d3			0x01ea			0x013a			0x1cf			0xfff
syscon	nr	__NR_faccessat				0x010d			0x0030			0x20001d2		0x01d2			0x01e9			0x0139			0x1ce			0xfff
syscon	nr	__NR_unshare				0x0110			0x0061			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_splice				0x0113			0x004c			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_tee				0x0114			0x004d			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_vmsplice				0x0116			0x004b			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_migrate_pages			0x0100			0x00ee			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_move_pages				0x0117			0x00ef			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_preadv				0x0127			0x0045			0xfff			0xfff			0x0121			0x00ab			0x121			0xfff			# OpenBSD 7.3+
syscon	nr	__NR_pwritev				0x0128			0x0046			0xfff			0xfff			0x0122			0x00aa			0x122			0xfff			# OpenBSD 7.3+
syscon	nr	__NR_utimensat				0x0118			0x0058			0xfff			0xfff			0x0223			0x0054			0x1d3			0xfff
syscon	nr	__NR_fallocate				0x011d			0x002f			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_posix_fallocate			0xfff			0xfff			0xfff			0xfff			0x0212			0xfff			0x1df			0xfff
syscon	nr	__NR_accept4				0x0120			0x00f2			0xfff			0xfff			0x021d			0x005d			0xfff			0xfff
syscon	nr	__NR_dup3				0x0124			0x0018			0xfff			0xfff			0xfff			0x0066			0x1c6			0xfff
syscon	nr	__NR_pipe2				0x0125			0x003b			0xfff			0xfff			0x021e			0x0065			0x1c5			0xfff
syscon	nr	__NR_epoll_pwait			0x0119			0x0016			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_epoll_create1			0x0123			0x0014			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_perf_event_open			0x012a			0x00f1			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_inotify_init1			0x0126			0x001a			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_tgsigqueueinfo			0x0129			0x00f0			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_signalfd				0x011a			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_signalfd4				0x0121			0x004a			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_eventfd				0x011c			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_eventfd2				0x0122			0x0013			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_timerfd_create			0x011b			0x0055			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_timerfd_settime			0x011e			0x0056			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_timerfd_gettime			0x011f			0x0057			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_recvmmsg				0x012b			0x00f3			0xfff			0xfff			0xfff			0xfff			0x1db			0xfff
syscon	nr	__NR_fanotify_init			0x012c			0x0106			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_fanotify_mark			0x012d			0x0107			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_prlimit				0x012e			0x0105			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_name_to_handle_at			0x012f			0x0108			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_open_by_handle_at			0x0130			0x0109			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_clock_adjtime			0x0131			0x010a			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_syncfs				0x0132			0x010b			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_sendmmsg				0x0133			0x010d			0xfff			0xfff			0xfff			0xfff			0x1dc			0xfff
syscon	nr	__NR_setns				0x0134			0x010c			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_getcpu				0x0135			0x00a8			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_process_vm_readv			0x0136			0x010e			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_process_vm_writev			0x0137			0x010f			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_kcmp				0x0138			0x0110			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_finit_module			0x0139			0x0111			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_setattr			0x013a			0x0112			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_sched_getattr			0x013b			0x0113			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_renameat2				0x013c			0x0114			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_seccomp				0x013d			0x0115			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_getrandom				0x013e			0x0116			0x20001f4		0x01f4			0x0233			0x0007			0x05b			0xfff
syscon	nr	__NR_memfd_create			0x013f			0x0117			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_kexec_file_load			0x0140			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_bpf				0x0141			0x0118			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_execveat				0x0142			0x0119			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_userfaultfd			0x0143			0x011a			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_membarrier				0x0144			0x011b			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_mlock2				0x0145			0x011c			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_copy_file_range			0x0146			0x011d			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_preadv2				0x0147			0x011e			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_pwritev2				0x0148			0x011f			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_pkey_mprotect			0x0149			0x0120			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_pkey_alloc				0x014a			0x0121			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_pkey_free				0x014b			0x0122			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_statx				0x014c			0x0123			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_io_pgetevents			0x014d			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_rseq				0x014e			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_pidfd_send_signal			0x01a8			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_io_uring_setup			0x01a9			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_io_uring_enter			0x01aa			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_io_uring_register			0x01ab			0x0fff			0xfff			0xfff			0xfff			0xfff			0xfff			0xfff
syscon	nr	__NR_pledge				0xfff			0xfff			0xfff			0xfff			0xfff			0x006c			0xfff			0xfff
syscon	nr	__NR_msyscall				0xfff			0xfff			0xfff			0xfff			0xfff			0x0025			0xfff			0xfff
syscon	nr	__NR_ktrace				0xfff			0xfff			0xfff			0xfff			0x002d			0x002d			0x02d			0xfff

#	unilateral undocumented errnos
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	junkerr	ECHRNG					44			44			-1			-1			-1			-1			-1			-1
syscon	junkerr	EL2NSYNC				45			45			-1			-1			-1			-1			-1			-1
syscon	junkerr	EL3HLT					46			46			-1			-1			-1			-1			-1			-1
syscon	junkerr	EL3RST					47			47			-1			-1			-1			-1			-1			-1
syscon	junkerr	ELNRNG					48			48			-1			-1			-1			-1			-1			-1
syscon	junkerr	EUNATCH					49			49			-1			-1			-1			-1			-1			-1
syscon	junkerr	ENOCSI					50			50			-1			-1			-1			-1			-1			-1
syscon	junkerr	EL2HLT					51			51			-1			-1			-1			-1			-1			-1
syscon	junkerr	EBADE					52			52			-1			-1			-1			-1			-1			-1
syscon	junkerr	EBADR					53			53			-1			-1			-1			-1			-1			-1
syscon	junkerr	EXFULL					54			54			-1			-1			-1			-1			-1			-1
syscon	junkerr	ENOANO					55			55			-1			-1			-1			-1			-1			-1
syscon	junkerr	EBADRQC					56			56			-1			-1			-1			-1			-1			-1
syscon	junkerr	EBADSLT					57			57			-1			-1			-1			-1			-1			-1
syscon	junkerr	ENOPKG					65			65			-1			-1			-1			-1			-1			-1
syscon	junkerr	EADV					68			68			-1			-1			-1			-1			-1			-1
syscon	junkerr	ESRMNT					69			69			-1			-1			-1			-1			-1			-1
syscon	junkerr	ECOMM					70			70			-1			-1			-1			-1			-1			-1
syscon	junkerr	EDOTDOT					73			73			-1			-1			-1			-1			-1			-1
syscon	junkerr	ENOTUNIQ				76			76			-1			-1			-1			-1			-1			-1
syscon	junkerr	EREMCHG					78			78			-1			-1			-1			-1			-1			-1
syscon	junkerr	ELIBACC					79			79			-1			-1			-1			-1			-1			-1
syscon	junkerr	ELIBBAD					80			80			-1			-1			-1			-1			-1			-1
syscon	junkerr	ELIBSCN					81			81			-1			-1			-1			-1			-1			-1
syscon	junkerr	ELIBMAX					82			82			-1			-1			-1			-1			-1			-1
syscon	junkerr	ELIBEXEC				83			83			-1			-1			-1			-1			-1			-1
syscon	junkerr	ESTRPIPE				86			86			-1			-1			-1			-1			-1			-1
syscon	junkerr	EUCLEAN					117			117			-1			-1			-1			-1			-1			-1
syscon	junkerr	ENOTNAM					118			118			-1			-1			-1			-1			-1			-1
syscon	junkerr	ENAVAIL					119			119			-1			-1			-1			-1			-1			-1
syscon	junkerr	EISNAM					120			120			-1			-1			-1			-1			-1			-1
syscon	junkerr	EREMOTEIO				121			121			-1			-1			-1			-1			-1			-1
syscon	junkerr	ENOKEY					126			126			-1			-1			-1			-1			-1			-1
syscon	junkerr	EKEYEXPIRED				127			127			-1			-1			-1			-1			-1			-1
syscon	junkerr	EKEYREVOKED				128			128			-1			-1			-1			-1			-1			-1
syscon	junkerr	EKEYREJECTED				129			129			-1			-1			-1			-1			-1			-1
syscon	junkerr	ERFKILL					132			132			-1			-1			-1			-1			-1			-1
syscon	junkerr	EHWPOISON				133			133			-1			-1			-1			-1			-1			-1

#	arpanet fork combating human-induced exhaustion of our ipv4 address space
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	ipv6	IPV6_PMTUDISC_DONT			0			0			0			0			0			0			0			0			# consensus
syscon	ipv6	IPV6_RTHDR_LOOSE			0			0			0			0			0			0			0			0			# consensus
syscon	ipv6	IPV6_RTHDR_TYPE_0			0			0			0			0			0			0			0			0			# consensus
syscon	ipv6	IPV6_CHECKSUM				7			7			26			26			26			26			26			26			# bsd consensus
syscon	ipv6	IPV6_JOIN_GROUP				20			20			12			12			12			12			12			12			# bsd consensus
syscon	ipv6	IPV6_LEAVE_GROUP			21			21			13			13			13			13			13			13			# bsd consensus
syscon	ipv6	IPV6_MULTICAST_HOPS			18			18			10			10			10			10			10			10			# bsd consensus
syscon	ipv6	IPV6_MULTICAST_IF			17			17			9			9			9			9			9			9			# bsd consensus
syscon	ipv6	IPV6_MULTICAST_LOOP			19			19			11			11			11			11			11			11			# bsd consensus
syscon	ipv6	IPV6_UNICAST_HOPS			0x10			0x10			4			4			4			4			4			4			# bsd consensus
syscon	ipv6	IPV6_V6ONLY				26			26			27			27			27			27			27			27			# bsd consensus
syscon	ipv6	IPV6_RECVTCLASS				66			66			35			35			57			57			57			40
syscon	ipv6	IPV6_TCLASS				67			67			36			36			61			61			61			39
syscon	ipv6	IPV6_DONTFRAG				62			62			0			0			62			62			62			14
syscon	ipv6	IPV6_HOPLIMIT				52			52			0			0			47			47			47			21
syscon	ipv6	IPV6_HOPOPTS				54			54			0			0			49			49			49			1
syscon	ipv6	IPV6_PKTINFO				50			50			0			0			46			46			46			19
syscon	ipv6	IPV6_RECVRTHDR				56			56			0			0			38			38			38			38
syscon	ipv6	IPV6_RTHDR				57			57			0			0			51			51			51			0x20
syscon	ipv6	IPV6_DSTOPTS				59			59			0			0			50			50			50			0
syscon	ipv6	IPV6_IPSEC_POLICY			34			34			28			28			28			0			0			0
syscon	ipv6	IPV6_NEXTHOP				9			9			0			0			48			48			48			0
syscon	ipv6	IPV6_PATHMTU				61			61			0			0			44			44			44			0
syscon	ipv6	IPV6_RECVDSTOPTS			58			58			0			0			40			40			40			0
syscon	ipv6	IPV6_RECVHOPLIMIT			51			51			0			0			37			37			37			0
syscon	ipv6	IPV6_RECVHOPOPTS			53			53			0			0			39			39			39			0
syscon	ipv6	IPV6_RECVPATHMTU			60			60			0			0			43			43			43			0
syscon	ipv6	IPV6_RECVPKTINFO			49			49			0			0			36			36			36			0
syscon	ipv6	IPV6_RTHDRDSTOPTS			55			55			0			0			35			35			35			0
syscon	ipv6	IPV6_RTHDR_STRICT			1			1			1			1			1			0			0			0
syscon	ipv6	IPV6_ADD_MEMBERSHIP			20			20			0			0			0			0			0			12			# bsd consensus
syscon	ipv6	IPV6_DROP_MEMBERSHIP			21			21			0			0			0			0			0			13			# bsd consensus
syscon	ipv6	IPV6_HDRINCL				36			36			0			0			0			0			0			2			# bsd consensus
syscon	ipv6	IPV6_MTU				24			24			0			0			0			0			0			72			# bsd consensus
syscon	ipv6	IPV6_MTU_DISCOVER			23			23			0			0			0			0			0			71			# bsd consensus
syscon	ipv6	IPV6_RECVERR				25			25			0			0			0			0			0			75			# bsd consensus
syscon	ipv6	IPV6_2292DSTOPTS			4			4			23			23			0			0			0			0
syscon	ipv6	IPV6_2292HOPLIMIT			8			8			20			20			0			0			0			0
syscon	ipv6	IPV6_2292HOPOPTS			3			3			22			22			0			0			0			0
syscon	ipv6	IPV6_2292PKTINFO			2			2			19			19			0			0			0			0
syscon	ipv6	IPV6_2292PKTOPTIONS			6			6			25			25			0			0			0			0
syscon	ipv6	IPV6_2292RTHDR				5			5			24			24			0			0			0			0
syscon	ipv6	IPV6_AUTOFLOWLABEL			0			0			0			0			59			59			59			0
syscon	ipv6	IPV6_ADDRFORM				1			1			0			0			0			0			0			0
syscon	ipv6	IPV6_AUTHHDR				10			10			0			0			0			0			0			0
syscon	ipv6	IPV6_JOIN_ANYCAST			27			27			0			0			0			0			0			0
syscon	ipv6	IPV6_LEAVE_ANYCAST			28			28			0			0			0			0			0			0
syscon	ipv6	IPV6_PMTUDISC_DO			2			2			0			0			0			0			0			0
syscon	ipv6	IPV6_PMTUDISC_INTERFACE			4			4			0			0			0			0			0			0
syscon	ipv6	IPV6_PMTUDISC_OMIT			5			5			0			0			0			0			0			0
syscon	ipv6	IPV6_PMTUDISC_PROBE			3			3			0			0			0			0			0			0
syscon	ipv6	IPV6_PMTUDISC_WANT			1			1			0			0			0			0			0			0
syscon	ipv6	IPV6_ROUTER_ALERT			22			22			0			0			0			0			0			0
syscon	ipv6	IPV6_RXDSTOPTS				59			59			0			0			0			0			0			0
syscon	ipv6	IPV6_RXHOPOPTS				54			54			0			0			0			0			0			0
syscon	ipv6	IPV6_XFRM_POLICY			35			35			0			0			0			0			0			0
syscon	ipv6	IPV6_MINHOPCOUNT			0			0			0			0			0			65			65			0
syscon	ipv6	IPV6_ORIGDSTADDR			0			0			0			0			72			0			0			0
syscon	ipv6	IPV6_RECVORIGDSTADDR			0			0			0			0			72			0			0			0
syscon	ipv6	INET6_ADDRSTRLEN			46			46			46			46			46			46			46			65			# unix consensus
syscon	icmp6	ICMP6_DST_UNREACH_NOROUTE		0			0			0			0			0			0			0			0			# consensus
syscon	icmp6	ICMP6_PARAMPROB_HEADER			0			0			0			0			0			0			0			0			# consensus
syscon	icmp6	ICMP6_TIME_EXCEED_TRANSIT		0			0			0			0			0			0			0			0			# consensus
syscon	icmp6	ICMP6_DST_UNREACH_ADMIN			1			1			1			1			1			1			1			1			# consensus
syscon	icmp6	ICMP6_PARAMPROB_NEXTHEADER		1			1			1			1			1			1			1			1			# consensus
syscon	icmp6	ICMP6_TIME_EXCEED_REASSEMBLY		1			1			1			1			1			1			1			1			# consensus
syscon	icmp6	ICMP6_DST_UNREACH			1			1			1			1			1			1			1			0			# unix consensus
syscon	icmp6	ICMP6_FILTER				1			1			18			18			18			18			18			0			# bsd consensus
syscon	icmp6	ICMP6_DST_UNREACH_BEYONDSCOPE		2			2			2			2			2			2			2			2			# consensus
syscon	icmp6	ICMP6_PARAMPROB_OPTION			2			2			2			2			2			2			2			2			# consensus
syscon	icmp6	ICMP6_PACKET_TOO_BIG			2			2			2			2			2			2			2			0			# unix consensus
syscon	icmp6	ICMP6_DST_UNREACH_ADDR			3			3			3			3			3			3			3			3			# consensus
syscon	icmp6	ICMP6_TIME_EXCEEDED			3			3			3			3			3			3			3			0			# unix consensus
syscon	icmp6	ICMP6_DST_UNREACH_NOPORT		4			4			4			4			4			4			4			4			# consensus
syscon	icmp6	ICMP6_PARAM_PROB			4			4			4			4			4			4			4			0			# unix consensus
syscon	icmp6	ICMP6_RR_FLAGS_PREVDONE			8			8			8			8			8			8			8			0			# unix consensus
syscon	icmp6	ICMP6_RR_FLAGS_SPECSITE			0x10			0x10			0x10			0x10			0x10			0x10			0x10			0			# unix consensus
syscon	icmp6	ICMP6_RR_PCOUSE_RAFLAGS_AUTO		0x10			0x10			0x40			0x40			0x40			0x40			0x40			0			# bsd consensus
syscon	icmp6	ICMP6_RR_FLAGS_FORCEAPPLY		0x20			0x20			0x20			0x20			0x20			0x20			0x20			0			# unix consensus
syscon	icmp6	ICMP6_RR_PCOUSE_RAFLAGS_ONLINK		0x20			0x20			0x80			0x80			0x80			0x80			0x80			0			# bsd consensus
syscon	icmp6	ICMP6_RR_FLAGS_REQRESULT		0x40			0x40			0x40			0x40			0x40			0x40			0x40			0			# unix consensus
syscon	icmp6	ICMP6_RR_PCOUSE_FLAGS_DECRPLTIME	0x40			0x40			0x40			0x40			0x40			0x40			0x40			0			# unix consensus
syscon	icmp6	ICMP6_INFOMSG_MASK			0x80			0x80			0x80			0x80			0x80			0x80			0x80			0x80			# consensus
syscon	icmp6	ICMP6_ECHO_REQUEST			0x80			0x80			0x80			0x80			0x80			0x80			0x80			0			# unix consensus
syscon	icmp6	ICMP6_RR_FLAGS_TEST			0x80			0x80			0x80			0x80			0x80			0x80			0x80			0			# unix consensus
syscon	icmp6	ICMP6_RR_PCOUSE_FLAGS_DECRVLTIME	0x80			0x80			0x80			0x80			0x80			0x80			0x80			0			# unix consensus
syscon	icmp6	ICMP6_ECHO_REPLY			129			129			129			129			129			129			129			0			# unix consensus
syscon	icmp6	ICMP6_ROUTER_RENUMBERING		138			138			138			138			138			138			138			0			# unix consensus
syscon	icmp6	ICMP6_RR_RESULT_FLAGS_FORBIDDEN		0x0100			0x0100			0x0100			0x0100			0x0100			0x0100			0x0100			0			# unix consensus
syscon	icmp6	ICMP6_RR_RESULT_FLAGS_OOB		0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			0			# unix consensus

syscon	misc	FIFOTYPE				54			54			54			54			54			54			54			0			# unix consensus
syscon	misc	GRPQUOTA				1			1			1			1			1			1			1			0			# unix consensus
syscon	misc	IF_NAMESIZE				0x10			0x10			0x10			0x10			0x10			0x10			0x10			0			# unix consensus
syscon	misc	INTERMEDIATE_C_GOOD			10			10			0			0			0			0			0			0
syscon	misc	INTERMEDIATE_GOOD			8			8			0			0			0			0			0			0

syscon	misc	IOV_MAX					0x0400			0x0400			0x0400			0x0400			0x0400			0x0400			0x0400			16			# unix consensus & MSG_MAXIOVLEN
syscon	misc	LINE_MAX				0x0800			0x0800			0x0800			0x0800			0x0800			0x0800			0x0800			0			# unix consensus
syscon	misc	LINKED_CMD_COMPLETE			10			10			0			0			0			0			0			0
syscon	misc	LINKED_FLG_CMD_COMPLETE			11			11			0			0			0			0			0			0

syscon	misc	LNKTYPE					49			49			49			49			49			49			49			0			# unix consensus
syscon	misc	MAXNAMLEN				255			255			255			255			255			255			255			0			# unix consensus
syscon	misc	MAXQUOTAS				2			2			2			2			2			2			2			0			# unix consensus
syscon	misc	MEDIUM_ERROR				3			3			0			0			0			0			0			0
syscon	misc	MEDIUM_SCAN				56			56			0			0			0			0			0			0

syscon	misc	NBBY					8			8			8			8			8			8			8			0			# unix consensus
syscon	misc	NR_DQHASH				43			43			0			0			0			0			0			0
syscon	misc	NR_DQUOTS				0x0100			0x0100			0			0			0			0			0			0

syscon	misc	PERSISTENT_RESERVE_IN			94			94			0			0			0			0			0			0
syscon	misc	PERSISTENT_RESERVE_OUT			95			95			0			0			0			0			0			0

syscon	misc	PRELIM					1			1			1			1			1			1			1			0			# unix consensus
syscon	misc	REGTYPE					48			48			48			48			48			48			48			0			# unix consensus
syscon	misc	RES_PRF_CLASS				4			4			4			4			4			4			4			0			# unix consensus
syscon	misc	RHF_GUARANTEE_START_INIT		0x80			0x80			0			0			0			0			0			0
syscon	misc	RHF_NO_LIBRARY_REPLACEMENT		4			4			0			0			0			0			0			0

syscon	misc	RRQ					1			1			1			1			1			1			1			0			# unix consensus
syscon	misc	RTF_NOFORWARD				0x1000			0x1000			0			0			0			0			0			0
syscon	misc	RTF_NOPMTUDISC				0x4000			0x4000			0			0			0			0			0			0

syscon	misc	SARMAG					8			8			8			8			8			8			8			0			# unix consensus
syscon	misc	SEGSIZE					0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			0			# unix consensus
syscon	misc	SEND_DIAGNOSTIC				29			29			0			0			0			0			0			0
syscon	misc	SEND_VOLUME_TAG				182			182			0			0			0			0			0			0

syscon	misc	SET_LIMITS				51			51			0			0			0			0			0			0
syscon	misc	SET_WINDOW				36			36			0			0			0			0			0			0

syscon	misc	SFD_CLOEXEC				0x080000		0x080000		0			0			0			0			0			0
syscon	misc	SFD_NONBLOCK				0x0800			0x0800			0			0			0			0			0			0

syscon	misc	SUBCMDMASK				255			255			255			255			255			255			255			0			# unix consensus
syscon	misc	SUBCMDSHIFT				8			8			8			8			8			8			8			0			# unix consensus
syscon	misc	SYMTYPE					50			50			50			50			50			50			50			0			# unix consensus
syscon	misc	TGEXEC					8			8			8			8			8			8			8			0			# unix consensus
syscon	misc	TGREAD					0x20			0x20			0x20			0x20			0x20			0x20			0x20			0			# unix consensus
syscon	misc	TGWRITE					0x10			0x10			0x10			0x10			0x10			0x10			0x10			0			# unix consensus
syscon	misc	TMAGLEN					6			6			6			6			6			6			6			0			# unix consensus
syscon	misc	TOEXEC					1			1			1			1			1			1			1			0			# unix consensus
syscon	misc	TOREAD					4			4			4			4			4			4			4			0			# unix consensus
syscon	misc	TOWRITE					2			2			2			2			2			2			2			0			# unix consensus
syscon	misc	TRANSIENT				4			4			4			4			4			4			4			0			# unix consensus
syscon	misc	TSGID					0x0400			0x0400			0x0400			0x0400			0x0400			0x0400			0x0400			0			# unix consensus
syscon	misc	TSUID					0x0800			0x0800			0x0800			0x0800			0x0800			0x0800			0x0800			0			# unix consensus
syscon	misc	TSVTX					0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			0x0200			0			# unix consensus
syscon	misc	TUEXEC					0x40			0x40			0x40			0x40			0x40			0x40			0x40			0			# unix consensus
syscon	misc	TUREAD					0x0100			0x0100			0x0100			0x0100			0x0100			0x0100			0x0100			0			# unix consensus
syscon	misc	TUWRITE					0x80			0x80			0x80			0x80			0x80			0x80			0x80			0			# unix consensus
syscon	misc	TVERSLEN				2			2			2			2			2			2			2			0			# unix consensus
syscon	misc	WORD_BIT				0x20			0x20			0x20			0x20			0x20			0x20			0x20			0			# unix consensus
syscon	misc	WRQ					2			2			2			2			2			2			2			0			# unix consensus
syscon	misc	SIGEV_THREAD				2			2			3			3			2			0			2			0
syscon	misc	SIGEV_SIGNAL				0			0			1			1			1			0			1			0
syscon	misc	SIGEV_NONE				1			1			0			0			0			0			0			0

syscon	misc	BC_BASE_MAX				99			99			99			99			99			0x7fffffff		0x7fffffff		0
syscon	misc	BC_DIM_MAX				0x0800			0x0800			0x0800			0x0800			0x0800			0xffff			0xffff			0
syscon	misc	BC_SCALE_MAX				99			99			99			99			99			0x7fffffff		0x7fffffff		0
syscon	misc	BC_STRING_MAX				0x03e8			0x03e8			0x03e8			0x03e8			0x03e8			0x7fffffff		0x7fffffff		0

syscon	misc	ABORTED_COMMAND				11			11			0			0			0			0			0			0
syscon	misc	ACORE					0			0			8			8			8			8			8			0			# bsd consensus
syscon	misc	AFORK					0			0			1			1			1			1			1			0			# bsd consensus
syscon	misc	AIO_ALLDONE				2			2			1			1			3			0			0			0
syscon	misc	AIO_NOTCANCELED				1			1			4			4			2			0			0			0
syscon	misc	AIO_CANCELED				0			0			2			2			1			0			0			0

#	baud rates
#
#	group	name					GNU/Systemd		GNU/Systemd (Aarch64)	XNU's Not UNIX!		MacOS (Arm64)		FreeBSD			OpenBSD			NetBSD			The New Technology	Commentary
syscon	baud	B0					0			0			0			0			0			0			0			0
syscon	baud	B50					1			1			50			50			50			50			50			50
syscon	baud	B75					2			2			75			75			75			75			75			75
syscon	baud	B110					3			3			110			110			110			110			110			110
syscon	baud	B134					4			4			134			134			134			134			134			134
syscon	baud	B150					5			5			150			150			150			150			150			150
syscon	baud	B200					6			6			200			200			200			200			200			200
syscon	baud	B300					7			7			300			300			300			300			300			300
syscon	baud	B600					8			8			600			600			600			600			600			600
syscon	baud	B1200					9			9			1200			1200			1200			1200			1200			1200
syscon	baud	B1800					10			10			1800			1800			1800			1800			1800			1800
syscon	baud	B2400					11			11			2400			2400			2400			2400			2400			2400
syscon	baud	B4800					12			12			4800			4800			4800			4800			4800			4800
syscon	baud	B9600					13			13			9600			9600			9600			9600			9600			9600
syscon	baud	B19200					14			14			19200			19200			19200			19200			19200			19200
syscon	baud	B38400					15			15			38400			38400			38400			38400			38400			38400
syscon	baud	B57600					0x1001			0x1001			57600			57600			57600			57600			57600			57600
syscon	baud	B115200					0x1002			0x1002			115200			115200			115200			115200			115200			115200
syscon	baud	B230400					0x1003			0x1003			230400			230400			230400			230400			230400			230400
syscon	baud	B500000					0x1005			0x1005			500000			500000			500000			500000			500000			500000
syscon	baud	B576000					0x1006			0x1006			576000			576000			576000			576000			576000			576000
syscon	baud	B1000000				0x1008			0x1008			1000000			1000000			1000000			1000000			1000000			1000000
syscon	baud	B1152000				0x1009			0x1009			1152000			1152000			1152000			1152000			1152000			1152000
syscon	baud	B1500000				0x100a			0x100a			1500000			1500000			1500000			1500000			1500000			1500000
syscon	baud	B2000000				0x100b			0x100b			2000000			2000000			2000000			2000000			2000000			2000000
syscon	baud	B2500000				0x100c			0x100c			2500000			2500000			2500000			2500000			2500000			2500000
syscon	baud	B3000000				0x100d			0x100d			3000000			3000000			3000000			3000000			3000000			3000000
syscon	baud	B3500000				0x100e			0x100e			3500000			3500000			3500000			3500000			3500000			3500000
syscon	baud	B4000000				0x100f			0x100f			4000000			4000000			4000000			4000000			4000000			4000000

syscon	misc	WEOF					0xffffffff		0xffffffff		-1			-1			-1			-1			-1			-1			# bsd consensus (win fake)
syscon	misc	_LINUX_QUOTA_VERSION			2			2			0			0			0			0			0			0
syscon	misc	_SEM_SEMUN_UNDEFINED			1			1			0			0			0			0			0			0
syscon	misc	D_FMT					0x020029		0x020029		2			2			2			1			1			0
syscon	misc	D_T_FMT					0x020028		0x020028		1			1			1			0			0			0

syscon	misc	LOGIN_PROCESS				6			6			6			6			6			0			0			0
syscon	misc	LOGIN_NAME_MAX				0x0100			0x0100			0			0			0			0x20			0x20			0

syscon	misc	T_FMT					0x02002a		0x02002a		3			3			3			2			2			0
syscon	misc	T_FMT_AMPM				0x02002b		0x02002b		4			4			4			3			3			0

syscon	misc	UL_GETFSIZE				1			1			1			1			1			0			0			0
syscon	misc	UL_SETFSIZE				2			2			2			2			2			0			0			0

syscon	misc	XATTR_CREATE				1			1			2			2			0			0			0			0
syscon	misc	XATTR_REPLACE				2			2			4			4			0			0			0			0

# https://youtu.be/GUQUD3IMbb4?t=85
