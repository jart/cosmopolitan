.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetTickCount64,GetTickCount64,0

	.text.windows
GetTickCount64:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetTickCount64(%rip)
	leave
	ret
	.endfn	GetTickCount64,globl
	.previous
