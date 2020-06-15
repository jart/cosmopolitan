.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtTerminateProcess

	.text.windows
NtTerminateProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtTerminateProcess(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtTerminateProcess,globl
	.previous
