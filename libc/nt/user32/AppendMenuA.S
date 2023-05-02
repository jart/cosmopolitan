#include "libc/nt/codegen.h"
.imp	user32,__imp_AppendMenuA,AppendMenuA,1515

#ifdef __x86_64__
	.text.windows
AppendMenuA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AppendMenuA(%rip),%rax
	jmp	__sysv2nt
	.endfn	AppendMenuA,globl
	.previous
#endif
