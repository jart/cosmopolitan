.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateDirectoryObject

	.text.windows
NtCreateDirectoryObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateDirectoryObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCreateDirectoryObject,globl
	.previous
