.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtSetInformationFile

	.text.windows
NtSetInformationFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetInformationFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtSetInformationFile,globl
	.previous
