#include "libc/nt/codegen.h"
.imp	user32,__imp_GetMenu,GetMenu,1881

	.text.windows
GetMenu:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetMenu(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetMenu,globl
	.previous