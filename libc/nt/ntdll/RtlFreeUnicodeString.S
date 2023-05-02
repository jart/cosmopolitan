#include "libc/nt/ntdllimport.h"
.ntimp	RtlFreeUnicodeString,RtlFreeUnicodeString

#ifdef __x86_64__
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
#endif
