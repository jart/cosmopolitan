.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtOpenKey

	.text.windows
NtOpenKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenKey(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenKey,globl
	.previous
