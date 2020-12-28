.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFileSizeEx,GetFileSizeEx,0

	.text.windows
GetFileSizeEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileSizeEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileSizeEx,globl
	.previous
