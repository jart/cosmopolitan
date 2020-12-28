.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetTempPathW,GetTempPathW,0

	.text.windows
GetTempPath:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetTempPathW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetTempPath,globl
	.previous
