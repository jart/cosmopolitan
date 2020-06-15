.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtOpenSection

	.text.windows
NtOpenSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenSection(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenSection,globl
	.previous
