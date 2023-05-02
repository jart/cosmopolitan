#include "libc/nt/codegen.h"
.imp	user32,__imp_IsMenu,IsMenu,2070

#ifdef __x86_64__
	.text.windows
IsMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_IsMenu(%rip)
	leave
	ret
	.endfn	IsMenu,globl
	.previous
#endif
