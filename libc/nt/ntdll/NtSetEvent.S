#include "libc/nt/ntdllimport.h"
.ntimp	NtSetEvent,NtSetEvent

#ifdef __x86_64__
	.text.windows
NtSetEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSetEvent,globl
	.previous
#endif
