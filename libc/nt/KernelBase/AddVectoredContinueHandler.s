.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_AddVectoredContinueHandler,AddVectoredContinueHandler,33

	.text.windows
AddVectoredContinueHandler:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AddVectoredContinueHandler(%rip),%rax
	jmp	__sysv2nt
	.endfn	AddVectoredContinueHandler,globl
	.previous
