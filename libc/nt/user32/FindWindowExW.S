#include "libc/nt/codegen.h"
.imp	user32,__imp_FindWindowExW,FindWindowExW,1783

	.text.windows
FindWindowEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindWindowExW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	FindWindowEx,globl
	.previous