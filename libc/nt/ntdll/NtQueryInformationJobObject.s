.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryInformationJobObject

	.text.windows
NtQueryInformationJobObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryInformationJobObject(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryInformationJobObject,globl
	.previous
