.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQueryPerformanceCounter

	.text.windows
NtQueryPerformanceCounter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryPerformanceCounter(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtQueryPerformanceCounter,globl
	.previous
