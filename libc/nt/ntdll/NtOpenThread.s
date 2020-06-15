.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtOpenThread

	.text.windows
NtOpenThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenThread,globl
	.previous
