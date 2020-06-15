.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtFlushVirtualMemory

	.text.windows
NtFlushVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFlushVirtualMemory(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtFlushVirtualMemory,globl
	.previous
