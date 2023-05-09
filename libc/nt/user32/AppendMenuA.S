#include "libc/nt/codegen.h"
.imp	user32,__imp_AppendMenuA,AppendMenuA,1515

	.text.windows
AppendMenuA:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AppendMenuA(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	AppendMenuA,globl
	.previous
