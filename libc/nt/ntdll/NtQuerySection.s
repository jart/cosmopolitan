.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQuerySection

	.text.windows
NtQuerySection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQuerySection(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQuerySection,globl
	.previous
