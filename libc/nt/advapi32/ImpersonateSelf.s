.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_ImpersonateSelf,ImpersonateSelf,0

	.text.windows
ImpersonateSelf:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ImpersonateSelf(%rip)
	leave
	ret
	.endfn	ImpersonateSelf,globl
	.previous
