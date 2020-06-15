.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtEnumerateValueKey

	.text.windows
NtEnumerateValueKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtEnumerateValueKey(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtEnumerateValueKey,globl
	.previous
