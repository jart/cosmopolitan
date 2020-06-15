.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateThread

	.text.windows
NtCreateThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateThread(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtCreateThread,globl
	.previous
