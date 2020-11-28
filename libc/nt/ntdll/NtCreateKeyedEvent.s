.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateKeyedEvent

	.text.windows
NtCreateKeyedEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateKeyedEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCreateKeyedEvent,globl
	.previous
