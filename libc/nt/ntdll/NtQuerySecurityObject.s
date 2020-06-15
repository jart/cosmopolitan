.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQuerySecurityObject

	.text.windows
NtQuerySecurityObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQuerySecurityObject(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQuerySecurityObject,globl
	.previous
