.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQuerySymbolicLinkObject

	.text.windows
NtQuerySymbolicLinkObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQuerySymbolicLinkObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQuerySymbolicLinkObject,globl
	.previous
