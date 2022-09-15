.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SuspendThread,SuspendThread,0

	.text.windows
SuspendThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SuspendThread(%rip)
	leave
	ret
	.endfn	SuspendThread,globl
	.previous
