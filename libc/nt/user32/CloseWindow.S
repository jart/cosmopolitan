#include "libc/nt/codegen.h"
.imp	user32,__imp_CloseWindow,CloseWindow,1591

#ifdef __x86_64__
	.text.windows
CloseWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CloseWindow(%rip)
	leave
	ret
	.endfn	CloseWindow,globl
	.previous
#endif
