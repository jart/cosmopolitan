.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtUnmapViewOfSection

	.text.windows
NtUnmapViewOfSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtUnmapViewOfSection(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtUnmapViewOfSection,globl
	.previous
