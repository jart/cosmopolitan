.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateTimer

	.text.windows
NtCreateTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateTimer(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCreateTimer,globl
	.previous
