.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueueApcThread

	.text.windows
NtQueueApcThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueueApcThread(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueueApcThread,globl
	.previous
