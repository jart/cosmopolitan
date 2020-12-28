.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_AllocConsole,AllocConsole,0

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
