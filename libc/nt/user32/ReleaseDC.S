#include "libc/nt/codegen.h"
.imp	user32,__imp_ReleaseDC,ReleaseDC,2280

#ifdef __x86_64__
	.text.windows
ReleaseDC:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReleaseDC(%rip),%rax
	jmp	__sysv2nt
	.endfn	ReleaseDC,globl
	.previous
#endif
