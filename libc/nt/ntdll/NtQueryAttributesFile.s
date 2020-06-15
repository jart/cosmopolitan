.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryAttributesFile

	.text.windows
NtQueryAttributesFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryAttributesFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQueryAttributesFile,globl
	.previous
