.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtFreeVirtualMemory

	.text.windows
NtFreeVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFreeVirtualMemory(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtFreeVirtualMemory,globl
	.previous
