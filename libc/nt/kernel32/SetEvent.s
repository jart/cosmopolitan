.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetEvent,SetEvent,0

	.text.windows
SetEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetEvent(%rip)
	leave
	ret
	.endfn	SetEvent,globl
	.previous
