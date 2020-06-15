.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtSetEvent

	.text.windows
NtSetEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSetEvent,globl
	.previous
