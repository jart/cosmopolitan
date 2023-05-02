#include "libc/nt/codegen.h"
.imp	user32,__imp_CallNextHookEx,CallNextHookEx,1535

#ifdef __x86_64__
	.text.windows
CallNextHookEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CallNextHookEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	CallNextHookEx,globl
	.previous
#endif
