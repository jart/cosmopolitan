.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateIoCompletion

	.text.windows
NtCreateIoCompletion:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateIoCompletion(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCreateIoCompletion,globl
	.previous
