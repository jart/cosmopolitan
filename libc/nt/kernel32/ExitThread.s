.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ExitThread,ExitThread,0

	.text.windows
ExitThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ExitThread(%rip)
	leave
	ret
	.endfn	ExitThread,globl
	.previous
