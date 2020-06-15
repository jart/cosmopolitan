.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryInformationThread

	.text.windows
NtQueryInformationThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryInformationThread(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryInformationThread,globl
	.previous
