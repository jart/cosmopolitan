#include "libc/nt/codegen.h"
.imp	user32,__imp_PostQuitMessage,PostQuitMessage,2206

#ifdef __x86_64__
	.text.windows
PostQuitMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_PostQuitMessage(%rip)
	leave
	ret
	.endfn	PostQuitMessage,globl
	.previous
#endif
