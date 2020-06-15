.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FreeConsole,FreeConsole,399

	.text.windows
FreeConsole:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_FreeConsole(%rip)
	leave
	ret
	.endfn	FreeConsole,globl
	.previous
