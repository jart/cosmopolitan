.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlInitUnicodeString

	.text.windows
RtlInitUnicodeString:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlInitUnicodeString(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlInitUnicodeString,globl
	.previous
