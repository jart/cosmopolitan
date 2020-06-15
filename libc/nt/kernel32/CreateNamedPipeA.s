.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateNamedPipeA,CreateNamedPipeA,219

	.text.windows
CreateNamedPipeA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateNamedPipeA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CreateNamedPipeA,globl
	.previous
