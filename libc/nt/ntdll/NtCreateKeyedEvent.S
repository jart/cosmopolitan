#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateKeyedEvent,NtCreateKeyedEvent

	.text.windows
NtCreateKeyedEvent:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateKeyedEvent(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCreateKeyedEvent,globl
	.previous
