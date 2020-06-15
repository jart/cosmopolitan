.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtFlushInstructionCache

	.text.windows
NtFlushInstructionCache:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFlushInstructionCache(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtFlushInstructionCache,globl
	.previous
