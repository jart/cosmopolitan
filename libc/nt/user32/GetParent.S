#include "libc/nt/codegen.h"
.imp	user32,__imp_GetParent,GetParent,1906

#ifdef __x86_64__
	.text.windows
GetParent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetParent(%rip)
	leave
	ret
	.endfn	GetParent,globl
	.previous
#endif
