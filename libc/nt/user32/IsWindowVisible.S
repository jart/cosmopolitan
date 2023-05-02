#include "libc/nt/codegen.h"
.imp	user32,__imp_IsWindowVisible,IsWindowVisible,2090

#ifdef __x86_64__
	.text.windows
IsWindowVisible:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_IsWindowVisible(%rip)
	leave
	ret
	.endfn	IsWindowVisible,globl
	.previous
#endif
