.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WaitForSingleObject,WaitForSingleObject,0

	.text.windows
__WaitForSingleObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForSingleObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	__WaitForSingleObject,globl
	.previous
