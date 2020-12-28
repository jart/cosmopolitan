.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetFileTime,SetFileTime,0

	.text.windows
SetFileTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileTime(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileTime,globl
	.previous
