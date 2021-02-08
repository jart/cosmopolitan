.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetProcessImageFileNameW,GetProcessImageFileNameW,677

	.text.windows
GetProcessImageFileName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessImageFileNameW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessImageFileName,globl
	.previous
