#include "libc/nt/codegen.h"
.imp	user32,__imp_DefWindowProcW,DefWindowProcW,174

#ifdef __x86_64__
	.text.windows
DefWindowProc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DefWindowProcW(%rip),%rax
	jmp	__sysv2nt
	.endfn	DefWindowProc,globl
	.previous
#endif
