#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateEvent,NtCreateEvent

#ifdef __x86_64__
	.text.windows
NtCreateEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateEvent(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtCreateEvent,globl
	.previous
#endif
