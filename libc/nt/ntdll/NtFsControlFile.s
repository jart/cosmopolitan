.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtFsControlFile

	.text.windows
NtFsControlFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFsControlFile(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtFsControlFile,globl
	.previous
