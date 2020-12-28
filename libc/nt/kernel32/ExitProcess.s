.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ExitProcess,ExitProcess,0

	.text.windows
ExitProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ExitProcess(%rip)
	leave
	ret
	.endfn	ExitProcess,globl
	.previous
