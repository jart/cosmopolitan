#include "libc/nt/codegen.h"
.imp	user32,__imp_DestroyIcon,DestroyIcon,1687

#ifdef __x86_64__
	.text.windows
DestroyIcon:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DestroyIcon(%rip)
	leave
	ret
	.endfn	DestroyIcon,globl
	.previous
#endif
