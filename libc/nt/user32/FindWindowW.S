#include "libc/nt/codegen.h"
.imp	user32,__imp_FindWindowW,FindWindowW,1784

	.text.windows
FindWindow:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindWindowW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	FindWindow,globl
	.previous
