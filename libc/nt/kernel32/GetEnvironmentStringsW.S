#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetEnvironmentStringsW,GetEnvironmentStringsW,0

#ifdef __x86_64__
	.text.windows
GetEnvironmentStrings:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetEnvironmentStringsW(%rip)
	leave
	ret
	.endfn	GetEnvironmentStrings,globl
	.previous
#endif
