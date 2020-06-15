.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryObject

	.text.windows
NtQueryObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryObject(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryObject,globl
	.previous
