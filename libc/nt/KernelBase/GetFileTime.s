.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetFileTime,GetFileTime,554

	.text.windows
GetFileTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileTime(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileTime,globl
	.previous
