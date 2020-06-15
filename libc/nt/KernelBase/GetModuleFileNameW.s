.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetModuleFileNameW,GetModuleFileNameW,602

	.text.windows
GetModuleFileName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetModuleFileNameW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetModuleFileName,globl
	.previous
