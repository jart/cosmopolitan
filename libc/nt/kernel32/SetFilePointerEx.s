.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetFilePointerEx,SetFilePointerEx,0

	.text.windows
SetFilePointerEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFilePointerEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFilePointerEx,globl
	.previous
