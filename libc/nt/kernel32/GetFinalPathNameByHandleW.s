.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFinalPathNameByHandleW,GetFinalPathNameByHandleW,0

	.text.windows
GetFinalPathNameByHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFinalPathNameByHandleW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFinalPathNameByHandle,globl
	.previous
