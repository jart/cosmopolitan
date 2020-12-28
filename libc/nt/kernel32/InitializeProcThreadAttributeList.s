.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_InitializeProcThreadAttributeList,InitializeProcThreadAttributeList,0

	.text.windows
InitializeProcThreadAttributeList:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InitializeProcThreadAttributeList(%rip),%rax
	jmp	__sysv2nt
	.endfn	InitializeProcThreadAttributeList,globl
	.previous
