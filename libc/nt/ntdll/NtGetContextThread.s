.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtGetContextThread

	.text.windows
NtGetContextThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtGetContextThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtGetContextThread,globl
	.previous
