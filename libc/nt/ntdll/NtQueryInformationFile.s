.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryInformationFile

	.text.windows
NtQueryInformationFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryInformationFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryInformationFile,globl
	.previous
