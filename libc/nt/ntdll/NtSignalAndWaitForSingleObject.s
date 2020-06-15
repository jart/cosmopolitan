.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtSignalAndWaitForSingleObject

	.text.windows
NtSignalAndWaitForSingleObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSignalAndWaitForSingleObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSignalAndWaitForSingleObject,globl
	.previous
