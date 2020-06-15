.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtMapViewOfSection

	.text.windows
NtMapViewOfSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtMapViewOfSection(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtMapViewOfSection,globl
	.previous
