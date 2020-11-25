.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_SaveDC,SaveDC,1815

	.text.windows
SaveDC:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SaveDC(%rip)
	leave
	ret
	.endfn	SaveDC,globl
	.previous
