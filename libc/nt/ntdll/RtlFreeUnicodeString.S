#include "libc/nt/ntdllimport.h"
.ntimp	RtlFreeUnicodeString,RtlFreeUnicodeString

	.text.windows
RtlFreeUnicodeString:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlFreeUnicodeString(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	RtlFreeUnicodeString,globl
	.previous
