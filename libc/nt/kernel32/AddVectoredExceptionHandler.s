.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_AddVectoredExceptionHandler,AddVectoredExceptionHandler,0

	.text.windows
AddVectoredExceptionHandler:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AddVectoredExceptionHandler(%rip),%rax
	jmp	__sysv2nt
	.endfn	AddVectoredExceptionHandler,globl
	.previous
