.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryInformationToken

	.text.windows
NtQueryInformationToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryInformationToken(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryInformationToken,globl
	.previous
