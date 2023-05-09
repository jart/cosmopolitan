#include "libc/nt/codegen.h"
.imp	user32,__imp_ShowCursor,ShowCursor,2412

	.text.windows
ShowCursor:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ShowCursor(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	ShowCursor,globl
	.previous
