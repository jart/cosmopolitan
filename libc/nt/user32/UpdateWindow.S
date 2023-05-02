#include "libc/nt/codegen.h"
.imp	user32,__imp_UpdateWindow,UpdateWindow,2474

#ifdef __x86_64__
	.text.windows
UpdateWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_UpdateWindow(%rip)
	leave
	ret
	.endfn	UpdateWindow,globl
	.previous
#endif
