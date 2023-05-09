#include "libc/nt/codegen.h"
.imp	user32,__imp_CreateIconIndirect,CreateIconIndirect,1618

	.text.windows
CreateIconIndirect:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CreateIconIndirect(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	CreateIconIndirect,globl
	.previous
