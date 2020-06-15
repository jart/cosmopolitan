.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateProfile

	.text.windows
NtCreateProfile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateProfile(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtCreateProfile,globl
	.previous
