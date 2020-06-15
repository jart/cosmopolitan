.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtReadFile

	.text.windows
NtReadFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtReadFile(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtReadFile,globl
	.previous
