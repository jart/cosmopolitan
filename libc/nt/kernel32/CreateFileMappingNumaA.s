.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateFileMappingNumaA,CreateFileMappingNumaA,198

	.text.windows
CreateFileMappingNumaA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateFileMappingNumaA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CreateFileMappingNumaA,globl
	.previous
