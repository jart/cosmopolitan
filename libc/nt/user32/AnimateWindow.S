#include "libc/nt/codegen.h"
.imp	user32,__imp_AnimateWindow,AnimateWindow,1513

	.text.windows
AnimateWindow:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AnimateWindow(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	AnimateWindow,globl
	.previous
