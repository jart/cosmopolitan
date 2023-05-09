#include "libc/nt/codegen.h"
.imp	user32,__imp_MoveWindow,MoveWindow,2176

	.text.windows
MoveWindow:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MoveWindow(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	MoveWindow,globl
	.previous
