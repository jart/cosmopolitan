.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_InitializeProcThreadAttributeList,InitializeProcThreadAttributeList,859

	.text.windows
InitializeProcThreadAttributeList:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InitializeProcThreadAttributeList(%rip),%rax
	jmp	__sysv2nt
	.endfn	InitializeProcThreadAttributeList,globl
	.previous
