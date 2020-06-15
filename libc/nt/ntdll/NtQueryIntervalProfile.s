.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryIntervalProfile

	.text.windows
NtQueryIntervalProfile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryIntervalProfile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQueryIntervalProfile,globl
	.previous
