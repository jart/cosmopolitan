.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtEnumerateKey

	.text.windows
NtEnumerateKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtEnumerateKey(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtEnumerateKey,globl
	.previous
