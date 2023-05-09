#include "libc/nt/codegen.h"
.imp	kernel32,__imp_UpdateProcThreadAttribute,UpdateProcThreadAttribute,0

	.text.windows
UpdateProcThreadAttribute:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UpdateProcThreadAttribute(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	UpdateProcThreadAttribute,globl
	.previous
