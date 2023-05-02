#include "libc/nt/ntdllimport.h"
.ntimp	NtReleaseKeyedEvent,NtReleaseKeyedEvent

#ifdef __x86_64__
	.text.windows
NtReleaseKeyedEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtReleaseKeyedEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtReleaseKeyedEvent,globl
	.previous
#endif
