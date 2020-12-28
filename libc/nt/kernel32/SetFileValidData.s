.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetFileValidData,SetFileValidData,0

	.text.windows
SetFileValidData:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileValidData(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileValidData,globl
	.previous
