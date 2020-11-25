.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_ReleaseCapture,ReleaseCapture,2279

	.text.windows
ReleaseCapture:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_ReleaseCapture(%rip)
	leave
	ret
	.endfn	ReleaseCapture,globl
	.previous
