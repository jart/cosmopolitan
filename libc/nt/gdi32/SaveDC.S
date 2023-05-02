#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SaveDC,SaveDC,1815

#ifdef __x86_64__
	.text.windows
SaveDC:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SaveDC(%rip)
	leave
	ret
	.endfn	SaveDC,globl
	.previous
#endif
