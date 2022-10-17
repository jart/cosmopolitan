.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ClearCommBreak,ClearCommBreak,0

	.text.windows
ClearCommBreak:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ClearCommBreak(%rip)
	leave
	ret
	.endfn	ClearCommBreak,globl
	.previous
