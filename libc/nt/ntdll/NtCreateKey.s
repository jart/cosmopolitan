.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateKey

	.text.windows
NtCreateKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateKey(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtCreateKey,globl
	.previous
