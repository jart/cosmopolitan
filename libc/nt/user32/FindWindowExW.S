#include "libc/nt/codegen.h"
.imp	user32,__imp_FindWindowExW,FindWindowExW,1783

#ifdef __x86_64__
	.text.windows
FindWindowEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindWindowExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindWindowEx,globl
	.previous
#endif
