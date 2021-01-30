.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFinalPathNameByHandleA,GetFinalPathNameByHandleA,0

	.text.windows
GetFinalPathNameByHandleA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFinalPathNameByHandleA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFinalPathNameByHandleA,globl
	.previous
