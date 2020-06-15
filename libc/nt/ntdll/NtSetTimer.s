.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtSetTimer

	.text.windows
NtSetTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetTimer(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtSetTimer,globl
	.previous
