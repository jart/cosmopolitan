#include "libc/nt/codegen.h"
.imp	user32,__imp_MessageBoxW,MessageBoxW,2170

	.text.windows
MessageBox:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MessageBoxW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	MessageBox,globl
	.previous
