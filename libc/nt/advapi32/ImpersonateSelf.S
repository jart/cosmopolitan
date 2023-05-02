#include "libc/nt/codegen.h"
.imp	advapi32,__imp_ImpersonateSelf,ImpersonateSelf,0

#ifdef __x86_64__
	.text.windows
ImpersonateSelf:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ImpersonateSelf(%rip)
	leave
	ret
	.endfn	ImpersonateSelf,globl
	.previous
#endif
