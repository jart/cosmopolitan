.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlConvertSidToUnicodeString

	.text.windows
RtlConvertSidToUnicodeString:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlConvertSidToUnicodeString(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlConvertSidToUnicodeString,globl
	.previous
