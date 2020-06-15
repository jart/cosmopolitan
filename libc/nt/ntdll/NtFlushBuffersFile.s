.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtFlushBuffersFile

	.text.windows
NtFlushBuffersFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFlushBuffersFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtFlushBuffersFile,globl
	.previous
