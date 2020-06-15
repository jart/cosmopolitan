.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReleaseSemaphore,ReleaseSemaphore,1395

	.text.windows
ReleaseSemaphore:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReleaseSemaphore(%rip),%rax
	jmp	__sysv2nt
	.endfn	ReleaseSemaphore,globl
	.previous
