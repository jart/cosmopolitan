#include "libc/nt/codegen.h"
.imp	user32,__imp_DestroyIcon,DestroyIcon,1687

	.text.windows
DestroyIcon:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DestroyIcon(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	DestroyIcon,globl
	.previous
