.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlQueryEnvironmentVariable

	.text.windows
RtlQueryEnvironmentVariable:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlQueryEnvironmentVariable(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlQueryEnvironmentVariable,globl
	.previous
