.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WaitNamedPipeA,WaitNamedPipeA,1509

	.text.windows
WaitNamedPipeA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitNamedPipeA(%rip),%rax
	jmp	__sysv2nt
	.endfn	WaitNamedPipeA,globl
	.previous
