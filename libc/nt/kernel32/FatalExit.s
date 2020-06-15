.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FatalExit,FatalExit,364

	.text.windows
FatalExit:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FatalExit(%rip)
	leave
	ret
	.endfn	FatalExit,globl
	.previous
