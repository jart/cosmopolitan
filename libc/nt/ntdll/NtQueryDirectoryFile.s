.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryDirectoryFile

	.text.windows
NtQueryDirectoryFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryDirectoryFile(%rip),%rax
	jmp	__sysv2nt12
	.endfn	NtQueryDirectoryFile,globl
	.previous
