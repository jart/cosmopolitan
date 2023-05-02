#include "libc/nt/codegen.h"
.imp	user32,__imp_IsIconic,IsIconic,2067

#ifdef __x86_64__
	.text.windows
IsIconic:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_IsIconic(%rip)
	leave
	ret
	.endfn	IsIconic,globl
	.previous
#endif
