.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtOpenFile

	.text.windows
NtOpenFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtOpenFile,globl
	.previous
