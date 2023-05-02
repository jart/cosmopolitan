#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegDisableReflectionKey,RegDisableReflectionKey,1631

#ifdef __x86_64__
	.text.windows
RegDisableReflectionKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegDisableReflectionKey(%rip)
	leave
	ret
	.endfn	RegDisableReflectionKey,globl
	.previous
#endif
