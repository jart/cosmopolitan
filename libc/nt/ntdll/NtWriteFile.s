.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtWriteFile

	.text.windows
NtWriteFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtWriteFile(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtWriteFile,globl
	.previous
