#include "libc/nt/ntdllimport.h"
.ntimp	NtWaitForKeyedEvent,NtWaitForKeyedEvent

#ifdef __x86_64__
	.text.windows
NtWaitForKeyedEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtWaitForKeyedEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtWaitForKeyedEvent,globl
	.previous
#endif
