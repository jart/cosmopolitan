#include "libc/nt/codegen.h"
.imp	user32,__imp_CreateIconIndirect,CreateIconIndirect,1618

#ifdef __x86_64__
	.text.windows
CreateIconIndirect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CreateIconIndirect(%rip)
	leave
	ret
	.endfn	CreateIconIndirect,globl
	.previous
#endif
