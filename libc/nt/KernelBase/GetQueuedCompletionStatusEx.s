.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetQueuedCompletionStatusEx,GetQueuedCompletionStatusEx,696

	.text.windows
GetQueuedCompletionStatusEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetQueuedCompletionStatusEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetQueuedCompletionStatusEx,globl
	.previous
