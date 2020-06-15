.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtWriteVirtualMemory

	.text.windows
NtWriteVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtWriteVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtWriteVirtualMemory,globl
	.previous
