.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetSystemTimeAdjustment,GetSystemTimeAdjustment,0

	.text.windows
GetSystemTimeAdjustment:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemTimeAdjustment(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetSystemTimeAdjustment,globl
	.previous
