.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtSetInformationThread

	.text.windows
NtSetInformationThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetInformationThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSetInformationThread,globl
	.previous
