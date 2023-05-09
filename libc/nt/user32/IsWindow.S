#include "libc/nt/codegen.h"
.imp	user32,__imp_IsWindow,IsWindow,2084

	.text.windows
IsWindow:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_IsWindow(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	IsWindow,globl
	.previous
