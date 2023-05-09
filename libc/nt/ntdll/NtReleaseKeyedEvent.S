#include "libc/nt/ntdllimport.h"
.ntimp	NtReleaseKeyedEvent,NtReleaseKeyedEvent

	.text.windows
NtReleaseKeyedEvent:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtReleaseKeyedEvent(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtReleaseKeyedEvent,globl
	.previous
