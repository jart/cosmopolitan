.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtSetValueKey

	.text.windows
NtSetValueKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetValueKey(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtSetValueKey,globl
	.previous
