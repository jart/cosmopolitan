.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ReleaseSemaphore,ReleaseSemaphore,0

	.text.windows
ReleaseSemaphore:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReleaseSemaphore(%rip),%rax
	jmp	__sysv2nt
	.endfn	ReleaseSemaphore,globl
	.previous
