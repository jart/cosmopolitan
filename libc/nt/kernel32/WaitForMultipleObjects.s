.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WaitForMultipleObjects,WaitForMultipleObjects,0

	.text.windows
WaitForMultipleObjects:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForMultipleObjects(%rip),%rax
	jmp	__sysv2nt
	.endfn	WaitForMultipleObjects,globl
	.previous
