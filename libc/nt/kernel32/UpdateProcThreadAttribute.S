#include "libc/nt/codegen.h"
.imp	kernel32,__imp_UpdateProcThreadAttribute,UpdateProcThreadAttribute,0

#ifdef __x86_64__
	.text.windows
UpdateProcThreadAttribute:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UpdateProcThreadAttribute(%rip),%rax
	jmp	__sysv2nt8
	.endfn	UpdateProcThreadAttribute,globl
	.previous
#endif
