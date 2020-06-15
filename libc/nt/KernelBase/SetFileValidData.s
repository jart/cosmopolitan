.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetFileValidData,SetFileValidData,1515

	.text.windows
SetFileValidData:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileValidData(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileValidData,globl
	.previous
