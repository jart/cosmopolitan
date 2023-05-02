#include "libc/nt/codegen.h"
.imp	user32,__imp_MessageBoxW,MessageBoxW,2170

#ifdef __x86_64__
	.text.windows
MessageBox:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MessageBoxW(%rip),%rax
	jmp	__sysv2nt
	.endfn	MessageBox,globl
	.previous
#endif
