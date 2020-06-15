.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FlushConsoleInputBuffer,FlushConsoleInputBuffer,388

	.text.windows
FlushConsoleInputBuffer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FlushConsoleInputBuffer(%rip)
	leave
	ret
	.endfn	FlushConsoleInputBuffer,globl
	.previous
