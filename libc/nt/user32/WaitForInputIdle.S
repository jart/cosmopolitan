#include "libc/nt/codegen.h"
.imp	user32,__imp_WaitForInputIdle,WaitForInputIdle,2495

#ifdef __x86_64__
	.text.windows
WaitForInputIdle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForInputIdle(%rip),%rax
	jmp	__sysv2nt
	.endfn	WaitForInputIdle,globl
	.previous
#endif
