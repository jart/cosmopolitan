#include "libc/nt/ntdllimport.h"
.ntimp	RtlInitUnicodeString,RtlInitUnicodeString

#ifdef __x86_64__
	.text.windows
RtlInitUnicodeString:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlInitUnicodeString(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlInitUnicodeString,globl
	.previous
#endif
