.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtSuspendThread

	.text.windows
NtSuspendThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSuspendThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSuspendThread,globl
	.previous
