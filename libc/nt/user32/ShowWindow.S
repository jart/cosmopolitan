#include "libc/nt/codegen.h"
.imp	user32,__imp_ShowWindow,ShowWindow,2417

	.text.windows
ShowWindow:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ShowWindow(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	ShowWindow,globl
	.previous
