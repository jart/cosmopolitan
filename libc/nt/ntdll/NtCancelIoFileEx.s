.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCancelIoFileEx

	.text.windows
NtCancelIoFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCancelIoFileEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCancelIoFileEx,globl
	.previous
