#include "libc/nt/codegen.h"
.imp	gdi32,__imp_CreateCompatibleDC,CreateCompatibleDC,1064

#ifdef __x86_64__
	.text.windows
CreateCompatibleDC:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CreateCompatibleDC(%rip)
	leave
	ret
	.endfn	CreateCompatibleDC,globl
	.previous
#endif
