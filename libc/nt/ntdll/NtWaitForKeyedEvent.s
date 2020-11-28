.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtWaitForKeyedEvent

	.text.windows
NtWaitForKeyedEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtWaitForKeyedEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtWaitForKeyedEvent,globl
	.previous
