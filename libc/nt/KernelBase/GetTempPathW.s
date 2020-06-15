.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetTempPathW,GetTempPathW,763

	.text.windows
GetTempPath:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetTempPathW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetTempPath,globl
	.previous
