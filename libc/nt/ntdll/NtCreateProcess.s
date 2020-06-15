.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateProcess

	.text.windows
NtCreateProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateProcess(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtCreateProcess,globl
	.previous
