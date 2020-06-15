.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtDelayExecution

	.text.windows
NtDelayExecution:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtDelayExecution(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtDelayExecution,globl
	.previous
