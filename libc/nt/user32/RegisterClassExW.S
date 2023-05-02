#include "libc/nt/codegen.h"
.imp	user32,__imp_RegisterClassExW,RegisterClassExW,2250

#ifdef __x86_64__
	.text.windows
RegisterClassEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegisterClassExW(%rip)
	leave
	ret
	.endfn	RegisterClassEx,globl
	.previous
#endif
