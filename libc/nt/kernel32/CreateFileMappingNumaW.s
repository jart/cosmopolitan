.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateFileMappingNumaW,CreateFileMappingNumaW,0

	.text.windows
CreateFileMappingNuma:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateFileMappingNumaW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CreateFileMappingNuma,globl
	.previous
