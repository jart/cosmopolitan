#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateKeyedEvent,NtCreateKeyedEvent

#ifdef __x86_64__
	.text.windows
NtCreateKeyedEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateKeyedEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCreateKeyedEvent,globl
	.previous
#endif
