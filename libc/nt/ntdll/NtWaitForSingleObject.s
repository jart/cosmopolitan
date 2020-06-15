.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtWaitForSingleObject

	.text.windows
NtWaitForSingleObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtWaitForSingleObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtWaitForSingleObject,globl
	.previous
