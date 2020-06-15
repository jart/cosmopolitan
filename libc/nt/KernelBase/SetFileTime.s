.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetFileTime,SetFileTime,1514

	.text.windows
SetFileTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileTime(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileTime,globl
	.previous
