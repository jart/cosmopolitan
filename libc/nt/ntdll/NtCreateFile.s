.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateFile

	.text.windows
NtCreateFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateFile(%rip),%rax
	jmp	__sysv2nt12
	.endfn	NtCreateFile,globl
	.previous
