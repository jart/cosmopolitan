.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RevertToSelf,RevertToSelf,0

	.text.windows
RevertToSelf:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_RevertToSelf(%rip)
	leave
	ret
	.endfn	RevertToSelf,globl
	.previous
