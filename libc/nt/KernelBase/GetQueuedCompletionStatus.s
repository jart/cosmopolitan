.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetQueuedCompletionStatus,GetQueuedCompletionStatus,695

	.text.windows
GetQueuedCompletionStatus:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetQueuedCompletionStatus(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetQueuedCompletionStatus,globl
	.previous
