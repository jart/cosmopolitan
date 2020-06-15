.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateEvent

	.text.windows
NtCreateEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateEvent(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtCreateEvent,globl
	.previous
