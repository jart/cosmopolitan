.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFileSize,GetFileSize,0

	.text.windows
GetFileSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileSize(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileSize,globl
	.previous
