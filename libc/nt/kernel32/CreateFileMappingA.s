.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateFileMappingA,CreateFileMappingA,196

	.text.windows
CreateFileMappingA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateFileMappingA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CreateFileMappingA,globl
	.previous
