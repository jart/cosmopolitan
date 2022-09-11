.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateSemaphoreW,CreateSemaphoreW,0

	.text.windows
CreateSemaphore:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateSemaphoreW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateSemaphore,globl
	.previous
