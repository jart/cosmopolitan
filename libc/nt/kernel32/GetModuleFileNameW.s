.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetModuleFileNameW,GetModuleFileNameW,0

	.text.windows
GetModuleFileName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetModuleFileNameW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetModuleFileName,globl
	.previous
