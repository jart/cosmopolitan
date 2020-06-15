.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCancelIoFile

	.text.windows
NtCancelIoFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCancelIoFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCancelIoFile,globl
	.previous
