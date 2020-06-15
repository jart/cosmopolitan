.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryVirtualMemory

	.text.windows
NtQueryVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryVirtualMemory,globl
	.previous
