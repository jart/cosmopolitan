.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryFullAttributesFile

	.text.windows
NtQueryFullAttributesFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryFullAttributesFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQueryFullAttributesFile,globl
	.previous
