.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryValueKey

	.text.windows
NtQueryValueKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryValueKey(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryValueKey,globl
	.previous
