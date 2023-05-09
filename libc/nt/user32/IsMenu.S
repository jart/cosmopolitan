#include "libc/nt/codegen.h"
.imp	user32,__imp_IsMenu,IsMenu,2070

	.text.windows
IsMenu:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_IsMenu(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	IsMenu,globl
	.previous
