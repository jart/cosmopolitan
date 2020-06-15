.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtOpenProcess

	.text.windows
NtOpenProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenProcess(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenProcess,globl
	.previous
