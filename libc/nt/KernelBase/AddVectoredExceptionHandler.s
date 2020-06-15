.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_AddVectoredExceptionHandler,AddVectoredExceptionHandler,34

	.text.windows
AddVectoredExceptionHandler:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AddVectoredExceptionHandler(%rip),%rax
	jmp	__sysv2nt
	.endfn	AddVectoredExceptionHandler,globl
	.previous
