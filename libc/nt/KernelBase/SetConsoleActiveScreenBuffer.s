.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetConsoleActiveScreenBuffer,SetConsoleActiveScreenBuffer,1472

	.text.windows
SetConsoleActiveScreenBuffer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetConsoleActiveScreenBuffer(%rip)
	leave
	ret
	.endfn	SetConsoleActiveScreenBuffer,globl
	.previous
