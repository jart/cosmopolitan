.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtRaiseException

	.text.windows
NtRaiseException:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtRaiseException(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtRaiseException,globl
	.previous
