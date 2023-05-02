#include "libc/nt/ntdllimport.h"
.ntimp	RtlConvertSidToUnicodeString,RtlConvertSidToUnicodeString

#ifdef __x86_64__
	.text.windows
RtlConvertSidToUnicodeString:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlConvertSidToUnicodeString(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlConvertSidToUnicodeString,globl
	.previous
#endif
