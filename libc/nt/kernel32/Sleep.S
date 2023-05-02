#include "libc/nt/codegen.h"
.imp	kernel32,__imp_Sleep,Sleep,0

#ifdef __x86_64__
	.text.windows
Sleep:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_Sleep(%rip)
	leave
	ret
	.endfn	Sleep,globl
	.previous
#endif
