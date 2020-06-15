.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQuerySystemInformation

	.text.windows
NtQuerySystemInformation:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQuerySystemInformation(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQuerySystemInformation,globl
	.previous
