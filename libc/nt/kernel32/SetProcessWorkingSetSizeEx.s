.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetProcessWorkingSetSizeEx,SetProcessWorkingSetSizeEx,0

	.text.windows
SetProcessWorkingSetSizeEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetProcessWorkingSetSizeEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetProcessWorkingSetSizeEx,globl
	.previous
