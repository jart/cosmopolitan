#include "libc/nt/codegen.h"
.imp	user32,__imp_ReleaseCapture,ReleaseCapture,2279

#ifdef __x86_64__
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
#endif
