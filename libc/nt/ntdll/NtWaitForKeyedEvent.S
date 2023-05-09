#include "libc/nt/ntdllimport.h"
.ntimp	NtWaitForKeyedEvent,NtWaitForKeyedEvent

	.text.windows
NtWaitForKeyedEvent:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtWaitForKeyedEvent(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtWaitForKeyedEvent,globl
	.previous
