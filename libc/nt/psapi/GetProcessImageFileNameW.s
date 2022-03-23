.include "o/libc/nt/codegen.inc"
.imp	psapi,__imp_GetProcessImageFileNameW,GetProcessImageFileNameW,0

	.text.windows
GetProcessImageFileName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessImageFileNameW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessImageFileName,globl
	.previous
