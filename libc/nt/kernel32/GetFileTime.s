.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFileTime,GetFileTime,0

	.text.windows
GetFileTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileTime(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileTime,globl
	.previous
