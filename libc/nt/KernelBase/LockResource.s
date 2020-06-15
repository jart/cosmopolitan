.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_LockResource,LockResource,995

	.text.windows
LockResource:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_LockResource(%rip)
	leave
	ret
	.endfn	LockResource,globl
	.previous
