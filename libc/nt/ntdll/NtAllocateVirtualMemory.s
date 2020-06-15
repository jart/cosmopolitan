.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtAllocateVirtualMemory

	.text.windows
NtAllocateVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtAllocateVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtAllocateVirtualMemory,globl
	.previous
