#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RevertToSelf,RevertToSelf,0

	.text.windows
RevertToSelf:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_RevertToSelf(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	RevertToSelf,globl
	.previous
