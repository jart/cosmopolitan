.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_AttachConsole,AttachConsole,0

	.text.windows
AttachConsole:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_AttachConsole(%rip)
	leave
	ret
	.endfn	AttachConsole,globl
	.previous
