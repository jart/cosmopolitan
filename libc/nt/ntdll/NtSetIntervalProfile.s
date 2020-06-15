.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtSetIntervalProfile

	.text.windows
NtSetIntervalProfile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetIntervalProfile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSetIntervalProfile,globl
	.previous
