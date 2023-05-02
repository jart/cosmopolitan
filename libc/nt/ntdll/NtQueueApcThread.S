#include "libc/nt/ntdllimport.h"
.ntimp	NtQueueApcThread,NtQueueApcThread

#ifdef __x86_64__
	.text.windows
NtQueueApcThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueueApcThread(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueueApcThread,globl
	.previous
#endif
