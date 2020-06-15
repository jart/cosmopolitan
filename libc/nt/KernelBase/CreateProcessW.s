.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateProcessW,CreateProcessW,212

	.text.windows
CreateProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateProcessW(%rip),%rax
	jmp	__sysv2nt10
	.endfn	CreateProcess,globl
	.previous
