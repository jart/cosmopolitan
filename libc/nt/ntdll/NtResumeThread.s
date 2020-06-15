.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtResumeThread

	.text.windows
NtResumeThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtResumeThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtResumeThread,globl
	.previous
