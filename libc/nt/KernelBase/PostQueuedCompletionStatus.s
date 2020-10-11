.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_PostQueuedCompletionStatus,PostQueuedCompletionStatus,1221

	.text.windows
PostQueuedCompletionStatus:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PostQueuedCompletionStatus(%rip),%rax
	jmp	__sysv2nt
	.endfn	PostQueuedCompletionStatus,globl
	.previous
