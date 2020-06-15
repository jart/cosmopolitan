.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtRaiseHardError

	.text.windows
NtRaiseHardError:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtRaiseHardError(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtRaiseHardError,globl
	.previous
