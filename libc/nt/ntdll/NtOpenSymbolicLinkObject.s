.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtOpenSymbolicLinkObject

	.text.windows
NtOpenSymbolicLinkObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenSymbolicLinkObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenSymbolicLinkObject,globl
	.previous
