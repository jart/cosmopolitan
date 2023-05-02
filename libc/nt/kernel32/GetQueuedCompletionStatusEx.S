#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetQueuedCompletionStatusEx,GetQueuedCompletionStatusEx,0

#ifdef __x86_64__
	.text.windows
GetQueuedCompletionStatusEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetQueuedCompletionStatusEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetQueuedCompletionStatusEx,globl
	.previous
#endif
