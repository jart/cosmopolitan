.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtReadVirtualMemory

	.text.windows
NtReadVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtReadVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtReadVirtualMemory,globl
	.previous
