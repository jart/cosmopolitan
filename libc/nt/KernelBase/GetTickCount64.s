.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetTickCount64,GetTickCount64,780

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
