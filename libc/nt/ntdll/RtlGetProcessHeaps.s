.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlGetProcessHeaps

	.text.windows
RtlGetProcessHeaps:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlGetProcessHeaps(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlGetProcessHeaps,globl
	.previous
