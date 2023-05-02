#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GlobalFree,GlobalFree,0

#ifdef __x86_64__
	.text.windows
GlobalFree:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GlobalFree(%rip)
	leave
	ret
	.endfn	GlobalFree,globl
	.previous
#endif
