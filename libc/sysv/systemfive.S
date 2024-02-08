/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/strace.internal.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/thread/pt.internal.h"

#define SIG_IGN 1

/*                                           ▄▄▄
                       ▄▄▄                    ▀▓▓▒▄
                     ▄▓▒▒░                      ▀▓▒▒▒▄
                   ▄▓▓▓▒▀              ▄▄▄▄      ▒▓▒▒░▒▄
                  ▄▓▓▓▒▓        ▄▄▓██▓▓▓▓▒▒▒▒▓▓▄▄▓▓▒▒▒░░▒
                  ▓▓▓▓▒▒▒▄▄  ░▒█▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▓▒░░▒░
                  ██▓▓▓▒▒░░▒▒▒▒▓▓▓▓▓▓▒▓▒░▒▒░▀▒▒▒▒░▀░▒▒▒░▒
                  ▓▓▓▓▓▓▓▒▒▒▒▒▒▓▓▒▓▓▒▒▒░▒▒░░  ░▒▒░  ░▒▒▒▒
                   ▀▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒░░▒░░    ░▒▒  ░ ▀▒▒
                     ▀▓▓█▓▓▓▓▓▓▓▓▓▓▒▒░░▒▒░░   ░░░▓░ ▓░░░▒
                       ▀▀█▓███▓▓▓▓▓▒▒░░░▒░░  ░█▓░█▓░█▓▓▄▒░
                          ░▓██▓▓▓▓▓▒▒░░░▒░░  ░████▓▒▓█▓▀░▀▄
                          ░▓██▓▓▓▓▓▒▒▒░░░▒░░  ▒██▓▒▒▒▒▒▒░░░▒
                           ████▓▓▓▓▓▒▒▒▒▒▒▒▒▒░░▒▓▓▒░░░░▒░░░▒░ ░░░░░
                           ░▓███▓▓▓▓▓▒▒░░░░░░░▒▒▒▒▒▒▒▒▒▒▒░░░ ░░░░░   ░
                             ▓███▓▓▓▓▓▒▓▒▒▒▒░░░░░░░░░▒▓▒▒░▀ ░░░  ░░░░░
                              ▀▒██▓▓▓▓▒▒▒▓▓▓▓▒▒▒▒▒▒▒▓▀▀░    ░░░░░░░░░     ░
                                 ▓▓▓▓▓▓▓▒▓▒▒▒▒▓▓▓▒▀░ ░░░░░▄░░░  ░░░  ░░░░░░
                                 ▓▓▓▒▒▒▒▒▒▒▒▒▒▒▓     █▓▒░░▒░░░░ ░░░░░░░░
                                ▄▓▓▓▒▒▒▒▒░░░░░░░▒▄▄▄░▒▓▓▒▒░▀░
                               ░▓█▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▒░░░▒  besiyata
                               ▓▓█▓▓▒▓▓▓▒▒▒░░░░░░▒▓▓▓▓▒▒▒▒▒░   dishmaya
                               ▓▓█▓▓▓▓▓▓▒▒▒░░░░░░░▒▓▓▒▀▀▀
                               ▓▓██▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▀
                                █▓▓█▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▀
                               ▒▓▓▓▓▀░░▒▓▓▓▓▓▓▓▓▒▒░░▒
                              ▄▓▓▀░░░▄▓▓▓▓▒▒▒▒▒░░░░▄░
                             ▄███▄▄▓▓▓▓▓▓▓▒▒▒▒▒░░▒▒░
                           ▄▓▓▓█▓█▓▓███▓▓▓▓▓▓▓▓▓▓▓░
                       ▄░▓▓▓▓▓▓▀▒▓▓▓▒▒▓▒░░░▒▓▒░░░▓
               ▄▄▄░▒▓▓▓▓▓▓░▀▀   ▓▓▒░▓▒▒▒▒▒▒▒▒▒▒▄░░▀▀░░ ▄▄▄▄
     ▄▄▄▒▒▓▓█▓▓▓▓▓▀▀▀▀▀        ▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▀░░▀░░▒▒▒░░░   ░░░░░
  ▄▓▓▓▒▀▀                      ▓▒▓▓▓▓▓▒▒▒▒▒▒▒▒▓░░░       ▒▒▒░░░░░░░░▒
  █▓▓▒      ▄▄▄                  ▀▓▒▓▒▒▒▓▓▓▓▓▓▒▒▒░░░░░░░░░▒▒░░░░░░░
   ▀▓▓▓▓▒▄▄▒▒▒▒▒▒▄▄                    ▀▀▀▀░░▒▒▒▒░░░░░░
       ▀▀▀▓▓▓▓▒▒▒▒▒▓▓▄▄
╔────────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bell system five » system call support                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

	.initbss 300,_init_systemfive

//	Performs System Five System Call.
//
//	Cosmopolitan is designed to delegate all function calls into the
//	Linux, FreeBSD, OpenBSD, and XNU kernels via this function, with
//	few exceptions. This function should generally only be called by
//	generated thunks in the libc/sysv/syscalls/ directory.
//
//	It's safe to call this function on Windows, where it will always
//	return -1 with errno == ENOSYS. Further note that -1 is the only
//	return value that means error, a common anti-pattern is to check
//	for values less than 0 (which is more problematic on 32-bit).
//
//	It is important to consider that system calls are one order of a
//	magnitude more expensive than normal function calls. For example
//	getpid() on Linux usually takes 500ns, and cached i/o calls will
//	take 1µs or more. So we don't need to inline them like Chromium.
//
//	Another thing to consider is that BSDs only loosely follow the
//	System Five ABI for the SYSCALL instruction. For example Linux
//	always follows the six argument limit but the FreeBSD sendfile
//	system call accepts a seventh argument that is passed on stack
//	and OpenBSD modifies functions like mmap so that the sixth arg
//	is passed on the stack. There's also the carry flag convention
//	that XNU, FreeBSD, and OpenBSD inherited from 386BSD aka Jolix
//
//	@param	%rax function ordinal supplied by jump slot
//	@param	%rdi,%rsi,%rdx,%rcx,%r8,%r9 and rest on stack
//	@return	%rax:%rdx is result, or -1 w/ errno on error
//	@clob	%rcx,%r10,%r11
//	@see	syscalls.sh
__systemfive:
	.quad	0
	.endobj	__systemfive,globl,hidden
__pid:	.quad	0
	.endobj	__pid,globl,hidden
	.previous

systemfive_cp:
	push	%rbp
	mov	%rsp,%rbp		// so backtraces work
systemfive_cancellable:			// our pthread_cancel() miracle code
	cmpb	$0,__tls_enabled(%rip)	// inspired by the musl libc design!
	je	1f			// we handle linux and bsd together!
	mov	%fs:0,%r10		// CosmoTib::tib_self
	mov	0x28(%r10),%r10		// CosmoTib::tib_pthread
	test	%r10,%r10		// is it a posix thread?
	jz	1f			// it's spawn() probably
	testb	$PT_NOCANCEL,0x00(%r10)	// PosixThread::flags
	jnz	1f			// canceler no cancelling
#if IsModeDbg()
	testb	$PT_INCANCEL,0x00(%r10)
	jz	5f
#endif
	cmpl	$0,0x04(%r10)		// PosixThread::cancelled
	jne	systemfive_cancel	// we will tail call below
1:	mov	%rcx,%r10		// move the fourth argument
	clc				// no cancellable system calls exist
	syscall				// that have 7+ args on the bsd OSes
systemfive_cancellable_end:		// i/o calls park here for long time
	pop	%rbp
	jnc	2f
	neg	%rax			// turns bsd errno to system v errno
2:	cmp	$-4095,%rax		// but we still check again on eintr
	jae	3f			// branch because system call failed
	ret				// done if the system call succeeded
3:	neg	%eax			// now examine the nature of failure
	cmp	EINTR(%rip),%eax	// did the SIGTHR cancel our IO call
	jne	systemfive_errno	// werent interrupted by OnSigCancel
	cmpb	$0,__tls_enabled(%rip)	// make sure it's safe to grab %fs:0
	je	systemfive_errno	// tls is disabled we can't continue
	mov	%fs:0,%rcx		// CosmoTib::tib_self
	mov	0x28(%rcx),%rcx		// CosmoTib::tib_pthread
	test	%rcx,%rcx		// is it a posix thread?
	jz	systemfive_errno	// it's spawn() probably
	testb	$PT_NOCANCEL,0x00(%rcx)	// PosixThread::flags
	jnz	systemfive_errno	// cancellation is disabled
	cmpl	$0,0x04(%rcx)		// PosixThread::cancelled
	je	systemfive_errno	// we aren't actually cancelled
	jmp	4f			// now we are in fact cancelled
systemfive_cancel:			// SIGTHR will jump here too
	pop	%rbp
4:	jmp	_pthread_cancel_ack	// tail call
	.weak	_pthread_cancel_ack	// must be linked if we're cancelled
#if IsModeDbg()
not_a_cancellation_point:		// need BEGIN/END_CANCELLATION_POINT
	nop
	.weak	report_cancellation_point
5:	ezlea	report_cancellation_point,cx
	test	%rcx,%rcx
	jz	6f
	call	*%rcx
6:	ud2
	nop
#endif
	.globl	systemfive_cancellable_end
	.globl	systemfive_cancellable
	.globl	systemfive_cancel
	.endfn	systemfive_cp

.Lanchorpoint:
#if SupportsLinux() || SupportsMetal()
systemfive_linux:
	and	$0xfff,%eax		// remove nonlinux bits from ordinal
	cmp	$0xfff,%eax		// checks if unsupported by platform
	je	systemfive_enosys	// never taken branches cost nothing
	btr	$11,%eax		// 0x800 means a call is cancellable
	jc	systemfive_cp		// it is handled by the holiest code
	mov	%rcx,%r10		// syscall instruction clobbers %rcx
	push	%rbp			// linux never reads args from stack
	mov	%rsp,%rbp		// having frame will help backtraces
	syscall				// this is known as a context switch
	pop	%rbp			// next we check to see if it failed
	cmp	$-4095,%rax		// system five nexgen32e abi § a.2.1
	jae	systemfive_error	// encodes errno as neg return value
	ret
	.endfn	systemfive_linux,globl,hidden
systemfive_error:
	neg	%eax
//	𝑠𝑙𝑖𝑑𝑒
	.endfn	systemfive_error,globl,hidden
#endif
systemfive_errno:
	xchg	%eax,%ecx
	.errno
	mov	%ecx,(%rax)		// normalize to c library convention
	push	$-1			// negative one is only error result
	pop	%rax			// the push pop is to save code size
	ret
	.endfn	systemfive_errno,globl,hidden
systemfive_enosys:
	mov	ENOSYS(%rip),%eax
	jmp	systemfive_errno
	.endfn	systemfive_enosys,globl,hidden
#if SupportsNetbsd()
systemfive_netbsd:
	shr	$4*13,%rax
	jmp	systemfive_bsdscrub
	.endfn	systemfive_netbsd,globl,hidden
#endif
#if SupportsOpenbsd()
systemfive_openbsd:
	shr	$4*10,%rax
	jmp	systemfive_bsdscrub
	.endfn	systemfive_openbsd,globl,hidden
#endif
#if SupportsFreebsd()
systemfive_freebsd:
	shr	$4*7,%rax
	movzwl	%ax,%eax
//	𝑠𝑙𝑖𝑑𝑒
	.endfn	systemfive_freebsd,globl,hidden
#endif
#if SupportsBsd()
systemfive_bsdscrub:
	and	$0xfff,%eax
//	𝑠𝑙𝑖𝑑𝑒
	.endfn	systemfive_bsdscrub,globl,hidden
systemfive_bsd:
	cmp	$0xfff,%ax
	je	systemfive_enosys
	btr	$11,%eax		// checks/reset the 800 cancellable bit
	jc	systemfive_cp
	mov	%rcx,%r10
	syscall				// bsd will need arg on stack sometimes
	jc	systemfive_errno	// bsd sets carry flag if %rax is errno
	ret
	.endfn	systemfive_bsd
#endif
#if SupportsXnu()
systemfive_xnu:
//	0x?????????2153???		// how syscalls.sh encodes xnu ordinals
//	           │└┴┴┐
//	           │   ├┬┐
//	0x0000000002000153		// how xnu wants ordinals to be encoded
	mov	%eax,%r11d
	and	$0x0f000000,%r11d
	shl	$8,%eax
	shr	$20,%eax
	or	%r11d,%eax
	jmp	systemfive_bsd
	.endfn	systemfive_xnu,globl,hidden
#endif
	.previous

//	Initializes System Five system call support.
//
//	  (1) Extracts parameters passed by kernel
//	  (2) Detects OS without issuing system calls
//	  (3) Unpacks magnums from libc/sysv/consts.sh
//	  (4) Replaces stack with one we control
//
//	@param	%r15 is auxv
//	@note	OpenBSD devs: let us know if you start using auxv
	.init.start 300,_init_systemfive
	push	%rbx
	push	%rsi
//	Detect the operating system.
	mov	__hostos(%rip),%eax
#if SupportsWindows() || SupportsXnu() || SupportsFreebsd()
//	set by	libc/ape.S		for XNU
//	set by	libc/crt/crt.S		for XNU/FreeBSD
//	set by	libc/nt/winmain.greg.c	for New Technology
	test	%eax,%eax
	jnz	_init_systemfive_detected	// os is already known
#endif
#if SupportsOpenbsd()
	cmpq	$0,(%r15)			// OpenBSD has no auxv
	jnz	0f
	mov	$_HOSTOPENBSD,%al
	jmp	_init_systemfive_detected
0:
#endif
#if SupportsNetbsd()
	xor	%ecx,%ecx
0:	cmpq	$2014,(%r15,%rcx,8)		// NetBSD's AT_EXECFN
	jne	1f
	mov	$_HOSTNETBSD,%al
	jmp	_init_systemfive_detected
1:	cmpq	$0,(%r15,%rcx,8)
	lea	2(%ecx),%ecx
	jnz	0b
2:
#endif
	mov	$_HOSTLINUX,%al
_init_systemfive_detected:
	mov	%eax,__hostos(%rip)
	bsr	%eax,%eax
	mov	$_init_systemfive_jmptab,%ebx
	movzbl	(%rbx,%rax),%eax
	lea	(%rbx,%rax),%eax
	jmp	*%rax
_init_systemfive_jmptab:
	.byte	_init_systemfive_linux-_init_systemfive_jmptab
	.byte	_init_systemfive_metal-_init_systemfive_jmptab
	.byte	_init_systemfive_windows-_init_systemfive_jmptab
	.byte	_init_systemfive_xnu-_init_systemfive_jmptab
	.byte	_init_systemfive_openbsd-_init_systemfive_jmptab
	.byte	_init_systemfive_freebsd-_init_systemfive_jmptab
	.byte	_init_systemfive_netbsd-_init_systemfive_jmptab
	.endobj	_init_systemfive_jmptab
_init_systemfive_linux:
#if SupportsLinux()
	pushb	systemfive_linux-.Lanchorpoint
	mov	$syscon_linux,%esi
	jmp	_init_systemfive_os
#endif
	.endfn	_init_systemfive_linux
_init_systemfive_metal:
#if SupportsMetal()
	pushb	systemfive_enosys-.Lanchorpoint
	mov	$syscon_linux,%esi
	jmp	_init_systemfive_os
#endif
	.endfn	_init_systemfive_metal
_init_systemfive_windows:
#if SupportsWindows()
	pushb	systemfive_enosys-.Lanchorpoint
	mov	$syscon_windows,%esi
	jmp	_init_systemfive_os
#endif
	.endfn	_init_systemfive_windows
_init_systemfive_xnu:
#if SupportsXnu()
	pushb	systemfive_xnu-.Lanchorpoint
	mov	$syscon_xnu,%esi
	jmp	_init_systemfive_os
#endif
	.endfn	_init_systemfive_xnu
_init_systemfive_openbsd:
#if SupportsOpenbsd()
	pushb	systemfive_openbsd-.Lanchorpoint
	mov	$syscon_openbsd,%esi
	jmp	_init_systemfive_os
#endif
	.endfn	_init_systemfive_openbsd
_init_systemfive_freebsd:
#if SupportsFreebsd()
	pushb	systemfive_freebsd-.Lanchorpoint
	mov	$syscon_freebsd,%esi
	jmp	_init_systemfive_os
#endif
	.endfn	_init_systemfive_freebsd
_init_systemfive_netbsd:
#if SupportsNetbsd()
	pushb	systemfive_netbsd-.Lanchorpoint
	mov	$syscon_netbsd,%esi
//	𝑠𝑙𝑖𝑑𝑒
#endif
	.endfn	_init_systemfive_netbsd
_init_systemfive_os:
	pop	%rax
	add	$.Lanchorpoint,%eax
	stosq	#→ __systemfive
//	𝑠𝑙𝑖𝑑𝑒
	.endfn	_init_systemfive_os
_init_systemfive_magnums:
	push	%rdi
	mov	$syscon_start,%edi
2:	cmp	$syscon_end,%edi
	jnb	5f
	xor	%ebx,%ebx
	xor	%ecx,%ecx
	xor	%edx,%edx
3:	lodsb						// decodes uleb128
	movzbl	%al,%edx
	and	$127,%dl
	shl	%cl,%rdx
	or	%rdx,%rbx
	add	$7,%cl
	test	$128,%al
	jnz	3b
	xchg	%rbx,%rax
	stosq
#if SYSDEBUG
	inc	%r8d
#endif
	jmp	2b
5:	nop
#if SYSDEBUG
	push	%rdi
	push	%rsi
	push	%r8
	call	__describe_os
	mov	%rax,%rdx
	pop	%r8
	.weak	__stracef
	ezlea	__stracef,ax
	test	%rax,%rax
	jz	5f
	ezlea	.Llog,di
	mov	%r8d,%esi
	push	%rax
	call	*%rax
	pop	%rax
5:	pop	%rsi
	pop	%rdi
#endif /* DEBUGSYS */
	pop	%rdi
	pop	%rsi
	pop	%rbx
