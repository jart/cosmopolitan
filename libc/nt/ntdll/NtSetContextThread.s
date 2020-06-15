.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtSetContextThread

	.text.windows
NtSetContextThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetContextThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSetContextThread,globl
	.previous
