#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetQueuedCompletionStatus,GetQueuedCompletionStatus,0

#ifdef __x86_64__
	.text.windows
GetQueuedCompletionStatus:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetQueuedCompletionStatus(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetQueuedCompletionStatus,globl
	.previous
#endif
