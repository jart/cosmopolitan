#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetLastError,SetLastError,0

#ifdef __x86_64__
	.text.windows
SetLastError:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetLastError(%rip)
	leave
	ret
	.endfn	SetLastError,globl
	.previous
#endif
