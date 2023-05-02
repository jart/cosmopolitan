#include "libc/nt/codegen.h"
.imp	user32,__imp_CreateWindowExW,CreateWindowExW,1625

#ifdef __x86_64__
	.text.windows
CreateWindowEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWindowExW(%rip),%rax
	jmp	__sysv2nt12
	.endfn	CreateWindowEx,globl
	.previous
#endif
