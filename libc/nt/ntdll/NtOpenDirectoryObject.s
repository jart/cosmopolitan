.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtOpenDirectoryObject

	.text.windows
NtOpenDirectoryObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenDirectoryObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenDirectoryObject,globl
	.previous
