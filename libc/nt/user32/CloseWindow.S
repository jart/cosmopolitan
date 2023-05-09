#include "libc/nt/codegen.h"
.imp	user32,__imp_CloseWindow,CloseWindow,1591

	.text.windows
CloseWindow:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CloseWindow(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	CloseWindow,globl
	.previous
