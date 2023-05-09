#include "libc/nt/codegen.h"
.imp	user32,__imp_CreateWindowExW,CreateWindowExW,1625

	.text.windows
CreateWindowEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWindowExW(%rip),%rax
	jmp	__sysv2nt12
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CreateWindowEx,globl
	.previous
