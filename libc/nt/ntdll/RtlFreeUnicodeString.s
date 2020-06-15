.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlFreeUnicodeString

	.text.windows
RtlFreeUnicodeString:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlFreeUnicodeString(%rip)
	leave
	ret
	.endfn	RtlFreeUnicodeString,globl
	.previous
