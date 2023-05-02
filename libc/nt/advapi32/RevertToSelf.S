#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RevertToSelf,RevertToSelf,0

#ifdef __x86_64__
	.text.windows
RevertToSelf:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_RevertToSelf(%rip)
	leave
	ret
	.endfn	RevertToSelf,globl
	.previous
#endif
