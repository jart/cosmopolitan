.include "o/libc/nt/ntdllimport.inc"
.ntimp	ZwAreMappedFilesTheSame

	.text.windows
ZwAreMappedFilesTheSame:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ZwAreMappedFilesTheSame(%rip),%rax
	jmp	__sysv2nt
	.endfn	ZwAreMappedFilesTheSame,globl
	.previous
