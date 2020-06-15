.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtDuplicateObject

	.text.windows
NtDuplicateObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtDuplicateObject(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtDuplicateObject,globl
	.previous
