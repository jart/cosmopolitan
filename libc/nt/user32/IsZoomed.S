#include "libc/nt/codegen.h"
.imp	user32,__imp_IsZoomed,IsZoomed,2092

	.text.windows
IsZoomed:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_IsZoomed(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	IsZoomed,globl
	.previous
