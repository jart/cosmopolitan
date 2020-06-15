.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetProcessWorkingSetSizeEx,SetProcessWorkingSetSizeEx,1538

	.text.windows
SetProcessWorkingSetSizeEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetProcessWorkingSetSizeEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetProcessWorkingSetSizeEx,globl
	.previous
