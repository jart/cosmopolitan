.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetKeyState,GetKeyState,1866

	.text.windows
GetKeyState:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetKeyState(%rip)
	leave
	ret
	.endfn	GetKeyState,globl
	.previous
