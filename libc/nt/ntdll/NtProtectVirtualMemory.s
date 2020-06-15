.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtProtectVirtualMemory

	.text.windows
NtProtectVirtualMemory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtProtectVirtualMemory(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtProtectVirtualMemory,globl
	.previous