//	𝑠𝑙𝑖𝑑𝑒
	.endfn	_init_systemfive_magnums
#if SupportsSystemv()
_init_systemfive_pid:
	ezlea	__hostos,cx
	mov	(%rcx),%al
	mov	(%rdi),%eax
	testb	$_HOSTWINDOWS|_HOSTMETAL,(%rcx)
	jnz	1f
	mov	__NR_getpid(%rip),%eax
	syscall
1:	stosq
	.endfn	_init_systemfive_pid
#endif
#if SupportsBsd() && !defined(TINY)
_init_systemfive_sigsys:
	testb	IsBsd()					// BSDs will trap SIGSYS!
	jz	1f					// We want ENOSYS instead
	push	%rdi					// XNU removed some calls
	push	%rsi					// in past, so this makes
	xor	%eax,%eax				// troubleshooting easier
	push	%rax					// but it's non-essential
	push	%rax
	push	%rax
	push	%rax
	push	%rax
	push	$SIG_IGN				// sigaction_meta size 48
	mov	__NR_sigaction(%rip),%eax		// mag
	mov	SIGSYS(%rip),%edi			// sig
	mov	%rsp,%rsi				// new
	xor	%edx,%edx				// old
	mov	$8,%r10d				// for linux
	xor	%r8d,%r8d				// for netbsd
	syscall
	add	$6*8,%rsp
	pop	%rsi
	pop	%rdi
1:	.endfn	_init_systemfive_sigsys
#endif
_init_systemfive_done:
	nop
	.init.end 300,_init_systemfive,globl,hidden

#if SYSDEBUG
	.rodata.str1.1
.Llog:	.ascii	STRACE_PROLOGUE
	.ascii	"bell system five system call support"
	.asciz	" %'u magnums loaded on %s\n"
	.previous
#endif /* DEBUGSYS */
