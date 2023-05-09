#include "libc/nt/ntdllimport.h"
.ntimp	RtlInitUnicodeString,RtlInitUnicodeString

	.text.windows
RtlInitUnicodeString:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlInitUnicodeString(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlInitUnicodeString,globl
	.previous
