.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtOpenThreadToken

	.text.windows
NtOpenThreadToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenThreadToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenThreadToken,globl
	.previous
