.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateSection

	.text.windows
NtCreateSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateSection(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtCreateSection,globl
	.previous
