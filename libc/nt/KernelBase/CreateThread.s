.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateThread,CreateThread,224

	.text.windows
CreateThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateThread(%rip),%rax
	jmp	__sysv2nt6
	.endfn	CreateThread,globl
	.previous
