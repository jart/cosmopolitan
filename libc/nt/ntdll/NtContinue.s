.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtContinue

	.text.windows
NtContinue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtContinue(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtContinue,globl
	.previous
