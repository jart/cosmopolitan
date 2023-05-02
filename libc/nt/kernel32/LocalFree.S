#include "libc/nt/codegen.h"
.imp	kernel32,__imp_LocalFree,LocalFree,0

#ifdef __x86_64__
	.text.windows
LocalFree:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_LocalFree(%rip)
	leave
	ret
	.endfn	LocalFree,globl
	.previous
#endif
