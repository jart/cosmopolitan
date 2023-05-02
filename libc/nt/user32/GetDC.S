#include "libc/nt/codegen.h"
.imp	user32,__imp_GetDC,GetDC,1830

#ifdef __x86_64__
	.text.windows
GetDC:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetDC(%rip)
	leave
	ret
	.endfn	GetDC,globl
	.previous
#endif
