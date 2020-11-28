.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtReleaseKeyedEvent

	.text.windows
NtReleaseKeyedEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtReleaseKeyedEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtReleaseKeyedEvent,globl
	.previous
