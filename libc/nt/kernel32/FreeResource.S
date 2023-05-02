#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FreeResource,FreeResource,0

#ifdef __x86_64__
	.text.windows
FreeResource:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FreeResource(%rip)
	leave
	ret
	.endfn	FreeResource,globl
	.previous
#endif
