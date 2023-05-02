#include "libc/nt/codegen.h"
.imp	kernel32,__imp_PostQueuedCompletionStatus,PostQueuedCompletionStatus,0

#ifdef __x86_64__
	.text.windows
PostQueuedCompletionStatus:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PostQueuedCompletionStatus(%rip),%rax
	jmp	__sysv2nt
	.endfn	PostQueuedCompletionStatus,globl
	.previous
#endif
