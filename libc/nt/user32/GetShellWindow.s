.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetShellWindow,GetShellWindow,1950

	.text.windows
GetShellWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetShellWindow(%rip)
	leave
	ret
	.endfn	GetShellWindow,globl
	.previous
