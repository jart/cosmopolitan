#include "libc/nt/codegen.h"
.imp	user32,__imp_WaitForInputIdle,WaitForInputIdle,2495

	.text.windows
WaitForInputIdle:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForInputIdle(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WaitForInputIdle,globl
	.previous
