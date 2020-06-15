.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtTerminateThread

	.text.windows
NtTerminateThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtTerminateThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtTerminateThread,globl
	.previous
