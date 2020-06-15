.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetProcessWorkingSetSize,SetProcessWorkingSetSize,1356

	.text.windows
SetProcessWorkingSetSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetProcessWorkingSetSize(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetProcessWorkingSetSize,globl
	.previous
