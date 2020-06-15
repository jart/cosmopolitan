.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_AllocConsole,AllocConsole,37

	.text.windows
AllocConsole:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_AllocConsole(%rip)
	leave
	ret
	.endfn	AllocConsole,globl
	.previous
