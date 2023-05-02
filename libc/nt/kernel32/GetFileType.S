#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetFileType,GetFileType,0

#ifdef __x86_64__
	.text.windows
GetFileType:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetFileType(%rip)
	leave
	ret
	.endfn	GetFileType,globl
	.previous
#endif
